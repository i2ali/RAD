//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines an effect that compares two LAB formatted images.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

class LabImageComparer : public ID2D1EffectImpl, public ID2D1DrawTransform
{
// Constructor
public:
    LabImageComparer(void);


// Destructor
public:
    virtual ~LabImageComparer(void);


// IUnknown Methods
public:
    ULONG STDMETHODCALLTYPE AddRef(void);

    ULONG STDMETHODCALLTYPE Release(void);

    HRESULT STDMETHODCALLTYPE QueryInterface(
        __in REFIID riid,
        __deref_opt_out LPVOID* ppvObj);


// ID2D1EffectImpl Methods
public:
    //
    // Initialize the effect with a context and a transform graph. The effect must
    // populate the transform graph with a topology and can update it later.
    //
    HRESULT STDMETHODCALLTYPE Initialize(
        ID2D1EffectContext* argEffectContext,
        ID2D1TransformGraph* argTransformGraph);

    //
    // Initialize the effect with a context and a transform graph. The effect must
    // populate the transform graph with a topology and can update it later.
    //
    HRESULT STDMETHODCALLTYPE PrepareForRender(
        D2D1_CHANGE_TYPE argChangeType);

    IFACEMETHODIMP SetGraph(
        __in ID2D1TransformGraph* pTransformGraph);


// ID2D1TransformNode Methods
public:
    IFACEMETHODIMP_(UINT32) GetInputCount(void) const;


// ID2D1DrawTransform Methods
public:
    HRESULT STDMETHODCALLTYPE SetDrawInfo(
        ID2D1DrawInfo* argDrawInfo);

    IFACEMETHODIMP MapOutputRectToInputRects(
        __in const D2D1_RECT_L* pOutputRect,
        __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
        UINT32 cTransformInputs) const;

    IFACEMETHODIMP MapInputRectsToOutputRect(
        __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
        __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputOpaqueSubRects,
        UINT32 cTransformInputs,
        __out D2D1_RECT_L* pOutputRect,
        __out D2D1_RECT_L* pOutputOpaqueSubRect);

    IFACEMETHODIMP MapInvalidRect(
        UINT32 inputIndex,
        D2D1_RECT_L invalidInputRect,
        __out D2D1_RECT_L* pInvalidOutputRect) const;


// Static Methods
public:
    static HRESULT STDMETHODCALLTYPE Create(__deref_out IUnknown** ppEffectImpl);

    static HRESULT Register(ID2D1Factory1* argFactory);


// Private Data Members
private:
    ID2D1DrawInfo* mDrawInfo;

    LONG mRefCount;

};