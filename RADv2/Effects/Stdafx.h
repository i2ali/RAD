#pragma once

#ifndef UNICODE
#define UNICODE
#endif

// ATL Headers
#include <atlbase.h>

// STL Headers
#include <map>
#include <sstream>
#include <string>
#include <vector>

// DirectX 11 Headers
#include <D2D1EffectAuthor.h>
#include <D2D1EffectHelpers.h>

// Out effects
#include "RADv2Effects.h"

template<typename Interface> inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

//----------------------------------------------------------------------------
// Adds a one long to another, clamping the result between LONG_MIN 
// and LONG_MAX.
//
inline long ClipAdd(long base, long valueToAdd)
{
    if (valueToAdd >= 0)
    {
        return ((base + valueToAdd) >= base) ? (base + valueToAdd) : LONG_MAX;
    }
    else
    {
        return ((base + valueToAdd) <= base) ? (base + valueToAdd) : LONG_MIN;
    }
}

//----------------------------------------------------------------------------
// Determines whether a rect is infinite (per D2D spec).
//
inline bool IsRectInfinite(const D2D1_RECT_L &rect)
{
    return  rect.left  == LONG_MIN && rect.top      == LONG_MIN &&
            rect.right == LONG_MAX && rect.bottom   == LONG_MAX;
}

//----------------------------------------------------------------------------
// Inflates one rect by a specified amount (per-side). Respects infinite rects, 
// and handles overflow by clipping the inflated rect to the valid range.
//
inline D2D1_RECT_L SafeInflateRect(const D2D1_RECT_L &rect, const D2D1_RECT_L &inflate)
{
    if (IsRectInfinite(rect))
    {
        return rect;
    }

    D2D1_RECT_L ret =
    {
        ClipAdd(rect.left,   inflate.left  ),
        ClipAdd(rect.top,    inflate.top   ),
        ClipAdd(rect.right,  inflate.right ),
        ClipAdd(rect.bottom, inflate.bottom),
    };
    
    // Ensure our final rect is valid (right > left, bottom > top):
    ret.right   = max(ret.right,    ret.left);
    ret.bottom  = max(ret.bottom,   ret.top );

    return ret;
}

//----------------------------------------------------------------------------
// Inflates one rect by a specified x and y amount. Respects infinite rects, 
// and handles overflow by clipping the inflated rect to the valid range.
//
inline D2D1_RECT_L SafeInflateRect(const D2D1_RECT_L &rect, long inflateX, long inflateY)
{
    const D2D1_RECT_L inflate = { -inflateX, -inflateY, inflateX, inflateY };

    return SafeInflateRect(rect, inflate);
}


//----------------------------------------------------------------------------
// Determines whether a rect is empty (per D2D spec).
//
template<class T>
inline bool IsRectEmpty(const T &rect)
{
    return (rect.left >= rect.right || rect.top >= rect.bottom);
}


//----------------------------------------------------------------------------
// Takes the union of two rects.
//
inline D2D1_RECT_L UnionRect(const D2D1_RECT_L &rect1, const D2D1_RECT_L &rect2)
{
    if (IsRectEmpty(rect1))
    {
        return rect2;
    }
    else if (IsRectEmpty(rect2))
    {
        return rect1;
    }

    D2D1_RECT_L rect;
    rect.left = min(rect1.left, rect2.left);
    rect.top = min(rect1.top, rect2.top);
    rect.right = max(rect1.right, rect2.right);
    rect.bottom = max(rect1.bottom, rect2.bottom);

    return rect;
}


#define IFC(expr) do { hResult = (expr); if (FAILED(hResult)) { goto Cleanup; } } while (0, 0)
#define IFR(expr) do { hResult = (expr); if (FAILED(hResult)) { return hResult; } } while (0, 0)
#define IFRNULL(expr) do { if (NULL == (expr)) { return E_INVALIDARG; } } while (0, 0)

// Helper macro for defining XML strings.
#define XML(X) L"<?xml version='1.0'?>"##L#X