#pragma once

#ifndef UNICODE
#define UNICODE
#endif

// WIC Headers
#include <wincodec.h>

// ATL Headers
#include <atlbase.h>

// STL Headers
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

// DirectX 11 Headers
#include <d3d11.h>
#include <d2d1_1.h>

// User Private Header
#include <winuserp.h>

namespace RAD
{
    void release_iunknown_deleter(IUnknown* unknown);
}

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