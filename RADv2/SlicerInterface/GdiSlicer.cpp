//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "GdiSlicer.h"

#include <new>

using namespace RADv2;

#define SET_HRESULT_FROM_GLE() do { DWORD e = ::GetLastError(); if (ERROR_SUCCESS == e) { hResult = E_UNEXPECTED; } else { hResult = HRESULT_FROM_WIN32(e); } } while (0, 0)

GdiSlicer::GdiSlicer(void)
    : mRefCount(0),
      mD2DDevice(nullptr),
      mD2DDeviceContext(nullptr),
      mD2DFeatureLevel(D3D_FEATURE_LEVEL_9_1),
      mSlicing(false),
      mStopSlicingEvent(nullptr),
      mThreadHandle(nullptr),
      mThreadId(0)
{
    // Create a critical section
    ::InitializeSRWLock(&this->mFunctionSyncLock);
}


HRESULT GdiSlicer::Initialize(
    std::shared_ptr<ID2D1Device> argD2DDevice,
    D3D_FEATURE_LEVEL argD2DFeatureLevel)
{
    HRESULT hResult = NOERROR;

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    if (nullptr != argD2DDevice)
    {
        this->mStopSlicingEvent = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);

        if (nullptr != this->mStopSlicingEvent)
        {
            ID2D1DeviceContext* d2d1DeviceContext = nullptr;
            hResult = argD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2d1DeviceContext);

            if (SUCCEEDED(hResult))
            {
                // Save the D2D device
                this->mD2DDeviceContext = std::shared_ptr<ID2D1DeviceContext>(d2d1DeviceContext, release_iunknown_deleter);
                this->mD2DDevice = argD2DDevice;
                this->mD2DFeatureLevel = argD2DFeatureLevel;
                this->mFramesToSkip = 0;
            }
            else
            {
                ::CloseHandle(this->mStopSlicingEvent);
                this->mStopSlicingEvent = nullptr;
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

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


GdiSlicer::~GdiSlicer(void)
{
    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    if (nullptr != this->mThreadHandle)
    {
        ::TerminateThread(this->mThreadHandle, 0);
        ::CloseHandle(this->mThreadHandle);
        this->mThreadHandle = nullptr;
    }

    // Close the event handle
    if (nullptr != this->mStopSlicingEvent)
    {
        ::CloseHandle(this->mStopSlicingEvent);
        this->mStopSlicingEvent = nullptr;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE GdiSlicer::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);
    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE GdiSlicer::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE GdiSlicer::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    // Always set out parameter to nullptr, validating it first.
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
    else if (riid == __uuidof(IGdiSlicer))
    {
        *ppvObj = static_cast<IGdiSlicer*>(this);
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
// Public Methods
//

bool GdiSlicer::IsSlicing(void)
{
    return this->mSlicing;
}


HRESULT GdiSlicer::EndCapture(
    __out UINT* argFramesCaptured,
    __out ID2D1Bitmap1** argLeftImage,
    __out ID2D1Bitmap1** argRightImage)
{
    HRESULT hResult = NOERROR;

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    if (true == this->mSlicing)
    {
        // Make sure a thread was created (we are or were slicing)
        if (nullptr != this->mThreadHandle)
        {
            // Signal the end event
            if (TRUE == ::SetEvent(this->mStopSlicingEvent))
            {
                // Wait for the thread to terminate
                switch (::WaitForSingleObject(this->mThreadHandle, 5000))
                {
                    case WAIT_OBJECT_0:
                    {
                        // The thread has terminated on its own
                        break;
                    }

                    case WAIT_TIMEOUT:
                    case WAIT_FAILED:
                    default:
                    {
                        // If we timed out or failed, explicitly terminate the thread
                        ::TerminateThread(this->mThreadHandle, 0);
                        break;
                    }
                }

                // Close the thread handle
                if (TRUE == ::CloseHandle(this->mThreadHandle))
                {
                    if (nullptr != argLeftImage)
                    {
                        if (nullptr != this->mGpuBitmapLeft)
                        {
                            hResult = this->mGpuBitmapLeft->CopyFromMemory(
                                nullptr,
                                this->mSystemMemoryTexture.get(),
                                this->mTextureSize.cx * 4);

                            if (SUCCEEDED(hResult))
                            {
                                ID2D1Bitmap1* bitmap = this->mGpuBitmapLeft.get();
                                bitmap->AddRef();
                                *argLeftImage = bitmap;
                                this->mGpuBitmapLeft = nullptr;
                            }
                            else
                            {
                                *argLeftImage = nullptr;
                            }
                        }
                        else
                        {
                            *argLeftImage = nullptr;
                        }
                    }

                    if (SUCCEEDED(hResult) && nullptr != argRightImage)
                    {
                        if (nullptr != this->mGpuBitmapRight)
                        {
                            hResult = this->mGpuBitmapRight->CopyFromMemory(
                                nullptr,
                                this->mSystemMemoryTexture.get(),
                                this->mTextureSize.cx * 4);

                            if (SUCCEEDED(hResult))
                            {
                                ID2D1Bitmap1* bitmap = this->mGpuBitmapRight.get();
                                bitmap->AddRef();
                                *argRightImage = bitmap;
                                this->mGpuBitmapRight = nullptr;
                            }
                            else
                            {
                                *argRightImage = nullptr;
                            }
                        }
                        else
                        {
                            *argRightImage = nullptr;
                        }
                    }

                    if (SUCCEEDED(hResult) && nullptr != argFramesCaptured)
                    {
                        *argFramesCaptured = this->mFramesCaptured;
                    }
                }
                else
                {
                    SET_HRESULT_FROM_GLE();
                }
            }
            else
            {
                SET_HRESULT_FROM_GLE();
                ::CloseHandle(this->mThreadHandle);
            }

            this->mThreadHandle = nullptr;
            this->mThreadId = 0;
        }
        else
        {
            hResult = E_ABORT;
        }

        this->CleanupResources();
        this->mSlicing = false;
    }
    else
    {
        hResult = E_ABORT;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


HRESULT GdiSlicer::StartCapture(
    RECT argAreaToCapture,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    __out UINT* argNumberOfFramesThatCanBeCaptured)
{
    HRESULT hResult = NOERROR;

    ::AcquireSRWLockExclusive(&this->mFunctionSyncLock);

    if (!IsChannelValid(argChannel) ||
        argAreaToCapture.left >= argAreaToCapture.right ||
        argAreaToCapture.top >= argAreaToCapture.bottom)
    {
        hResult = E_INVALIDARG;
    }
    else if (false == this->mSlicing)
    {
        // Check if a slicing thread already exists
        if (nullptr == this->mThreadHandle)
        {
            // Reset the end event to nonsignaled
            if (TRUE == ::ResetEvent(this->mStopSlicingEvent))
            {
                // Max Texture Dimension acording to MSDN
                UINT maxTextureSize = 0;
                switch (mD2DFeatureLevel)
                {
                    // Since the DWM uses D3D 10.1, we cannot create a hardware texture
                    // bigger than the maximum texture size of D3D 10.1.
                    case D3D_FEATURE_LEVEL_11_1:
                    case D3D_FEATURE_LEVEL_11_0:
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

                UINT frameWidth = argAreaToCapture.right - argAreaToCapture.left;
                UINT frameHeight = argAreaToCapture.bottom - argAreaToCapture.top;

                // Make sure the frame is smaller than the texture
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

                    UINT actualFramesPerRow = 0;
                    UINT actualRows = 0;

                    if (SUCCEEDED(hResult = this->AllocateResources(
                        argChannel,
                        frameWidth,
                        frameHeight,
                        framesPerRow,
                        rows,
                        &actualFramesPerRow,
                        &actualRows)))
                    {
                        auto maximumNumberOfFrames = actualFramesPerRow * actualRows;
                        auto textureWidth =          frameWidth * actualFramesPerRow;
                        auto textureHeight =         frameHeight * actualRows;

                        if (argMaximumNumberOfFramesToCapture > maximumNumberOfFrames)
                        {
                            argMaximumNumberOfFramesToCapture = maximumNumberOfFrames;
                        }

                        // Initialize all of the information we need to capture the screen
                        {
                            this->mAreaToCapture.bottom = argAreaToCapture.bottom;
                            this->mAreaToCapture.left = argAreaToCapture.left;
                            this->mAreaToCapture.right = argAreaToCapture.right;
                            this->mAreaToCapture.top = argAreaToCapture.top;

                            this->mFramesCaptured = 0;
                            this->mFramesToSkip = argFramesToSkip + 1;

                            this->mTextureSize.cx = textureWidth;
                            this->mTextureSize.cy = textureHeight;

                            this->mMaximumNumberOfFramesToCapture = argMaximumNumberOfFramesToCapture;
                        }

                        // Create a new thread
                        this->mThreadHandle = ::CreateThread(
                            nullptr,
                            0,
                            GdiSlicer::ExecuteSlicing,
                            this,
                            0,
                            &this->mThreadId);

                        if (nullptr != mThreadHandle)
                        {
                            this->mSlicing = true;

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
                }
            }
            else
            {
                SET_HRESULT_FROM_GLE();
            }
        }
        else
        {
            hResult = E_ABORT;
        }
    }
    else
    {
        hResult = E_ABORT;
    }

    ::ReleaseSRWLockExclusive(&this->mFunctionSyncLock);

    return hResult;
}


HRESULT GdiSlicer::AllocateResources(
    Channel argChannel,
    UINT argFrameWidth,
    UINT argFrameHeight,
    UINT argRequestedFramesPerRow,
    UINT argRequestedRows,
    __out UINT* argActualFramesPerRow,
    __out UINT* argActualRows)
{
    HRESULT hResult = NOERROR;

    if (nullptr != argActualFramesPerRow && nullptr != argActualRows)
    {
        *argActualFramesPerRow = 0;
        *argActualRows = 0;

        // Try to create the resources with as many rows
        // as we can until we get down to one row
        for (UINT row = argRequestedRows; row > 1; --row)
        {
            hResult = this->CreateTexturesForOutputs(
                argChannel,
                argFrameWidth * argRequestedFramesPerRow,
                argFrameHeight * row);

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
                argChannel,
                argFrameWidth * framesPerRow,
                argFrameHeight);

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


void GdiSlicer::CleanupResources(void)
{
    this->mGpuBitmapLeft = nullptr;
    this->mGpuBitmapRight = nullptr;
    this->mSystemMemoryTexture = nullptr;
}


HRESULT GdiSlicer::CreateTexturesForOutputs(
    Channel argChannel,
    UINT argTextureWidth,
    UINT argTextureHeight)
{
    HRESULT hResult = NOERROR;

    D2D1_BITMAP_PROPERTIES1 bitmapDesc;
    bitmapDesc.colorContext = nullptr;
    bitmapDesc.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bitmapDesc.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bitmapDesc.dpiX = 96.0f;
    bitmapDesc.dpiY = 96.0f;
    bitmapDesc.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;

    if (IsChannelLeft(argChannel))
    {
        ID2D1Bitmap1* bitmap = nullptr;

        hResult = this->mD2DDeviceContext->CreateBitmap(
            D2D1::SizeU(argTextureWidth, argTextureHeight),
            nullptr,
            0,
            bitmapDesc,
            &bitmap);

        this->mGpuBitmapLeft = std::shared_ptr<ID2D1Bitmap1>(bitmap, release_iunknown_deleter);
    }

    if (SUCCEEDED(hResult) && IsChannelRight(argChannel))
    {
        ID2D1Bitmap1* bitmap = nullptr;

        hResult = this->mD2DDeviceContext->CreateBitmap(
            D2D1::SizeU(argTextureWidth, argTextureHeight),
            nullptr,
            0,
            bitmapDesc,
            &bitmap);

            this->mGpuBitmapRight = std::shared_ptr<ID2D1Bitmap1>(bitmap, release_iunknown_deleter);
    }

    if (SUCCEEDED(hResult))
    {
        this->mSystemMemoryTexture = std::unique_ptr<BYTE[]>(new (std::nothrow) BYTE[argTextureWidth * argTextureHeight * 4]);

        if (nullptr == this->mSystemMemoryTexture)
        {
            this->CleanupResources();
            hResult = E_OUTOFMEMORY;
        }
    }

    return hResult;
}

//
// This functions "slices" frames into a system memory atlas texture from a GDI
// buffer at a specified time intervals.
//
// Function does following:
//  1. Calls GetDC, and, if successful, calls CreateCompatibleDC
//  2. Calls CreateDIBSection with the calculated atlas texture width and height
//  3. Select bitmap object for compatible DC
//  4. Loop for the number of frame rows and columns in the atlas texture
//      4.1 If it is not first iteration then sleep for multiple of
//          16 miliseconds defined by the frequency (i.e. argFramesToSkip)
//      4.2 Capture the frame from the DC and copy it into the atlas texture
//  5. Free objects as created in this function
//

DWORD GdiSlicer::ExecuteSlicing(LPVOID argParameter)
{
    // Cast the void pointer to a GdiSlicer
    GdiSlicer* slicer = static_cast<GdiSlicer*>(argParameter);

    // Save the variables from the slicer locally
    RECT   areaToCapture =             slicer->mAreaToCapture;
    SIZE   textureSize =               slicer->mTextureSize;
    HANDLE stopSlicingEvent =          slicer->mStopSlicingEvent;
    BYTE*  texture =                   slicer->mSystemMemoryTexture.get();
    UINT   maxFramesToCapture =        slicer->mMaximumNumberOfFramesToCapture;
    FLOAT  timeToSleepInMilliSeconds = slicer->mFramesToSkip * 16.66f;

    UINT captureWidth = areaToCapture.right - areaToCapture.left;
    UINT captureHeight = areaToCapture.bottom - areaToCapture.top;

    // Calculate the maximum number of frames that can be captured
    UINT columns = textureSize.cx / captureWidth;
    UINT rows = textureSize.cy / captureHeight;

    HDC screenDc = nullptr;
    HDC compatibleScreenDc = nullptr;

    // If the rows or columns are zero, then we can not capture anything
    if (0 == columns || 0 == rows)
    {
        return 1;
    }

    UINT framesCaptured = 0;
    UINT byteWidth = textureSize.cx * 4;
    BYTE* texturePointer = texture;

    LARGE_INTEGER startTime;
    LARGE_INTEGER currentTime;

    if (nullptr == (screenDc = ::GetDC(nullptr)))
    {
        //If GetDC has failed there is nothing to do more
        return ::GetLastError();
    }

    // Create a compatible DC from the Screen DC
    if (nullptr == (compatibleScreenDc = ::CreateCompatibleDC(screenDc)))
    {
        //If CreateCompatibleDC has failed there is nothing to do more
        ::ReleaseDC(nullptr, screenDc);
        return ::GetLastError();
    }


    // Create a bitmap the size of the area being captured
    LONG width = captureWidth;
    LONG height = captureHeight;
    BYTE* bitmapBits = nullptr;
    BITMAPINFO info = {};
    
    info.bmiColors;
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biClrImportant = 0;
    info.bmiHeader.biClrUsed = 0;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biSizeImage = 0;
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biXPelsPerMeter = 0;
    info.bmiHeader.biYPelsPerMeter = 0;

    if (HBITMAP bitmap = ::CreateDIBSection(compatibleScreenDc, &info, DIB_RGB_COLORS, (void**)&bitmapBits, nullptr, 0))
    {
        if (::SelectObject(compatibleScreenDc, bitmap))
        {
            // Loop through the dimensions
            for (UINT r = 0; (r < rows) && (framesCaptured < maxFramesToCapture) && (WAIT_OBJECT_0 != ::WaitForSingleObject(stopSlicingEvent, 0)); ++r)
            {
                for (UINT c = 0; (c < columns) && (framesCaptured < maxFramesToCapture) && (WAIT_OBJECT_0 != ::WaitForSingleObject(stopSlicingEvent, 0)); ++c)
                {
                    BYTE* bitmapBitsCopy = bitmapBits;
                    
                    if (!(r == 0 && c == 0))
                    {
                        ::Sleep(static_cast<DWORD>(timeToSleepInMilliSeconds));
                    }

                    // Check if we have been told to stop slicing
                    if (WAIT_OBJECT_0 == WaitForSingleObject(stopSlicingEvent, 0))
                    {
                        break;
                    }

                    UINT texelX = c * captureWidth;
                    UINT texelY = r * captureHeight;

                    // Calculate the offset into the texture
                    BYTE* subtexturePointer = texturePointer + (((texelY * textureSize.cx) + texelX) * 4);

                    if (::BitBlt(compatibleScreenDc, 0, 0, width, height, screenDc, areaToCapture.left, areaToCapture.top, SRCCOPY))
                    {
                        for (UINT y = 0; y < captureHeight; ++y)
                        {
                            BYTE* currentSubtextureRow = subtexturePointer;
                            // Convert from bgra to rgba
                            for (UINT x = 0; x < captureWidth; ++x)
                            {
                                // For BGRA to RGBA:
                                *(currentSubtextureRow+2) = *(bitmapBitsCopy++);
                                *(currentSubtextureRow+1) = *(bitmapBitsCopy++);
                                *(currentSubtextureRow+0) = *(bitmapBitsCopy++);
                                *(currentSubtextureRow+3) = *(bitmapBitsCopy++);
                                currentSubtextureRow += 4;
                            }
                            subtexturePointer += byteWidth;
                        }
                    }
                    
                    ++framesCaptured;
                }
            }
        }
    
        ::DeleteObject(bitmap);
    }

    ::DeleteDC(compatibleScreenDc);
    ::ReleaseDC(nullptr, screenDc);

    // Set the total frames captured
    slicer->mFramesCaptured = framesCaptured;

    return 0;
}