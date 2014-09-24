//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#ifdef DBG
#include <iostream>
#endif

using namespace WEX::Common;
using namespace WEX::Logging;

void release_iunknown_deleter(IUnknown* unknown)
{
    if (nullptr != unknown)
    {
        #ifdef DBG
        std::wcout << std::endl << L"Releasing: 0x" << unknown << std::endl;
        #endif
        unknown->Release();
    }
}

std::shared_ptr<IDXGIFactory> gDxgiFactory;
std::vector<std::shared_ptr<IDXGIAdapter>> gAdapters;
std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>> gOutputs;
std::map<std::shared_ptr<IDXGIAdapter>, std::shared_ptr<ID3D11Device>> gDevices;

bool ModuleSetup(void)
{
    VERIFY_SUCCEEDED(::CoInitialize(nullptr));

    // Create a DXGI factory
    IDXGIFactory* dxgiFactory = nullptr;
    VERIFY_SUCCEEDED(::CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
    gDxgiFactory = std::shared_ptr<IDXGIFactory>(dxgiFactory, release_iunknown_deleter);

    // Enumerate all adapters in the machine
    IDXGIAdapter* adapter = nullptr;
    for (UINT i = 0; DXGI_ERROR_NOT_FOUND != gDxgiFactory->EnumAdapters(i, &adapter); ++i)
    {
        if (nullptr != adapter)
        {
            gAdapters.push_back(std::shared_ptr<IDXGIAdapter>(adapter, release_iunknown_deleter));
            adapter = nullptr;
        }
    }

    // Get the outputs for each adapter
    for (UINT i = 0; i < gAdapters.size(); ++i)
    {
        IDXGIOutput* output = nullptr;
        for (UINT outputIndex = 0; DXGI_ERROR_NOT_FOUND != gAdapters[i]->EnumOutputs(outputIndex, &output); ++outputIndex)
        {
            if (nullptr != output)
            {
                gOutputs[gAdapters[i]].push_back(std::shared_ptr<IDXGIOutput>(output, release_iunknown_deleter));
                output = nullptr;
            }
        }
    }

    // Create a device for each adapter
    for (UINT i = 0; i < gAdapters.size(); ++i)
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

        ID3D11Device* hardwareDevice = nullptr;
        D3D_FEATURE_LEVEL hardwareFeatureLevel;

        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        VERIFY_SUCCEEDED(::D3D11CreateDevice(
            gAdapters[i].get(),
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            flags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &hardwareDevice,
            &hardwareFeatureLevel,
            nullptr));

        gDevices[gAdapters[i]] = std::shared_ptr<ID3D11Device>(hardwareDevice, release_iunknown_deleter);
    }

    return true;
}


bool ModuleCleanup(void)
{
    gDevices.clear();
    gOutputs.clear();
    gAdapters.clear();
    gDxgiFactory = nullptr;

    ::CoUninitialize();

    return true;
}


std::vector<std::shared_ptr<IDXGIAdapter>> GetAdapters(void)
{
    return gAdapters;
}


std::map<std::shared_ptr<IDXGIAdapter>, std::shared_ptr<ID3D11Device>> GetDevices(void)
{
    return gDevices;
}


D3D_FEATURE_LEVEL GetFeatureLevelOfAdapter(std::shared_ptr<IDXGIAdapter> argAdapter)
{
    return gDevices[argAdapter]->GetFeatureLevel();
}


std::shared_ptr<IDXGIAdapter> GetPrimaryAdapter(void)
{
    return gAdapters[0];
}


std::shared_ptr<ID3D11Device> GetPrimaryDevice(void)
{
    return gDevices[gAdapters[0]];
}


std::shared_ptr<IDXGIOutput> GetPrimaryOutput(void)
{
    return gOutputs[gAdapters[0]][0];
}


std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>> GetOutputsFromScreenRect(RECT argScreenRect)
{
    std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>> adapterToOutputsMap;

    // Loop over each adapter
    for (auto iter = gOutputs.begin(); iter != gOutputs.end(); ++iter)
    {
        // Loop over each output connected to the adapter
        for (UINT i = 0; i < iter->second.size(); ++i)
        {
            DXGI_OUTPUT_DESC desc = {};
            VERIFY_SUCCEEDED(iter->second[i]->GetDesc(&desc));

            // If the specified screen rect intersects with the desktop
            // coordinates of the monitor, then save the output
            RECT temp = {};
            if (FALSE != ::IntersectRect(&temp, &argScreenRect, &desc.DesktopCoordinates))
            {
                adapterToOutputsMap[iter->first].push_back(iter->second[i]);
            }
        }
    }

    return adapterToOutputsMap;
}


std::map<std::shared_ptr<ID3D11Texture2D>, SLICER_SHARED_TEXTURE> CreateTexturesForOutputs(
    const std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>>& argOutputs,
    UINT argTextureWidth,
    UINT argTextureHeight)
{
    std::map<std::shared_ptr<ID3D11Texture2D>, SLICER_SHARED_TEXTURE> sharedTextures;

    D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.ArraySize = 1;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET; // Shared surfaces must have these bind flags
    textureDesc.CPUAccessFlags = 0;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.Height = argTextureHeight;
    textureDesc.MipLevels = 1;
    textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.Width = argTextureWidth;

    for (auto iter = argOutputs.begin(); iter != argOutputs.end(); ++iter)
    {
        // Get the adapter LUID
        DXGI_ADAPTER_DESC adapterDesc = {};
        VERIFY_SUCCEEDED(iter->first->GetDesc(&adapterDesc));

        std::shared_ptr<ID3D11Device> device = gDevices[iter->first];

        for (UINT i = 0; i < iter->second.size(); ++i)
        {
            // Get the output HMONITOR
            DXGI_OUTPUT_DESC outputDesc;
            VERIFY_SUCCEEDED(iter->second[i]->GetDesc(&outputDesc));

            ID3D11Texture2D* t = nullptr;
            VERIFY_SUCCEEDED(device->CreateTexture2D(&textureDesc, nullptr, &t));
            std::shared_ptr<ID3D11Texture2D> texture = std::shared_ptr<ID3D11Texture2D>(t, release_iunknown_deleter);

            // Get the shared handle
            HANDLE sharedHandle = 0;
            CComPtr<IDXGIResource> dxgiResource;

            VERIFY_SUCCEEDED(texture->QueryInterface(__uuidof(IDXGIResource), (void**)&dxgiResource));
            VERIFY_SUCCEEDED(dxgiResource->GetSharedHandle(&sharedHandle));

            Log::Comment(String().Format(L"Shared texture %d:", i));
            Log::Comment(String().Format(L"\tAdapter:        %s", adapterDesc.Description));
            Log::Comment(String().Format(L"\tShared Handle:  0x%X", sharedHandle));
            Log::Comment(String().Format(L"\tMonitor Handle: 0x%X", outputDesc.Monitor));
            Log::Comment(String().Format(L"\tAdapter LUID:   0n%d, 0n%d", adapterDesc.AdapterLuid.HighPart, adapterDesc.AdapterLuid.LowPart));

            // Fill out the shared texture for this output
            SLICER_SHARED_TEXTURE sharedTexture = {};
            sharedTexture.AdapterLuid = adapterDesc.AdapterLuid;
            sharedTexture.hmonAssoc = outputDesc.Monitor;
            sharedTexture.hDxSharedTexture = sharedHandle;
            sharedTexture.Flags = 0;

            sharedTextures[texture] = sharedTexture;
        }
    }

    return sharedTextures;
}