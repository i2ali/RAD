//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicerOutOfMemory.h"

#define NTSTATUS HRESULT
#include <d3dkmthk.h>

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool DwmSlicerOutOfMemory::ClassSetup(void)
{
    VERIFY_SUCCEEDED(::CreateSlicerFactory(&this->mSlicerFactory));
    VERIFY_SUCCEEDED(this->mSlicerFactory->CreateDwmSlicer(&this->mSlicer));

    return true;
}


bool DwmSlicerOutOfMemory::ClassCleanup(void)
{
    return true;
}


bool DwmSlicerOutOfMemory::MethodSetup(void)
{
    this->mGdi32 = ::LoadLibraryW(L"gdi32.dll");
    VERIFY_IS_NOT_NULL(this->mGdi32);

    return true;
}


bool DwmSlicerOutOfMemory::MethodCleanup(void)
{
    if (nullptr != this->mGdi32)
    {
        ::FreeLibrary(this->mGdi32);
        this->mGdi32 = nullptr;
    }

    return true;
}


void DwmSlicerOutOfMemory::CapturePrimaryDisplay(void)
{
    auto adapters = ::GetAdapters();
    auto devices = ::GetDevices();

    //
    // We need the D3DKMTOpenAdapterFromLuid and D3DKMTQueryAdapterInfo private
    // APIs to check if the enumerated adapters are hardware or software
    //

    PFND3DKMT_OPENADAPTERFROMLUID pfnOpenAdapterFromLuid = nullptr;
    pfnOpenAdapterFromLuid = reinterpret_cast<PFND3DKMT_OPENADAPTERFROMLUID>(::GetProcAddress(this->mGdi32, "D3DKMTOpenAdapterFromLuid"));
    VERIFY_IS_NOT_NULL(pfnOpenAdapterFromLuid);

    PFND3DKMT_QUERYADAPTERINFO pfnOsThunkDDIQueryAdapterInfo = nullptr;
    pfnOsThunkDDIQueryAdapterInfo = reinterpret_cast<PFND3DKMT_QUERYADAPTERINFO>(::GetProcAddress(this->mGdi32, "D3DKMTQueryAdapterInfo"));
    VERIFY_IS_NOT_NULL(pfnOsThunkDDIQueryAdapterInfo);

    std::shared_ptr<IDXGIAdapter> adapter;

    // Find the first hardware device that we can use to test low memory scenarios
    for (UINT i = 0; i < adapters.size(); ++i)
    {
        DXGI_ADAPTER_DESC adapterDesc = {};
        VERIFY_SUCCEEDED(adapters[i]->GetDesc(&adapterDesc));

        D3DKMT_OPENADAPTERFROMLUID openAdapterFromLuid = {};
        openAdapterFromLuid.AdapterLuid = adapterDesc.AdapterLuid;

        VERIFY_SUCCEEDED(HRESULT_FROM_NT(pfnOpenAdapterFromLuid(&openAdapterFromLuid)));

        D3DKMT_QUERYADAPTERINFO info = {};
        D3DKMT_ADAPTERTYPE adapterType = {};

        info.hAdapter   = openAdapterFromLuid.hAdapter;
        info.Type       = KMTQAITYPE_ADAPTERTYPE;
        info.PrivateDriverDataSize = sizeof(D3DKMT_ADAPTERTYPE);
        info.pPrivateDriverData = &adapterType;

        VERIFY_SUCCEEDED(HRESULT_FROM_NT(pfnOsThunkDDIQueryAdapterInfo(&info)));

        // We found a hardware device
        if (false == !!adapterType.SoftwareDevice)
        {
            adapter = adapters[i];
            break;
        }
    }

    if (nullptr == adapter)
    {
        Log::Comment(L"No hardware adapters were found in the system");
        return;
    }

    CComPtr<IDXGIOutput> output;
    VERIFY_SUCCEEDED(adapter->EnumOutputs(0, &output));

    DXGI_OUTPUT_DESC outputDesc = {};
    VERIFY_SUCCEEDED(output->GetDesc(&outputDesc));

    RECT topLeft;
    topLeft.top =    outputDesc.DesktopCoordinates.top;
    topLeft.left =   outputDesc.DesktopCoordinates.left;
    topLeft.bottom = outputDesc.DesktopCoordinates.top + 100;
    topLeft.right =  outputDesc.DesktopCoordinates.left + 100;

    // Non-stereo
    {
        // First do a regular capture
        UINT numberOfFramesThatCanBeCaptured = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 100000, 0, RAD::Channel::Left, &numberOfFramesThatCanBeCaptured));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

        ::Sleep(500);

        UINT framesCaptured = 0;
        CComPtr<ID2D1Bitmap1> leftChannel;
        CComPtr<ID2D1Bitmap1> rightChannel;
        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

        Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));


        // Allocate a lot of video memory
        auto device = devices[adapter];

        HRESULT hResult = NOERROR;
        ID3D11Texture2D* texture;
        std::vector<std::shared_ptr<ID3D11Texture2D>> textures;

        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // Shared surfaces must have these bind flags
        textureDesc.CPUAccessFlags = 0;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.Height = 1000;
        textureDesc.MipLevels = 1;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.Width = 1000;

        while (SUCCEEDED(hResult = device->CreateTexture2D(&textureDesc, nullptr, &texture)))
        {
            textures.push_back(std::shared_ptr<ID3D11Texture2D>(texture, release_iunknown_deleter));
            texture = nullptr;
        }

        VERIFY_ARE_EQUAL(E_OUTOFMEMORY, hResult);

        // Pop one texture from the back
        textures.pop_back();

        // Now try slicing with limited memory
        UINT numberOfFramesThatCanBeCapturedWithLimitedMemory = 0;
        HRESULT hr = this->mSlicer->StartCapture(topLeft, 100000, 0, RAD::Channel::Left, &numberOfFramesThatCanBeCapturedWithLimitedMemory);

        static const HRESULT E_COMMITMENT_LIMIT = __HRESULT_FROM_WIN32(ERROR_COMMITMENT_LIMIT);
        switch (hr)
        {
            case E_COMMITMENT_LIMIT:
            {
                Log::Comment(L"Got ERROR_COMMITMENT_LIMIT... Skipping test.");
                break;
            }

            case E_OUTOFMEMORY:
            {
                Log::Comment(L"Got E_OUTOFMEMORY... Skipping test.");
                break;
            }

            default:
            {
                VERIFY_SUCCEEDED(hr);
                Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCapturedWithLimitedMemory));

                VERIFY_IS_LESS_THAN_OR_EQUAL(numberOfFramesThatCanBeCapturedWithLimitedMemory, numberOfFramesThatCanBeCaptured);

                ::Sleep(500);

                UINT framesCapturedWithLimitedMemory = 0;
                CComPtr<ID2D1Bitmap1> leftChannelWithLimitedMemory;
                CComPtr<ID2D1Bitmap1> rightChannelWithLimitedMemory;
                VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCapturedWithLimitedMemory, &leftChannelWithLimitedMemory, &rightChannelWithLimitedMemory));

                Log::Comment(String().Format(L"Frames captured: %d", framesCapturedWithLimitedMemory));

                VERIFY_IS_LESS_THAN_OR_EQUAL(framesCapturedWithLimitedMemory, numberOfFramesThatCanBeCapturedWithLimitedMemory);

                break;
            }
        }
    }

    // Windows 8 Bugs #11553
    // Won't fix for Windows 8; fix needed for Windows 9 MQ
    // TEST - Uncomment this code when the bug is fixed
    /*
    // Stereo
    {
        UINT numberOfFramesThatCanBeCaptured = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 100000, 0, RAD::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

        ::Sleep(500);

        UINT framesCaptured = 0;
        CComPtr<ID2D1Bitmap1> leftChannel;
        CComPtr<ID2D1Bitmap1> rightChannel;
        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

        Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));

        // Allocate a lot of video memory
        auto device = devices[adapter];

        HRESULT hResult = NOERROR;
        ID3D11Texture2D* texture;
        std::vector<std::shared_ptr<ID3D11Texture2D>> textures;

        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // Shared surfaces must have these bind flags
        textureDesc.CPUAccessFlags = 0;
        textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textureDesc.Height = 1000;
        textureDesc.MipLevels = 1;
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality = 0;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.Width = 1000;

        while (SUCCEEDED(hResult = device->CreateTexture2D(&textureDesc, nullptr, &texture)))
        {
            textures.push_back(std::shared_ptr<ID3D11Texture2D>(texture, release_iunknown_deleter));
            texture = nullptr;
        }

        VERIFY_ARE_EQUAL(E_OUTOFMEMORY, hResult);

        // Pop one texture from the back
        textures.pop_back();

        // Now try slicing with limited memory
        UINT numberOfFramesThatCanBeCapturedWithLimitedMemory = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 100000, 0, RAD::Channel::LeftAndRight, &numberOfFramesThatCanBeCapturedWithLimitedMemory));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCapturedWithLimitedMemory));

        VERIFY_IS_LESS_THAN_OR_EQUAL(numberOfFramesThatCanBeCapturedWithLimitedMemory, numberOfFramesThatCanBeCaptured);

        ::Sleep(500);

        UINT framesCapturedWithLimitedMemory = 0;
        CComPtr<ID2D1Bitmap1> leftChannelWithLimitedMemory;
        CComPtr<ID2D1Bitmap1> rightChannelWithLimitedMemory;
        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCapturedWithLimitedMemory, &leftChannelWithLimitedMemory, &rightChannelWithLimitedMemory));

        Log::Comment(String().Format(L"Frames captured: %d", framesCapturedWithLimitedMemory));
    }
    */
}