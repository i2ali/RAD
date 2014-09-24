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

namespace RADv2
{
    void release_iunknown_deleter(IUnknown* unknown);
}
