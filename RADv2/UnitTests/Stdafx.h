//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#pragma once

// Windows
#include <windows.h>
#include <winuserp.h>

// Dwm
#include <dwmapi.h>

// DirectX
#include <d3d11.h>

// WIC
#include <wincodec.h>

// TAEF Headers
#include <WexTestClass.h>
#include <TestData.h>

// RADv2
#include <SlicerInterface.h>
#include <RADv2Effects.h>

// STL
#include <vector>
#include <map>

// ATL
#include <atlbase.h>

// Utilities
#include <LabColor.h>

MODULE_SETUP(ModuleSetup);

MODULE_CLEANUP(ModuleCleanup);

std::vector<std::shared_ptr<IDXGIAdapter>> GetAdapters(void);

std::map<std::shared_ptr<IDXGIAdapter>, std::shared_ptr<ID3D11Device>> GetDevices(void);

D3D_FEATURE_LEVEL GetFeatureLevelOfAdapter(std::shared_ptr<IDXGIAdapter> argAdapter);

std::shared_ptr<IDXGIAdapter> GetPrimaryAdapter(void);

std::shared_ptr<ID3D11Device> GetPrimaryDevice(void);

std::shared_ptr<IDXGIOutput> GetPrimaryOutput(void);

std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>> GetOutputsFromScreenRect(RECT argScreenRect);

std::map<std::shared_ptr<ID3D11Texture2D>, SLICER_SHARED_TEXTURE> CreateTexturesForOutputs(
    const std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>>& argOutputs,
    UINT argTextureWidth,
    UINT argTextureHeight);

void release_iunknown_deleter(IUnknown* unknown);
