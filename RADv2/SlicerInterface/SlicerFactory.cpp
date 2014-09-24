//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "SlicerFactory.h"
#include "DwmSlicer.h"
#include "GdiSlicer.h"

#include <algorithm>
#include <tuple>

#define NTSTATUS HRESULT
#include <d3dkmthk.h>

using namespace RADv2;

template<typename Interface> inline void SafeRelease(Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}

// If check cleanup
#define IFC(expr) do { hResult = (expr); if (FAILED(hResult)) { goto Cleanup; } } while (0, 0)
#define IFCOOM(expr) do { if (nullptr == (expr)) { hResult = E_OUTOFMEMORY; goto Cleanup; } } while (0, 0)
#define IFCNULL(expr) do { if (nullptr == (expr)) { hResult = E_INVALIDARG; goto Cleanup; } } while (0, 0)

// If check return
#define IFR(expr) do { hResult = (expr); if (FAILED(hResult)) { return hResult; } } while (0, 0)
#define IFROOM(expr) do { if (nullptr == (expr)) { return E_OUTOFMEMORY; } } while (0, 0)
#define IFRNULL(expr) do { if (nullptr == (expr)) { return E_INVALIDARG; } } while (0, 0)
#define IFRFALSE(expr) do { if (false == (expr)) { return E_INVALIDARG; } } while (0, 0)
#define IFRTRUE(expr) do { if (true == (expr)) { return E_INVALIDARG; } } while (0, 0)


//
// The function is declared in ISlicerInterface.h
//
HRESULT CreateSlicerFactory(RADv2::ISlicerFactory** argSlicerFactory, RADv2::ComputeShaderModel argComputeShaderModelRequired)
{
    return RADv2::CreateSlicerFactory(argSlicerFactory, argComputeShaderModelRequired);
}


//
// The function is declared as a friend in SlicerFactory.h
//
HRESULT RADv2::CreateSlicerFactory(RADv2::ISlicerFactory** argSlicerFactory, RADv2::ComputeShaderModel argComputeShaderModelRequired)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argSlicerFactory);

    // Set the output to nullptr
    *argSlicerFactory = nullptr;

    // Create a new slicer factory
    SlicerFactory* slicerFactory = new SlicerFactory(argComputeShaderModelRequired);
    IFCOOM(slicerFactory);

    // Initialize the slicer factory
    IFC(slicerFactory->Initialize());

    // Set the output to the new slicer factory and AddRef
    *argSlicerFactory = slicerFactory;
    slicerFactory->AddRef();


Cleanup:

    // Initialize failed, then delete the slicer factory
    if (FAILED(hResult))
    {
        if (nullptr != slicerFactory)
        {
            delete slicerFactory;
        }
    }

    return hResult;
}


//
// Constructor
//

SlicerFactory::SlicerFactory(RADv2::ComputeShaderModel argComputeShaderModelRequired)
    : mComputeShaderModelRequired(argComputeShaderModelRequired), mRefCount(0), mDxgiFactory(nullptr), mD2DFactory(nullptr), mD2DDevice(nullptr),
      mD2DDeviceContext(nullptr), mLowestFeatureLevelIndex(0), mHighestFeatureLevelIndex(0)
{
}


HRESULT SlicerFactory::Initialize(void)
{
    const D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    typedef std::tuple<std::shared_ptr<IDXGIAdapter>, D3D_FEATURE_LEVEL, bool> AdapterInfo;
    std::vector<AdapterInfo> adapters;

    HRESULT      hResult;
    IDXGIDevice* pDxgiDevice = nullptr;
    HMODULE      hModule = nullptr;


    //
    // We need the D3DKMTOpenAdapterFromLuid and D3DKMTQueryAdapterInfo private
    // APIs to check if the enumerated adapters are hardware or software
    //
    hModule = ::LoadLibraryW(L"gdi32.dll");

    if (nullptr == hModule)
    {
        IFC(E_NOINTERFACE);
    }

    PFND3DKMT_OPENADAPTERFROMLUID pfnOpenAdapterFromLuid = nullptr;
    pfnOpenAdapterFromLuid = reinterpret_cast<PFND3DKMT_OPENADAPTERFROMLUID>(::GetProcAddress(hModule, "D3DKMTOpenAdapterFromLuid"));

    if (nullptr == pfnOpenAdapterFromLuid)
    {
        IFC(E_NOINTERFACE);
    }

    PFND3DKMT_QUERYADAPTERINFO pfnOsThunkDDIQueryAdapterInfo = nullptr;
    pfnOsThunkDDIQueryAdapterInfo = reinterpret_cast<PFND3DKMT_QUERYADAPTERINFO>(::GetProcAddress(hModule, "D3DKMTQueryAdapterInfo"));

    if (nullptr == pfnOsThunkDDIQueryAdapterInfo)
    {
        IFC(E_NOINTERFACE);
    }

    //
    // Create a DXGI factory
    //
    IDXGIFactory* dxgiFactory = nullptr;
    IFC(::CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
    this->mDxgiFactory = std::shared_ptr<IDXGIFactory>(dxgiFactory, release_iunknown_deleter);

    //
    // Create a D2D factory
    //
    ID2D1Factory1* d2d1Factory = nullptr;
    IFC(::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d1Factory));
    this->mD2DFactory = std::shared_ptr<ID2D1Factory1>(d2d1Factory, release_iunknown_deleter);

    //
    // Enumerate all adapters in the machine
    //
    IDXGIAdapter* currentAdapter = nullptr;
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != mDxgiFactory->EnumAdapters(i, &currentAdapter); ++i)
    {
        if (nullptr != currentAdapter)
        {
            // Save the adapter in a smart pointer
            std::shared_ptr<IDXGIAdapter> adapter(currentAdapter, release_iunknown_deleter);

            D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

            UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

            #ifdef DBG
                // This requires DXGIDebug.dll and *SDKLayeres.dll on the system
                createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
            #endif

            // Get the feature level of the adapter without creating a device
            IFC(::D3D11CreateDevice(
                currentAdapter,
                D3D_DRIVER_TYPE_UNKNOWN,
                nullptr,
                createDeviceFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                nullptr,
                &featureLevel,
                nullptr));

            // Determine if the adapter is software or hardware
            DXGI_ADAPTER_DESC adapterDesc = {};
            IFC(adapter->GetDesc(&adapterDesc));

            D3DKMT_OPENADAPTERFROMLUID openAdapterFromLuid = {};
            openAdapterFromLuid.AdapterLuid = adapterDesc.AdapterLuid;

            IFC(HRESULT_FROM_NT(pfnOpenAdapterFromLuid(&openAdapterFromLuid)));

            D3DKMT_QUERYADAPTERINFO info = {};
            D3DKMT_ADAPTERTYPE adapterType = {};

            info.hAdapter   = openAdapterFromLuid.hAdapter;
            info.Type       = KMTQAITYPE_ADAPTERTYPE;
            info.PrivateDriverDataSize = sizeof(D3DKMT_ADAPTERTYPE);
            info.pPrivateDriverData = &adapterType;

            IFC(HRESULT_FROM_NT(pfnOsThunkDDIQueryAdapterInfo(&info)));

            bool softwareDevice = !!adapterType.SoftwareDevice;

            #ifdef DBG
            std::wcout << L"Adapter Information: " << std::endl <<
                          L"\tDescription: " << adapterDesc.Description << std::endl <<
                          L"\tFeature Level: " << std::hex << featureLevel << std::endl <<
                          L"\tSoftware: " << softwareDevice << std::endl;
            #endif

            // Save the adapter
            adapters.push_back(AdapterInfo(adapter, featureLevel, softwareDevice));

            currentAdapter = nullptr;
        }
    }

    // Make sure we got at least one adapter
    if (adapters.empty())
    {
        IFC(E_UNEXPECTED);
    }

    // Sort the adapters by adapter type, and then by feature level
    std::sort(adapters.begin(), adapters.end(), [](AdapterInfo& left, AdapterInfo& right)->bool
    {
        bool softwareAdapterLeft = std::get<2>(left);
        bool softwareAdapterRight = std::get<2>(right);

        if (softwareAdapterLeft == softwareAdapterRight)
        {
            D3D_FEATURE_LEVEL featureLevelLeft = std::get<1>(left);
            D3D_FEATURE_LEVEL featureLevelRight = std::get<1>(right);

            return (featureLevelLeft > featureLevelRight);
        }
        else
        {
            return !softwareAdapterLeft;
        }
    });

    // Copy the adapters to the class member
    for (UINT i = 0; i < adapters.size(); ++i)
    {
        this->mDxgiAdapters.push_back(std::get<0>(adapters[i]));

        #ifdef DBG
        DXGI_ADAPTER_DESC adapterDesc = {};
        IFC(std::get<0>(adapters[i])->GetDesc(&adapterDesc));

        std::wcout << L"Adapters: " << std::endl <<
                      L"\tDescription: " << adapterDesc.Description << std::endl <<
                      L"\tFeature Level: " << std::hex << std::get<1>(adapters[i]) << std::endl <<
                      L"\tSoftware: " << std::get<2>(adapters[i]) << std::endl;
        #endif
    }


    //
    // Create a hardware device for each adapter and find the device that will
    // satisfy the compute shader requirement
    //
    ID3D11Device* bestDevice = nullptr;

    for (UINT i = 0; i < mDxgiAdapters.size(); ++i)
    {
        ID3D11Device*     hardwareDevice = nullptr;
        D3D_FEATURE_LEVEL hardwareFeatureLevel = D3D_FEATURE_LEVEL_11_1;

        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        #ifdef DBG
            // This requires DXGIDebug.dll and *SDKLayeres.dll on the system
            flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif

        IFC(::D3D11CreateDevice(
            mDxgiAdapters[i].get(),
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            flags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &hardwareDevice,
            &hardwareFeatureLevel,
            nullptr));

        // Save the hardware device
        mD3DDevices.push_back(std::shared_ptr<ID3D11Device>(hardwareDevice, release_iunknown_deleter));
        mD3DFeatureLevels.push_back(hardwareFeatureLevel);

        // Check if this device satisfies the compute shader model requirement
        if (nullptr == bestDevice)
        {
            switch (this->mComputeShaderModelRequired)
            {
                case ComputeShaderModel::CS_5_0:
                {
                    if (hardwareFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
                    {
                        bestDevice = hardwareDevice;
                    }

                    break;
                }

                case ComputeShaderModel::CS_4_1:
                {
                    if (hardwareFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
                    {
                        bestDevice = hardwareDevice;
                    }
                    else if (hardwareFeatureLevel >= D3D_FEATURE_LEVEL_10_1)
                    {
                        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS options = {};
                        IFC(hardwareDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &options, sizeof(options)));

                        if (TRUE == options.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
                        {
                            bestDevice = hardwareDevice;
                        }
                    }

                    break;
                }

                case ComputeShaderModel::CS_4_0:
                {
                    if (hardwareFeatureLevel >= D3D_FEATURE_LEVEL_11_0)
                    {
                        bestDevice = hardwareDevice;
                    }
                    else if (hardwareFeatureLevel >= D3D_FEATURE_LEVEL_10_0)
                    {
                        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS options = {};
                        IFC(hardwareDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &options, sizeof(options)));

                        if (TRUE == options.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
                        {
                            bestDevice = hardwareDevice;
                        }
                    }

                    break;
                }

                case ComputeShaderModel::None:
                default:
                {
                    bestDevice = hardwareDevice;
                    break;
                }
            }

            #ifdef DBG
            if (nullptr != bestDevice)
            {
                DXGI_ADAPTER_DESC adapterDesc = {};
                IFC(mDxgiAdapters[i]->GetDesc(&adapterDesc));

                std::wcout << L"Best Device: " << std::endl <<
                                L"\tDescription: " << adapterDesc.Description << std::endl <<
                                L"\tFeature Level: " << std::hex << hardwareFeatureLevel << std::endl;
            }
            #endif
        }
    }

    if (nullptr == bestDevice)
    {
        IFC(E_UNEXPECTED);
    }

    //
    // Create a D2D device from the D3D device
    //
    IFC(bestDevice->QueryInterface(&pDxgiDevice));

    ID2D1Device* d2dDevice = nullptr;
    IFC(mD2DFactory->CreateDevice(pDxgiDevice, &d2dDevice));
    this->mD2DDevice = std::shared_ptr<ID2D1Device>(d2dDevice, release_iunknown_deleter);

    ID2D1DeviceContext* d2dDeviceContext = nullptr;
    IFC(mD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext));
    this->mD2DDeviceContext = std::shared_ptr<ID2D1DeviceContext>(d2dDeviceContext, release_iunknown_deleter);

    mD2DFeatureLevel = bestDevice->GetFeatureLevel();


Cleanup:

    if (nullptr != hModule)
    {
        ::FreeLibrary(hModule);
    }

    SafeRelease(&pDxgiDevice);

    return hResult;
}


//
// Destructor
//

SlicerFactory::~SlicerFactory(void)
{
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE SlicerFactory::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);
    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE SlicerFactory::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE SlicerFactory::QueryInterface(REFIID riid, LPVOID* ppvObj)
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
    else if (riid == __uuidof(ISlicerFactory))
    {
        *ppvObj = static_cast<ISlicerFactory*>(this);
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
// Properties
//

HRESULT SlicerFactory::GetD2DDevice(ID2D1Device** argDevice) const
{
    IFRNULL(argDevice);

    *argDevice = this->mD2DDevice.get();
    this->mD2DDevice->AddRef();

    return NOERROR;
}


ID2D1Device* SlicerFactory::GetD2DDeviceNoRef(void) const
{
    return this->mD2DDevice.get();
}


HRESULT SlicerFactory::GetD2DFactory(ID2D1Factory1** argFactory) const
{
    IFRNULL(argFactory);

    *argFactory = this->mD2DFactory.get();
    this->mD2DFactory->AddRef();

    return NOERROR;
}


ID2D1Factory1* SlicerFactory::GetD2DFactoryNoRef(void) const
{
    return this->mD2DFactory.get();
}


D3D_FEATURE_LEVEL SlicerFactory::GetD2DFeatureLevel(void) const
{
    return this->mD2DFeatureLevel;
}


D3D_FEATURE_LEVEL SlicerFactory::GetHighestFeatureLevel(void) const
{
    return this->mD3DFeatureLevels[mHighestFeatureLevelIndex];
}


D3D_FEATURE_LEVEL SlicerFactory::GetLowestFeatureLevel(void) const
{
    return this->mD3DFeatureLevels[mLowestFeatureLevelIndex];
}


//
// Public Methods
//

HRESULT SlicerFactory::CreateDwmSlicer(IDwmSlicer** argDwmSlicer)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argDwmSlicer);

    *argDwmSlicer = nullptr;

    // Create a new DWM slicer
    DwmSlicer* dwmSlicer = new DwmSlicer();
    IFCOOM(dwmSlicer);

    // Initialize the slicer
    IFC(dwmSlicer->Initialize(this->mDxgiAdapters, this->mD3DDevices, this->mD2DDevice));

    // Assign and AddRef
    *argDwmSlicer = dwmSlicer;
    dwmSlicer->AddRef();


Cleanup:

    if (FAILED(hResult))
    {
        if (nullptr != dwmSlicer)
        {
            delete dwmSlicer;
        }
    }

    return hResult;
}


HRESULT SlicerFactory::CreateGdiSlicer(IGdiSlicer** argGdiSlicer)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argGdiSlicer);

    *argGdiSlicer = nullptr;

    // Create a new GDI slicer
    GdiSlicer* gdiSlicer = new GdiSlicer();
    IFCOOM(gdiSlicer);

    // Initialize the slicer
    IFC(gdiSlicer->Initialize(this->mD2DDevice, this->mD2DFeatureLevel));

    // Assign and AddRef
    *argGdiSlicer = gdiSlicer;
    gdiSlicer->AddRef();


Cleanup:

    if (FAILED(hResult))
    {
        if (nullptr != gdiSlicer)
        {
            delete gdiSlicer;
        }
    }

    return hResult;
}


HRESULT SlicerFactory::CreateFrame(const D2D1_SIZE_U& argFrameSize, UINT argFrameIndex, ID2D1Bitmap1* argAtlas, ID2D1Bitmap1** argFrame, D2D1_BITMAP_OPTIONS argOptions)
{
    HRESULT hResult;

    if (nullptr == argAtlas || nullptr == argFrame)
    {
        return E_INVALIDARG;
    }

    if (0 == argFrameSize.width || 0 == argFrameSize.height)
    {
        return E_INVALIDARG;
    }

    D2D1_SIZE_F atlasSize = argAtlas->GetSize();
    UINT columns = (UINT)atlasSize.width / argFrameSize.width;
    UINT rows = (UINT)atlasSize.height / argFrameSize.height;

    // Calculate the source frame coordinates
    UINT row = argFrameIndex / columns;
    UINT column = argFrameIndex % columns;

    if (column >= columns || row >= rows)
    {
        return E_INVALIDARG;
    }

    D2D1_RECT_U sourceRect;
    sourceRect.left = column * argFrameSize.width;
    sourceRect.right = sourceRect.left + argFrameSize.width;
    sourceRect.top = row * argFrameSize.height;
    sourceRect.bottom = sourceRect.top + argFrameSize.height;

    D2D1_BITMAP_PROPERTIES1 desc = {};
    argAtlas->GetColorContext(&desc.colorContext);
    argAtlas->GetDpi(&desc.dpiX, &desc.dpiY);
    desc.pixelFormat = argAtlas->GetPixelFormat();
    desc.bitmapOptions = argOptions;

    ID2D1Bitmap1* frame = nullptr;
    IFC(this->mD2DDeviceContext->CreateBitmap(argFrameSize, nullptr, 0, &desc, &frame));
    IFC(frame->CopyFromBitmap(nullptr, argAtlas, &sourceRect));


Cleanup:

    SafeRelease(&desc.colorContext);

    if (FAILED(hResult))
    {
        if (nullptr != frame)
        {
            frame->Release();
            frame = nullptr;
        }
    }

    *argFrame = frame;

    return hResult;
}


HRESULT SlicerFactory::LoadImageFromFile(wchar_t const * const argFileName, ID2D1Bitmap1** argBitmapOut)
{
    HRESULT hResult;

    CComPtr<IWICImagingFactory>    factory;
    CComPtr<IWICStream>            targetStream;
    CComPtr<IWICBitmapDecoder>     decoder;
    CComPtr<IWICBitmapFrameDecode> frame;
    CComPtr<IWICFormatConverter>   converter;

    // Create a WICImagingFactory
    IFC(factory.CoCreateInstance(CLSID_WICImagingFactory));

    // Create and initialize a WIC Stream
    IFC(factory->CreateStream(&targetStream));
    IFC(targetStream->InitializeFromFilename(argFileName, GENERIC_READ));

    // Create and initialize a decoder from the stream
    IFC(factory->CreateDecoderFromStream(targetStream, nullptr, WICDecodeMetadataCacheOnDemand, &decoder));

    // Grab the first frame from the image
    IFC(decoder->GetFrame(0, &frame));

    // Check the pixel format so that we can translate it to the correct format
    WICPixelFormatGUID pixelFormat;
    IFC(frame->GetPixelFormat(&pixelFormat));

    // Create and initialize a WIC Converter
    IFC(factory->CreateFormatConverter(&converter));
    IFC(converter->Initialize(frame, GUID_WICPixelFormat32bppPRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom));

    // Check if we can convert
    BOOL canConvert = FALSE;
    IFC(converter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppPRGBA, &canConvert));

    // Throw if we cannot convert
    if (!canConvert)
    {
        IFC(E_ABORT);
    }

    D2D1_BITMAP_PROPERTIES bitmapDesc;
    bitmapDesc.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bitmapDesc.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapDesc.dpiX = 96;
    bitmapDesc.dpiY = 96;

    IFC(this->mD2DDeviceContext->CreateBitmapFromWicBitmap(
        converter,
        bitmapDesc,
        (ID2D1Bitmap**)argBitmapOut));


Cleanup:

    return hResult;
}


HRESULT SlicerFactory::RenderOutput(ID2D1Image* argEffectOutput)
{
    HRESULT hResult;

    D2D1_BITMAP_PROPERTIES1 renderTargetProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                D2D1_ALPHA_MODE_IGNORE));

    CComPtr<ID2D1Bitmap1> renderTarget;
    this->mD2DDeviceContext->CreateBitmap(
        D2D1::SizeU(1, 1),
        nullptr,
        0,
        &renderTargetProperties,
        &renderTarget);

    this->mD2DDeviceContext->SetTarget(renderTarget);

    this->mD2DDeviceContext->BeginDraw();
    this->mD2DDeviceContext->DrawImage(argEffectOutput);
    IFC(this->mD2DDeviceContext->EndDraw());


Cleanup:

    return hResult;
}

HRESULT SlicerFactory::SaveImageToFile(wchar_t const * const argFileName, ID2D1Image* argImage, const GUID& argFileFormat)
{
    return SaveImageToFile(argFileName, argImage, argFileFormat, nullptr, nullptr);
}

HRESULT SlicerFactory::SaveImageToFile(wchar_t const * const argFileName, ID2D1Image* argImage, const GUID& argFileFormat, _In_opt_ PWSTR pszFrameWidth, _In_opt_ PWSTR pszFrameHeight)
{
    HRESULT hResult;
    CComPtr<IWICImagingFactory>    imagingFactory;
    CComPtr<IWICImagingFactory2>   imagingFactory2;
    CComPtr<IWICStream>            targetStream;
    CComPtr<IWICBitmapEncoder>     bitmapEncoder;
    CComPtr<IWICBitmapFrameEncode> targetFrame;
    CComPtr<IWICImageEncoder>      imageEncoder;

    // Validate the argument
    IFRNULL(argImage);

    // Create a WICImagingFactory
    IFC(imagingFactory.CoCreateInstance(CLSID_WICImagingFactory));
    IFC(imagingFactory->QueryInterface<IWICImagingFactory2>(&imagingFactory2));

    // Create and initialize a WIC Stream
    IFC(imagingFactory2->CreateStream(&targetStream));
    IFC(targetStream->InitializeFromFilename(argFileName, GENERIC_WRITE));

    // Create an image encoder
    IFC(imagingFactory2->CreateImageEncoder(this->mD2DDevice.get(), &imageEncoder));

    // Create and initialize a PNG Encoder
    IFC(imagingFactory2->CreateEncoder(argFileFormat, nullptr, &bitmapEncoder));
    IFC(bitmapEncoder->Initialize(targetStream, WICBitmapEncoderNoCache));

    // Create a new frame from the encoder
    IFC(bitmapEncoder->CreateNewFrame(&targetFrame, nullptr));
    IFC(targetFrame->Initialize(nullptr));
    
    if ((nullptr != pszFrameWidth) && (nullptr != pszFrameHeight))
    {
        CComPtr<IWICMetadataQueryWriter> frameQWriter;
        IFC(targetFrame->GetMetadataQueryWriter(&frameQWriter));

        PROPVARIANT value;

        value.vt = VT_LPWSTR;
        value.pwszVal = pszFrameWidth;
        IFC(frameQWriter->SetMetadataByName(L"/[*]tEXt/{str=FrameWidth}", &value));

        value.vt = VT_LPWSTR;
        value.pwszVal = pszFrameHeight;
        IFC(frameQWriter->SetMetadataByName(L"/[*]tEXt/{str=FrameHeight}", &value));
    }

    // Write the image to the frame and commit
    IFC(imageEncoder->WriteFrame(argImage, targetFrame, nullptr));
    IFC(targetFrame->Commit());
    IFC(bitmapEncoder->Commit());


Cleanup:

    return hResult;
}
