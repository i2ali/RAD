//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicer.h"

using namespace RAD;

const wchar_t gUser32Name[] = L"User32.dll";
const char* gSlicerControlName = (LPCSTR)2010;

#define SET_HRESULT_FROM_GLE() do { DWORD e = ::GetLastError(); if (ERROR_SUCCESS == e) { hResult = E_UNEXPECTED; } else { hResult = HRESULT_FROM_WIN32(e); } } while (0, 0)

// Copied from dwmcoretypes.h
typedef struct
{
    UINT Offset;
    UINT Spare0;
    LUID AdapterLuid;
    UINT64 hmonAssoc;
} MAGN_ADAPTERTEXTURE_OFFSET;

// Copied from dwmcoretypes.h
typedef struct
{
    UINT cSharedTextures;
    DXGI_FORMAT DxgiFormat;
    UINT Width;
    UINT Height;
    MAGN_ADAPTERTEXTURE_OFFSET TexturesBitsOffset[MAG_MAX_ADAPTER];
} MAGN_ADAPTERTEXTURE_OFFSETS;


DwmSlicer::DwmSlicer(void)
    : mRefCount(0),
      mD2DDevice(nullptr),
      mD2DDeviceContext(nullptr),
      mSlicing(false),
      mWindowClass(0),
      mWindowHandle(nullptr),
      mModuleHandle(::GetModuleHandleW(nullptr))
{
    ::InitializeSRWLock(&this->mFunctionSyncLock);
}


HRESULT DwmSlicer::Initialize(
    const std::vector<std::shared_ptr<IDXGIAdapter>>& argAdapters,
    const std::vector<std::shared_ptr<ID3D11Device>>& argDevices,
    const std::shared_ptr<ID2D1Device>& argD2DDevice)
{
    HRESULT hResult = NOERROR;

    if (true == argAdapters.empty() ||
        true == argDevices.empty() ||
        nullptr == argD2DDevice)
    {
        return E_INVALIDARG;
    }

    // Dynamically load the SlicerControl API
    HMODULE user32 = ::LoadLibraryW(gUser32Name);

    if (nullptr == user32)
    {
        return HRESULT_FROM_WIN32(::GetLastError());
    }

    SlicerControlApi slicerControl = reinterpret_cast<SlicerControlApi>(::GetProcAddress(user32, gSlicerControlName));

    if (nullptr == slicerControl)
    {
        ::FreeLibrary(user32);
        return CO_E_NOT_SUPPORTED;
    }

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    this->mUser32 = user32;
    this->mSlicerControl = slicerControl;

    // Save the DirectX objects locally
    #ifdef DBG
    std::wcout << std::endl << L"Adapters: " << std::endl;
    #endif

    for (UINT i = 0; i < argAdapters.size(); ++i)
    {
        #ifdef DBG
        DXGI_ADAPTER_DESC desc = {};
        argAdapters[i]->GetDesc(&desc);
        std::wcout << L"\t" << desc.Description << std::endl;
        #endif

        this->mAdapterToDeviceMap[argAdapters[i]] = argDevices[i];
    }

    this->mD2DDevice = argD2DDevice;

    // Get a device context from the Direct2D device
    ID2D1DeviceContext* d2d1DeviceContext = nullptr;
    hResult = argD2DDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        &d2d1DeviceContext);

    this->mD2DDeviceContext = std::shared_ptr<ID2D1DeviceContext>(d2d1DeviceContext, release_iunknown_deleter);

    if (SUCCEEDED(hResult))
    {
        // Register a class for the slicer context window
        std::wstringstream classNameStream;
        std::wstring       className;

        DWORD processId = ::GetCurrentProcessId();
        DWORD threadId = ::GetCurrentThreadId();

        classNameStream << L"DwmSlicerControlWindow<" << processId << L"-" << threadId << L"-" << reinterpret_cast<UINT>(this) << L">";
        className = classNameStream.str();

        WNDCLASSEXW wndClass;
        wndClass.cbClsExtra = 0;
        wndClass.cbSize = sizeof(wndClass);
        wndClass.cbWndExtra = 0;
        wndClass.hbrBackground = nullptr;
        wndClass.hCursor = nullptr;
        wndClass.hIcon = nullptr;
        wndClass.hIconSm = nullptr;
        wndClass.hInstance = mModuleHandle;
        wndClass.lpfnWndProc = ::DefWindowProcW;
        wndClass.lpszClassName = className.c_str();
        wndClass.lpszMenuName = nullptr;
        wndClass.style = 0;

        ::SetLastError(ERROR_SUCCESS);
        this->mWindowClass = ::RegisterClassExW(&wndClass);

        if (this->mWindowClass > 0)
        {
            // We have to create a message window for the slicer context
            ::SetLastError(ERROR_SUCCESS);
            this->mWindowHandle = ::CreateWindowExW(
                0,
                MAKEINTATOM(this->mWindowClass),
                L"DwmSlicerControlWindow",
                0,
                0,
                0,
                64,
                64,
                nullptr,
                nullptr,
                this->mModuleHandle,
                nullptr);

            if (nullptr != this->mWindowHandle)
            {
                // Initialize the slicer
                ::SetLastError(ERROR_SUCCESS);
                BOOL retval = this->mSlicerControl(
                    this->mWindowHandle,
                    SlicerContextInitialize,
                    nullptr,
                    0);

                if (FALSE == retval)
                {
                    SET_HRESULT_FROM_GLE();

                    ::DestroyWindow(this->mWindowHandle);
                    this->mWindowHandle = nullptr;

                    ::UnregisterClassW(MAKEINTATOM(this->mWindowClass), this->mModuleHandle);
                    this->mWindowClass = 0;
                }
            }
            else
            {
                SET_HRESULT_FROM_GLE();

                ::UnregisterClassW(MAKEINTATOM(this->mWindowClass), this->mModuleHandle);
                this->mWindowClass = 0;
            }
        }
        else
        {
            SET_HRESULT_FROM_GLE();
        }
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


DwmSlicer::~DwmSlicer(void)
{
    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    // Make sure a slicer context window was created
    if (nullptr != this->mWindowHandle)
    {
        // If we are slicing, stop before
        if (true == this->mSlicing)
        {
            SLICER_CAPTURE_RESULT captureResult = {};
            this->mSlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult));
        }

        // Deinitialize the slicer context
        this->mSlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0);

        // Destroy the slicer context window
        ::DestroyWindow(this->mWindowHandle);
        this->mWindowHandle = nullptr;

        // Unregister the window class
        ::UnregisterClassW(MAKEINTATOM(this->mWindowClass), this->mModuleHandle);
        this->mWindowClass = 0;
    }

    if (nullptr != this->mUser32)
    {
        ::FreeLibrary(this->mUser32);
        this->mUser32 = nullptr;
        this->mSlicerControl = nullptr;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE DwmSlicer::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);
    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE DwmSlicer::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE DwmSlicer::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (nullptr == ppvObj)
    {
        return E_INVALIDARG;
    }
    else if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ISlicer))
    {
        *ppvObj = static_cast<ISlicer*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(IDwmSlicer))
    {
        *ppvObj = static_cast<IDwmSlicer*>(this);
        this->AddRef();
        return NOERROR;
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }
}


//
// Public Properties
//

bool DwmSlicer::IsSlicing(void)
{
    return this->mSlicing;
}


//
// Public Methods
//

HRESULT STDMETHODCALLTYPE DwmSlicer::CalculateMaxFramesThatCanBeCaptured(
    RECT argAreaToCapture,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    __out UINT* argNumberOfFramesThatCanBeCaptured)
{
    return this->InternalStartCapture(
        argAreaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip,
        argChannel,
        argNumberOfFramesThatCanBeCaptured,
        true);
}


HRESULT DwmSlicer::EndCapture(
    __out UINT* argFramesCaptured,
    __out ID2D1Bitmap1** argLeftImage,
    __out ID2D1Bitmap1** argRightImage)
{
    HRESULT hResult = NOERROR;

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    if (nullptr != this->mWindowHandle && true == this->mSlicing)
    {
        SLICER_CAPTURE_RESULT captureResult = {};

        // End the capturing
        ::SetLastError(ERROR_SUCCESS);
        if (TRUE == this->mSlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)))
        {
            // Make sure the capture succeeded
            if (SUCCEEDED(captureResult.CaptureResult))
            {
                // Check if the image was sliced in system memory (i.e. TS scenario)
                if (TRUE == captureResult.UpdateFromSysMem)
                {
                    // Make sure the memory mapped file handle is not null
                    if (nullptr != captureResult.hSectionBits)
                    {
                        if (nullptr != argLeftImage)
                        {
                            hResult = this->CopyFromSystemMemory(captureResult.hSectionBits, captureResult.cBits, argLeftImage);
                        }

                        if (nullptr != argRightImage)
                        {
                            *argRightImage = nullptr;
                        }

                        ::CloseHandle(captureResult.hSectionBits);
                    }
                    else
                    {
                        hResult = E_POINTER;
                    }
                }
                else
                {
                    ID2D1Bitmap1* leftImage = nullptr;
                    ID2D1Bitmap1* rightImage = nullptr;

                    if (nullptr != argLeftImage)
                    {
                        if (this->mSharedTexturesLeft.size() > 0)
                        {
                            #ifdef DBG
                            this->Save(L"DBG-LeftChannelSharedTexture.png", this->mSharedTexturesLeft.begin()->second.begin()->get());
                            #endif
                            hResult = this->CombineTextures(this->mSharedTexturesLeft, this->mGpuBitmapLeft.get(), &leftImage);
                        }
                    }

                    if (SUCCEEDED(hResult) && nullptr != argRightImage)
                    {
                        if (this->mSharedTexturesRight.size() > 0)
                        {
                            if (FAILED(hResult = this->CombineTextures(this->mSharedTexturesRight, this->mGpuBitmapRight.get(), &rightImage)))
                            {
                                if (nullptr != leftImage)
                                {
                                    leftImage->Release();
                                    leftImage = nullptr;
                                }
                            }
                        }
                    }

                    if (nullptr != argLeftImage)
                    {
                        *argLeftImage = leftImage;
                    }

                    if (nullptr != argRightImage)
                    {
                        *argRightImage = rightImage;
                    }

                    this->mGpuBitmapLeft = nullptr;
                    this->mGpuBitmapRight = nullptr;
                }

                // Set the number of frames captured
                if (SUCCEEDED(hResult) && nullptr != argFramesCaptured)
                {
                    *argFramesCaptured = captureResult.cFramesCaptured;
                }
            }
            else
            {
                hResult = captureResult.CaptureResult;
            }
        }
        else
        {
            SET_HRESULT_FROM_GLE();
        }

        // Always clean up the resources on EndCapture
        this->CleanupResources();

        mSlicing = false;
    }
    else
    {
        hResult = E_ABORT;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


HRESULT DwmSlicer::StartCapture(
    RECT argAreaToCapture,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    __out UINT* argNumberOfFramesThatCanBeCaptured)
{
    return this->InternalStartCapture(
        argAreaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip,
        argChannel,
        argNumberOfFramesThatCanBeCaptured,
        false);
}


//
// Private Methods
//

HRESULT DwmSlicer::AllocateResources(
    const AdapterToOutputsMap& argAdapterToOutputsMap,
    Channel argChannel,
    UINT argFrameWidth,
    UINT argFrameHeight,
    UINT argRequestedFramesPerRow,
    UINT argRequestedRows,
    __out UINT* argActualFramesPerRow,
    __out UINT* argActualRows,
    __out std::vector<SLICER_SHARED_TEXTURE>* argSharedTextures)
{
    HRESULT hResult = NOERROR;

    if (nullptr != argActualFramesPerRow &&
        nullptr != argActualRows &&
        nullptr != argSharedTextures)
    {
        *argActualFramesPerRow = 0;
        *argActualRows = 0;

        // Try to create the resources with as many rows
        // as we can until we get down to one row
        for (UINT row = argRequestedRows; row > 1; --row)
        {
            hResult = this->CreateTexturesForOutputs(
                argAdapterToOutputsMap,
                argChannel,
                argFrameWidth * argRequestedFramesPerRow,
                argFrameHeight * row,
                argSharedTextures);

            //??? TEST - Uncomment this to simulate the E_OUTOFMEMORY case
            //if (row > 1)
            //{
            //    this->CleanupResources();
            //    hResult = E_OUTOFMEMORY;
            //}

            if (SUCCEEDED(hResult))
            {
                *argActualFramesPerRow = argRequestedFramesPerRow;
                *argActualRows = row;
                return hResult;
            }
            else if (E_OUTOFMEMORY != hResult)
            {
                return hResult;
            }

            // If we failed because of E_OUTOFMEMORY,
            // then loop again with one less row
        }

        // Try to create the resources with one row and as many
        // frames as we can until we get down to zero frames
        for (UINT framesPerRow = argRequestedFramesPerRow; framesPerRow > 0; --framesPerRow)
        {
            hResult = this->CreateTexturesForOutputs(
                argAdapterToOutputsMap,
                argChannel,
                argFrameWidth * framesPerRow,
                argFrameHeight,
                argSharedTextures);

            //??? TEST - Uncomment this to simulate the E_OUTOFMEMORY case
            //if (framesPerRow > 1)
            //{
            //    this->CleanupResources();
            //    hResult = E_OUTOFMEMORY;
            //}

            if (SUCCEEDED(hResult))
            {
                *argActualFramesPerRow = framesPerRow;
                *argActualRows = 1;
                return hResult;
            }
            else if (E_OUTOFMEMORY != hResult)
            {
                return hResult;
            }

            // If we failed because of E_OUTOFMEMORY,
            // then loop again with one less frame
        }
    }
    else
    {
        hResult = E_INVALIDARG;
    }

    return hResult;
}


void DwmSlicer::CleanupResources(void)
{
    this->mGpuBitmapLeft = nullptr;
    this->mGpuBitmapRight = nullptr;

    this->mCpuBitmap = nullptr;

    this->mStagingTextures.clear();

    this->mSharedTexturesLeft.clear();
    this->mSharedTexturesRight.clear();
}


HRESULT DwmSlicer::CombineTextures(
    const __in DeviceToTexturesMap& argTextureMap,
    ID2D1Bitmap1* argGpuBitmap,
    __out ID2D1Bitmap1** argBitmapOut)
{
    HRESULT hResult = NOERROR;

    if (0 == argTextureMap.size() || nullptr == argGpuBitmap || nullptr == argBitmapOut)
    {
        return E_INVALIDARG;
    }

    // Get the size of the GPU bitmap
    auto bitmapSize =    argGpuBitmap->GetSize();
    auto textureWidth =  static_cast<UINT32>(bitmapSize.width);
    auto textureHeight = static_cast<UINT32>(bitmapSize.height);
    auto texturePitch =  textureWidth * 4;

    // Zero the CPU staging bitmap
    ::memset(this->mCpuBitmap.get(), 0, texturePitch * textureHeight);

    // Loop over each texture and combine them
    for (auto iter = argTextureMap.begin(); iter != argTextureMap.end(); ++iter)
    {
        std::shared_ptr<ID3D11Device> device = iter->first;
        //ID3D11Device* device = iter->first.get();
        std::vector<std::shared_ptr<ID3D11Texture2D>> textures = iter->second;

        // Get the device context
        CComPtr<ID3D11DeviceContext> immediateContext;
        device->GetImmediateContext(&immediateContext);

        // Get the staging texture for this device
        ID3D11Texture2D* stagingTexture = this->mStagingTextures[device].get();

        // Loop through each texture on the device
        for (UINT i = 0; i < textures.size(); ++i)
        {
            // Get the start of the system memory texture
            BYTE* current = this->mCpuBitmap.get();

            // Get the current texture
            ID3D11Texture2D* sourceTexture = textures[i].get();

            // Copy the current hardware texture to the staging texture
            immediateContext->CopyResource(stagingTexture, sourceTexture);

            // Map the staging texture into system memory
            D3D11_MAPPED_SUBRESOURCE map = {};
            if (SUCCEEDED(hResult = immediateContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &map)))
            {
                // Save the start of the mapped resource
                BYTE* subtexturePointer = static_cast<BYTE*>(map.pData);

                // Loop over each row
                for (UINT y = 0; y < textureHeight; ++y)
                {
                    // Save the start of this row in the mapped resource
                    BYTE* currentSubtextureRow = subtexturePointer;

                    // Loop through each texel in the row,
                    // converting BGRA to RGBA
                    for (UINT x = 0; x < textureWidth; ++x)
                    {
                        *(current++) += *(currentSubtextureRow+2);
                        *(current++) += *(currentSubtextureRow+1);
                        *(current++) += *(currentSubtextureRow+0);
                        *(current++) += *(currentSubtextureRow+3);
                        currentSubtextureRow += 4;
                    }

                    // Move to the next row in the mapped resource
                    subtexturePointer += map.RowPitch;
                }

                // Unmap the staging texture
                immediateContext->Unmap(stagingTexture, 0);
            }
        }
    }

    // Copy the CPU bitmap into the GPU bitmap
    if (SUCCEEDED(hResult))
    {
        if (SUCCEEDED(hResult = argGpuBitmap->CopyFromMemory(nullptr, this->mCpuBitmap.get(), texturePitch)))
        {
            ID2D1Bitmap1* bitmap = argGpuBitmap;
            bitmap->AddRef();
            *argBitmapOut = bitmap;
        }
    }

    return hResult;
}


HRESULT DwmSlicer::CopyFromSystemMemory(
    HANDLE argSystemMemoryHandle,
    ULONG argFileLength,
    __out ID2D1Bitmap1** argBitmapOut)
{
    HRESULT hResult = NOERROR;

    if (nullptr != argSystemMemoryHandle && nullptr != argBitmapOut)
    {
        // Map the file into memory
        ::SetLastError(ERROR_SUCCESS);
        void* memoryMappedFileStart = ::MapViewOfFile(argSystemMemoryHandle, FILE_MAP_READ, 0, 0, argFileLength);

        if (nullptr != memoryMappedFileStart)
        {
            // The start of the file contains adapter texture offsets
            MAGN_ADAPTERTEXTURE_OFFSETS* textureOffset = static_cast<MAGN_ADAPTERTEXTURE_OFFSETS*>(memoryMappedFileStart);

            auto textureCount =  textureOffset->cSharedTextures;
            auto textureWidth =  textureOffset->Width;
            auto textureHeight = textureOffset->Height;

            auto texturePitch =  textureWidth * 4;
            auto textureSize =   texturePitch * textureHeight;

            // Loop over each texture
            for (UINT i = 0; i < textureCount; ++i)
            {
                BYTE* memoryMappedTexture = this->mCpuBitmap.get();
                BYTE* sourceTexture = static_cast<BYTE*>(memoryMappedFileStart) + textureOffset->TexturesBitsOffset[i].Offset;

                // Convert BGRA to RGBA
                for (UINT p = 0; p < textureWidth * textureHeight; ++p)
                {
                    *(memoryMappedTexture++) += *(sourceTexture+2);
                    *(memoryMappedTexture++) += *(sourceTexture+1);
                    *(memoryMappedTexture++) += *(sourceTexture+0);
                    *(memoryMappedTexture++) += *(sourceTexture+3);
                    sourceTexture += 4;
                }
            }

            ::UnmapViewOfFile(memoryMappedFileStart);

            if (SUCCEEDED(hResult = this->mGpuBitmapLeft->CopyFromMemory(nullptr, this->mCpuBitmap.get(), texturePitch)))
            {
                ID2D1Bitmap1* bitmap = this->mGpuBitmapLeft.get();
                bitmap->AddRef();
                *argBitmapOut = bitmap;
                this->mGpuBitmapLeft = nullptr;
            }
        }
        else
        {
            SET_HRESULT_FROM_GLE();
        }
    }
    else
    {
        hResult = E_INVALIDARG;
    }

    return hResult;
}


HRESULT DwmSlicer::CreateTexturesForOutputs(
    const AdapterToOutputsMap& argAdapterToOutputsMap,
    Channel argChannel,
    UINT argTextureWidth,
    UINT argTextureHeight,
    __out std::vector<SLICER_SHARED_TEXTURE>* argSharedTextures)
{
    HRESULT hResult = NOERROR;

    if (nullptr == argSharedTextures)
    {
        return E_INVALIDARG;
    }

    #ifdef DBG
    std::wcout << L"CreateTexturesForOutputs:" << std::endl;
    std::wcout << L"\tLeft Channel:  " << IsChannelLeft(argChannel) << std::endl;
    std::wcout << L"\tRight Channel: " << IsChannelRight(argChannel) << std::endl;
    std::wcout << L"\tWidth:         " << argTextureWidth << std::endl;
    std::wcout << L"\tHeight:        " << argTextureHeight << std::endl;
    #endif

    std::vector<SLICER_SHARED_TEXTURE> sharedTextures;

    // Create a shared texture for each output
    for (auto iter = argAdapterToOutputsMap.begin(); iter != argAdapterToOutputsMap.end() && sharedTextures.size() <= MAG_MAX_ADAPTER; ++iter)
    {
        // Get the adapter LUID
        DXGI_ADAPTER_DESC adapterDesc = {};
        if (FAILED(hResult = iter->first->GetDesc(&adapterDesc)))
        {
            this->CleanupResources();
            return hResult;
        }

        if (this->mAdapterToDeviceMap.count(iter->first) == 0)
        {
            this->CleanupResources();
            return E_POINTER;
        }

        // Get the D3D11 device from the adapter
        auto device = this->mAdapterToDeviceMap[iter->first];

        //
        // Create a staging texture for each device, so that all of the hardware
        // textures can be copied to a CPU texture, before being combined on a
        // CPU bitmap, and then finally copied to a hardware bitmap.
        //
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = 0;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.Height = argTextureHeight;
        textureDesc.MipLevels = 1;
        textureDesc.MiscFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.Width = argTextureWidth;

        ID3D11Texture2D* stagingTexture = nullptr;
        if (FAILED(hResult = device->CreateTexture2D(&textureDesc, nullptr, &stagingTexture)))
        {
            this->CleanupResources();
            return hResult;
        }

        this->mStagingTextures[device] = std::shared_ptr<ID3D11Texture2D>(stagingTexture, release_iunknown_deleter);

        // Set the texture description for the hardware textures
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // Shared surfaces must have these bind flags
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;

        // Loop over the outputs
        for (UINT i = 0; i < iter->second.size() && sharedTextures.size() <= MAG_MAX_ADAPTER; ++i)
        {
            // Get the output HMONITOR
            DXGI_OUTPUT_DESC outputDesc;
            if (FAILED(hResult = iter->second[i]->GetDesc(&outputDesc)))
            {
                this->CleanupResources();
                return hResult;
            }

            // Create a shared texture for the left channel
            if (IsChannelLeft(argChannel))
            {
                ID3D11Texture2D* texture = nullptr;
                if (FAILED(hResult = device->CreateTexture2D(&textureDesc, nullptr, &texture)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                std::shared_ptr<ID3D11Texture2D> textureLeft(texture, release_iunknown_deleter);

                // Get the shared handle
                HANDLE sharedHandle = 0;
                CComPtr<IDXGIResource> dxgiResource = nullptr;

                if (FAILED(hResult = texture->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                if (FAILED(hResult = dxgiResource->GetSharedHandle(&sharedHandle)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                #ifdef DBG
                std::wcout << L"Left shared texture " << i << std::endl;
                std::wcout << L"\tShared Handle:  " << sharedHandle << std::endl;
                std::wcout << L"\tMonitor Handle: " << outputDesc.Monitor << std::endl;
                std::wcout << L"\tAdapter:        " << adapterDesc.Description << std::endl;
                std::wcout << L"\tAdapter LUID:   " << adapterDesc.AdapterLuid.HighPart << L" " << adapterDesc.AdapterLuid.LowPart << std::endl << std::endl;
                #endif

                // Fill out the shared texture for this output
                SLICER_SHARED_TEXTURE sharedTexture = {};
                sharedTexture.AdapterLuid = adapterDesc.AdapterLuid;
                sharedTexture.hmonAssoc = outputDesc.Monitor;
                sharedTexture.hDxSharedTexture = sharedHandle;
                sharedTexture.Flags = 0;

                sharedTextures.push_back(sharedTexture);
                this->mSharedTexturesLeft[device].push_back(textureLeft);
            }

            // Create a shared texture for the right channel
            if (IsChannelRight(argChannel) && sharedTextures.size() <= MAG_MAX_ADAPTER)
            {
                ID3D11Texture2D* texture = nullptr;
                if (FAILED(hResult = device->CreateTexture2D(&textureDesc, nullptr, &texture)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                std::shared_ptr<ID3D11Texture2D> textureRight(texture, release_iunknown_deleter);

                // Get the shared handle
                HANDLE sharedHandle = 0;
                CComPtr<IDXGIResource> dxgiResource = nullptr;

                if (FAILED(hResult = texture->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                if (FAILED(hResult = dxgiResource->GetSharedHandle(&sharedHandle)))
                {
                    this->CleanupResources();
                    return hResult;
                }

                #ifdef DBG
                std::wcout << L"Right shared texture " << i << std::endl;
                std::wcout << L"\tShared Handle:  " << sharedHandle << std::endl;
                std::wcout << L"\tMonitor Handle: " << outputDesc.Monitor << std::endl;
                std::wcout << L"\tAdapter:        " << adapterDesc.Description << std::endl;
                std::wcout << L"\tAdapter LUID:   " << adapterDesc.AdapterLuid.HighPart << L" " << adapterDesc.AdapterLuid.LowPart << std::endl << std::endl;
                #endif

                // Fill out the shared texture for this output
                SLICER_SHARED_TEXTURE sharedTexture = {};
                sharedTexture.AdapterLuid = adapterDesc.AdapterLuid;
                sharedTexture.hmonAssoc = outputDesc.Monitor;
                sharedTexture.hDxSharedTexture = sharedHandle;
                sharedTexture.Flags = SLICER_TEXTURE_FLAGS_RIGHT_CHANNEL;

                sharedTextures.push_back(sharedTexture);
                this->mSharedTexturesRight[device].push_back(textureRight);
            }
        }
    }

    // Create a D2D bitmap for each channel to return on EndCapture
    D2D1_BITMAP_PROPERTIES1 desc;
    desc.colorContext = nullptr;
    desc.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    desc.dpiX = 96;
    desc.dpiY = 96;
    desc.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;

    if (IsChannelLeft(argChannel))
    {
        ID2D1Bitmap1* gpuBitmapLeft = nullptr;

        if (FAILED(hResult = this->mD2DDeviceContext->CreateBitmap(
            D2D1::SizeU(argTextureWidth, argTextureHeight),
            nullptr,
            0,
            &desc,
            &gpuBitmapLeft)))
        {
            this->CleanupResources();
            return hResult;
        }

        this->mGpuBitmapLeft = std::shared_ptr<ID2D1Bitmap1>(gpuBitmapLeft, release_iunknown_deleter);
    }

    if (IsChannelRight(argChannel))
    {
        ID2D1Bitmap1* gpuBitmapRight = nullptr;

        if (FAILED(hResult = this->mD2DDeviceContext->CreateBitmap(
            D2D1::SizeU(argTextureWidth, argTextureHeight),
            nullptr,
            0,
            &desc,
            &gpuBitmapRight)))
        {
            this->CleanupResources();
            return hResult;
        }

        this->mGpuBitmapRight = std::shared_ptr<ID2D1Bitmap1>(gpuBitmapRight, release_iunknown_deleter);
    }

    // Create a bitmap to stage the captured textures
    this->mCpuBitmap = std::unique_ptr<BYTE[]>(new (std::nothrow) BYTE[argTextureWidth * argTextureHeight * 4]);

    if (nullptr == this->mCpuBitmap)
    {
        this->CleanupResources();
        return E_OUTOFMEMORY;
    }

    // Everything succeeded
    *argSharedTextures = sharedTextures;

    return hResult;
}


HRESULT DwmSlicer::GetLowestFeatureLevelFromAdapters(
    const AdapterToOutputsMap& argAdapterToOutputsMap,
    __out D3D_FEATURE_LEVEL* argLowestFeatureLevel)
{
    if (nullptr != argLowestFeatureLevel)
    {
        auto lowestFeatureLevel = D3D_FEATURE_LEVEL_11_1;

        // Find the lowest feature level of the adapters returned
        for (auto iter = argAdapterToOutputsMap.begin(); iter != argAdapterToOutputsMap.end(); ++iter)
        {
            // Find the DirectX device associated with this adapter
            if (this->mAdapterToDeviceMap.count(iter->first) > 0)
            {
                auto featureLevel = this->mAdapterToDeviceMap[iter->first]->GetFeatureLevel();

                if (featureLevel < lowestFeatureLevel)
                {
                    lowestFeatureLevel = featureLevel;
                }
            }
            else
            {
                return E_POINTER;
            }
        }

        *argLowestFeatureLevel = lowestFeatureLevel;
    }
    else
    {
        return E_INVALIDARG;
    }

    return NOERROR;
}


UINT DwmSlicer::GetMaxTextureSizeFromFeatureLevel(D3D_FEATURE_LEVEL argFeatureLevel)
{
    UINT maxTextureSize = 0;

    // Maximum texture dimension acording to MSDN
    // Direct3D 11 on Downlevel Hardware - Introduction
    // http://msdn.microsoft.com/en-us/library/windows/desktop/ff476876(v=vs.85).aspx
    switch (argFeatureLevel)
    {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
            // Since the DWM uses D3D 10.1, we cannot create a hardware texture
            // bigger than the maximum texture size of D3D 10.1.
            //maxTextureSize = 16384;
            //break;

        case D3D_FEATURE_LEVEL_10_1:
        case D3D_FEATURE_LEVEL_10_0:
            maxTextureSize = 8192;
            break;

        case D3D_FEATURE_LEVEL_9_3:
            maxTextureSize = 4096;
            break;

        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
            maxTextureSize = 2048;
            break;
    }

    return maxTextureSize;
}


HRESULT DwmSlicer::GetOutputsFromScreenRect(
    const RECT& argScreenRect,
    __out AdapterToOutputsMap* argAdapterToOutputsMap)
{
    if (nullptr != argAdapterToOutputsMap)
    {
        AdapterToOutputsMap adapterToOutputsMap;

        // Loop over each adapter
        for (auto iter = this->mAdapterToDeviceMap.begin(); iter != this->mAdapterToDeviceMap.end(); ++iter)
        {
            IDXGIOutput* output = nullptr;

            // Loop over each output connected to the adapter
            for (UINT outputIndex = 0; DXGI_ERROR_NOT_FOUND != iter->first->EnumOutputs(outputIndex, &output); ++outputIndex)
            {
                HRESULT hResult = NOERROR;
                DXGI_OUTPUT_DESC desc = {};

                if (SUCCEEDED(hResult = output->GetDesc(&desc)))
                {
                    // If the specified screen rect intersects with the desktop
                    // coordinates of the monitor, then save the output
                    RECT temp = {};
                    if (FALSE != ::IntersectRect(&temp, &argScreenRect, &desc.DesktopCoordinates))
                    {
                        adapterToOutputsMap[iter->first].push_back(std::shared_ptr<IDXGIOutput>(output, release_iunknown_deleter));
                    }
                }
                else
                {
                    output->Release();
                    return hResult;
                }
            }
        }

        *argAdapterToOutputsMap = adapterToOutputsMap;
    }
    else
    {
        return E_INVALIDARG;
    }

    return NOERROR;
}


HRESULT DwmSlicer::InternalStartCapture(
    RECT argAreaToCapture,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    __out UINT* argNumberOfFramesThatCanBeCaptured,
    bool argTest)
{
    HRESULT hResult = NOERROR;

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    // Make sure the capture rect is positive
    if (!IsChannelValid(argChannel) || argAreaToCapture.left >= argAreaToCapture.right || argAreaToCapture.top >= argAreaToCapture.bottom)
    {
        hResult = E_INVALIDARG;
    }
    else if (nullptr != this->mWindowHandle && false == this->mSlicing)
    {
        AdapterToOutputsMap outputs;

        // Get the mapping of the monitors to the capture area
        if (SUCCEEDED(hResult = this->GetOutputsFromScreenRect(argAreaToCapture, &outputs)))
        {
            if (outputs.size() > 0)
            {
                D3D_FEATURE_LEVEL lowestFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);

                // Get the lowest feature level from the returned adapters
                if (SUCCEEDED(hResult = this->GetLowestFeatureLevelFromAdapters(outputs, &lowestFeatureLevel)))
                {
                    // Get the max texture size
                    UINT maxTextureSize = this->GetMaxTextureSizeFromFeatureLevel(lowestFeatureLevel);

                    if (maxTextureSize > 0)
                    {
                        // We have the maximum texture size we can create on the
                        // adapters in the output list
                        // Now we must calculate the size of the texture we need to
                        // create to accomodate the requestedrequested capture

                        // Calculate the frame width and height
                        UINT frameWidth = argAreaToCapture.right - argAreaToCapture.left;
                        UINT frameHeight = argAreaToCapture.bottom - argAreaToCapture.top;

                        // Make sure the frame width and height is smaller than the max texture size
                        if (frameWidth > maxTextureSize || frameHeight > maxTextureSize)
                        {
                            hResult = E_INVALIDARG;
                        }
                        else
                        {
                            // Calculate the maximum number of frames per row and
                            // rows that can fit in the maximum texture size
                            UINT maximumFramesPerRow = maxTextureSize / frameWidth;
                            UINT maximumRows =         maxTextureSize / frameHeight;
                            UINT framesPerRow =        0;
                            UINT rows =                0;

                            // If the maximum number of frames requested is greater
                            // than or equal to maximumFramesPerRow, then there is
                            // one full row or multiple rows
                            if (argMaximumNumberOfFramesToCapture >= maximumFramesPerRow)
                            {
                                // Calculate the number of frames per row and rows
                                // required to accommodate the requested capture
                                framesPerRow = maximumFramesPerRow;
                                rows = (argMaximumNumberOfFramesToCapture % maximumFramesPerRow) ?
                                       (argMaximumNumberOfFramesToCapture / maximumFramesPerRow) + 1 :
                                       (argMaximumNumberOfFramesToCapture / maximumFramesPerRow);

                                // Make sure the rows does not exceed the maximumRows
                                if (rows > maximumRows)
                                {
                                    rows = maximumRows;
                                }
                            }
                            else
                            {
                                framesPerRow = argMaximumNumberOfFramesToCapture;
                                rows = 1;
                            }

                            // Now that we have the framesPerRow and the rows
                            // needed, create all of the resources required for
                            // capturing
                            UINT actualFramesPerRow = 0;
                            UINT actualRows = 0;
                            std::vector<SLICER_SHARED_TEXTURE> sharedTexturesList;

                            if (SUCCEEDED(hResult = this->AllocateResources(
                                outputs,
                                argChannel,
                                frameWidth,
                                frameHeight,
                                framesPerRow,
                                rows,
                                &actualFramesPerRow,
                                &actualRows,
                                &sharedTexturesList)))
                            {
                                // We have successfully allocated all of the
                                // resources we need to begin capturing

                                #ifdef DBG
                                std::wcout << L"AllocateResources:" << std::endl;
                                std::wcout << L"\tActual Frames Per Row: " << std::dec << actualFramesPerRow << std::endl;
                                std::wcout << L"\tActual Rows:           " << std::dec << actualRows << std::endl;
                                #endif

                                auto maximumNumberOfFrames = actualFramesPerRow * actualRows;
                                auto textureWidth =          frameWidth * actualFramesPerRow;
                                auto textureHeight =         frameHeight * actualRows;

                                if (argMaximumNumberOfFramesToCapture > maximumNumberOfFrames)
                                {
                                    argMaximumNumberOfFramesToCapture = maximumNumberOfFrames;
                                }

                                if (true == argTest)
                                {
                                    // Just return the number of frames that can be captured
                                    *argNumberOfFramesThatCanBeCaptured = argMaximumNumberOfFramesToCapture;

                                    // Clean up the resource we just allocated
                                    this->CleanupResources();
                                }
                                else
                                {
                                    // Allocate memory for the shared textures
                                    auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[sharedTexturesList.size()]);
                                    SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

                                    // Check if malloc has failed
                                    if (nullptr != sharedTextures)
                                    {
                                        // Fill out the texture information
                                        sharedTextures->Width = textureWidth;
                                        sharedTextures->Height = textureHeight;
                                        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
                                        sharedTextures->cSharedTextures = sharedTexturesList.size();

                                        for (UINT i = 0; i < sharedTexturesList.size(); ++i)
                                        {
                                            sharedTextures->Textures[i] = sharedTexturesList[i];
                                        }

                                        // Set the destination shared textures
                                        ::SetLastError(ERROR_SUCCESS);

                                        BOOL retval = this->mSlicerControl(
                                            this->mWindowHandle,
                                            SlicerContextSetDestinationTextures,
                                            sharedTextures,
                                            sharedTextureSize);

                                        ::free(sharedTextures);

                                        if (TRUE == retval)
                                        {
                                            // Set the capture parameters
                                            SLICER_CAPTURE_PARAM captureParams;
                                            captureParams.cFramesToSkip = argFramesToSkip;
                                            captureParams.cMaxFrames = argMaximumNumberOfFramesToCapture;
                                            captureParams.rcCapture.bottom = argAreaToCapture.bottom;
                                            captureParams.rcCapture.left = argAreaToCapture.left;
                                            captureParams.rcCapture.right = argAreaToCapture.right;
                                            captureParams.rcCapture.top = argAreaToCapture.top;

                                            // Start capturing
                                            ::SetLastError(ERROR_SUCCESS);

                                            retval = this->mSlicerControl(
                                                this->mWindowHandle,
                                                SlicerContextBeginCapture,
                                                &captureParams,
                                                sizeof(captureParams));

                                            if (TRUE == retval)
                                            {
                                                mSlicing = true;

                                                if (nullptr != argNumberOfFramesThatCanBeCaptured)
                                                {
                                                    *argNumberOfFramesThatCanBeCaptured = argMaximumNumberOfFramesToCapture;
                                                }
                                            }
                                            else
                                            {
                                                this->CleanupResources();
                                                SET_HRESULT_FROM_GLE();
                                            }
                                        }
                                        else
                                        {
                                            this->CleanupResources();
                                            SET_HRESULT_FROM_GLE();
                                        }
                                    }
                                    else
                                    {
                                        this->CleanupResources();
                                        hResult = E_OUTOFMEMORY;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        hResult = E_FAIL;
                    }
                }
            }
            else
            {
                hResult = E_INVALIDARG;
            }
        }
    }
    else
    {
        hResult = E_ABORT;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


#ifdef DBG
void DwmSlicer::Save(const std::wstring& argFileName, ID3D11Texture2D* argTexture)
{
    HRESULT hResult;

    // Determine if we need a staging texture
    D3D11_TEXTURE2D_DESC textureDesc = {};
    argTexture->GetDesc(&textureDesc);

    CComPtr<ID3D11Device> device;
    argTexture->GetDevice(&device);

    CComPtr<ID3D11DeviceContext> immediateContext;
    device->GetImmediateContext(&immediateContext);

    CComPtr<ID3D11Texture2D> stagingTexture;

    if (D3D11_CPU_ACCESS_READ == (D3D11_CPU_ACCESS_READ & textureDesc.CPUAccessFlags))
    {
        stagingTexture = argTexture;
    }
    else
    {
        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = 0;
        textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        textureDesc.Format;
        textureDesc.Height;
        textureDesc.MipLevels = 1;
        textureDesc.MiscFlags = 0;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_STAGING;
        textureDesc.Width;

        if (FAILED(hResult = device->CreateTexture2D(&textureDesc, NULL, &stagingTexture)))
        {
            std::wcout << L"ID3D11Device::CreateTexture2D" << std::endl;
            return;
        }

        immediateContext->CopyResource(stagingTexture, argTexture);
    }

    // Map the texture into system memory
    D3D11_MAPPED_SUBRESOURCE map;
    if (FAILED(hResult = immediateContext->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &map)))
    {
        std::wcout << L"ID3D11DeviceContext::Map" << std::endl;
        return;
    }

    BYTE* data = (BYTE*)map.pData;
    BYTE* dataEnd = data+map.DepthPitch;

    // Create a WICImagingFactory
    CComPtr<IWICImagingFactory> factory;
    if (FAILED(hResult = factory.CoCreateInstance(CLSID_WICImagingFactory)))
    {
        std::wcout << L"CoCreateInstance(CLSID_WICImagingFactory)" << std::endl;
        return;
    }

    // Create and initialize a WIC Stream
    CComPtr<IWICStream> targetStream;
    if (FAILED(hResult = factory->CreateStream(&targetStream)))
    {
       std::wcout << "IWICImagingFactory::CreateStream" << std::endl;
       return;
    }

    if (FAILED(hResult = targetStream->InitializeFromFilename(argFileName.c_str(), GENERIC_WRITE)))
    {
        std::wcout << L"IWICStream::InitializeFromFilename" << std::endl;
    }

    // Create and initialize a PNG Encoder
    CComPtr<IWICBitmapEncoder> encoder;
    if (FAILED(hResult = encoder.CoCreateInstance(CLSID_WICPngEncoder)))
    {
        std::wcout << L"CoCreateInstance(CLSID_WICPngEncoder)" << std::endl;
    }

    if (FAILED(hResult = encoder->Initialize(targetStream, WICBitmapEncoderNoCache)))
    {
        std::wcout << L"IWICBitmapEncoder::Initialize" << std::endl;
    }

    // Create a new frame from the encoder
    CComPtr<IWICBitmapFrameEncode> targetFrame;
    if (FAILED(hResult = encoder->CreateNewFrame(&targetFrame, 0))) // No properties
    {
        std::wcout << L"IWICBitmapEncoder::CreateNewFrame" << std::endl;
    }

    if (FAILED(hResult = targetFrame->Initialize(0))) // No properties
    {
        std::wcout << L"IWICBitmapFrameEncode::Initialize" << std::endl;
    }

    if (FAILED(hResult = targetFrame->SetSize(textureDesc.Width, textureDesc.Height)))
    {
        std::wcout << L"IWICBitmapFrameEncode::SetSize" << std::endl;
    }

    // Determine the format and texel size
    CComPtr<IWICBitmapSource> source;
    switch (textureDesc.Format)
    {
        // 32 Bits Per Pixel Gray -> 8 Bits Per Pixel BGRA
        case DXGI_FORMAT_R32_FLOAT:
        {
            // Create a WIC Bitmap from the texture
            CComPtr<IWICBitmap> bitmap;
            if (FAILED(hResult = factory->CreateBitmapFromMemory(textureDesc.Width, textureDesc.Height, GUID_WICPixelFormat32bppGrayFloat, map.RowPitch, map.DepthPitch, data, &bitmap)))
            {
                std::wcout << L"IWICImagingFactory::CreateBitmapFromMemory(GUID_WICPixelFormat32bppGrayFloat)" << std::endl;
            }

            // Create and initialize a WIC Converter
            CComPtr<IWICFormatConverter> converter;
            if (FAILED(hResult = factory->CreateFormatConverter(&converter)))
            {
                std::wcout << L"IWICImagingFactory::CreateFormatConverter" << std::endl;
            }

            if (FAILED(hResult = converter->Initialize(bitmap, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
            {
                std::wcout << L"IWICFormatConverter::Initialize" << std::endl;
            }

            // Check if we can convert
            BOOL canConvert = FALSE;
            if (FAILED(hResult = converter->CanConvert(GUID_WICPixelFormat32bppGrayFloat, GUID_WICPixelFormat32bppBGRA, &canConvert)))
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat32bppGrayFloat to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            // Throw if we cannot convert
            if (!canConvert)
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat32bppGrayFloat to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            source = converter;

            break;
        }

        // 32 Bits Per Pixel RGBA -> 8 Bits Per Pixel BGRA
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
        {
            // Create a WIC Bitmap from the texture
            CComPtr<IWICBitmap> bitmap;
            if (FAILED(hResult = factory->CreateBitmapFromMemory(textureDesc.Width, textureDesc.Height, GUID_WICPixelFormat128bppRGBAFixedPoint, map.RowPitch, map.DepthPitch, data, &bitmap)))
            {
                std::wcout << L"IWICImagingFactory::CreateBitmapFromMemory(GUID_WICPixelFormat128bppRGBAFixedPoint)" << std::endl;
            }

            // Create and initialize a WIC Converter
            CComPtr<IWICFormatConverter> converter;
            if (FAILED(hResult = factory->CreateFormatConverter(&converter)))
            {
                std::wcout << L"IWICImagingFactory::CreateFormatConverter" << std::endl;
            }

            if (FAILED(hResult = converter->Initialize(bitmap, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
            {
                std::wcout << L"IWICFormatConverter::Initialize" << std::endl;
            }

            // Check if we can convert
            BOOL canConvert = FALSE;
            if (FAILED(hResult = converter->CanConvert(GUID_WICPixelFormat128bppRGBAFixedPoint, GUID_WICPixelFormat32bppBGRA, &canConvert)))
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat128bppRGBAFixedPoint to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            // Throw if we cannot convert
            if (!canConvert)
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat128bppRGBAFixedPoint to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            source = converter;

            break;
        }

        // 8 Bits Per Pixel RGBA -> 8 Bits Per Pixel BGRA
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        {
            // Create a WIC Bitmap from the texture
            CComPtr<IWICBitmap> bitmap;
            if (FAILED(hResult = factory->CreateBitmapFromMemory(textureDesc.Width, textureDesc.Height, GUID_WICPixelFormat32bppRGBA, map.RowPitch, map.DepthPitch, data, &bitmap)))
            {
                std::wcout << L"IWICImagingFactory::CreateBitmapFromMemory(GUID_WICPixelFormat32bppRGBA)" << std::endl;
            }

            // Create and initialize a WIC Converter
            CComPtr<IWICFormatConverter> converter;
            if (FAILED(hResult = factory->CreateFormatConverter(&converter)))
            {
                std::wcout << L"IWICImagingFactory::CreateFormatConverter" << std::endl;
            }

            if (FAILED(hResult = converter->Initialize(bitmap, GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom)))
            {
                std::wcout << L"IWICFormatConverter::Initialize" << std::endl;
            }

            // Check if we can convert
            BOOL canConvert = FALSE;
            if (FAILED(converter->CanConvert(GUID_WICPixelFormat32bppRGBA, GUID_WICPixelFormat32bppBGRA, &canConvert)))
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat32bppRGBA to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            // Throw if we cannot convert
            if (!canConvert)
            {
                std::wcout << L"Cannot convert from GUID_WICPixelFormat32bppRGBA to GUID_WICPixelFormat32bppBGRA" << std::endl;
            }

            source = converter;

            break;
        }

        // 8 Bits Per Pixel BGRA -> No conversion needed
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
        {
            // Create a WIC Bitmap from the texture
            CComPtr<IWICBitmap> bitmap;
            if (FAILED(hResult = factory->CreateBitmapFromMemory(textureDesc.Width, textureDesc.Height, GUID_WICPixelFormat32bppBGRA, map.RowPitch, map.DepthPitch, data, &bitmap)))
            {
                std::wcout << L"IWICImagingFactory::CreateBitmapFromMemory(GUID_WICPixelFormat32bppBGRA)" << std::endl;
            }

            source = bitmap;

            break;
        }
    }

    // Save and commit the frame and the stream
    if (FAILED(hResult = targetFrame->WriteSource(source, NULL)))
    {
        std::wcout << L"IWICBitmapFrameEncode::WriteSource" << std::endl;
    }

    if (FAILED(hResult = targetFrame->Commit()))
    {
        std::wcout << L"IWICBitmapFrameEncode::Commit" << std::endl;
    }

    if (FAILED(hResult = encoder->Commit()))
    {
        std::wcout << L"IWICBitmapEncoder::Commit" << std::endl;
    }
}
#endif
