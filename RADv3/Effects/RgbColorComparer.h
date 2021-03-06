//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Defines an effect that checks the color distance values and returns true
//      if any color distance is greater than the JND.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

class RgbColorComparer : public ID2D1EffectImpl, public ID2D1ComputeTransform, public ID2D1AnalysisTransform
{
// Constructor
public:
    RgbColorComparer(void);


// Destructor
public:
    virtual ~RgbColorComparer(void);


// IUnknown Methods
public:
    ULONG STDMETHODCALLTYPE AddRef(void);

    ULONG STDMETHODCALLTYPE Release(void);

    HRESULT STDMETHODCALLTYPE QueryInterface(
        __in REFIID riid,
        __deref_opt_out LPVOID* ppvObj);


// ID2D1EffectImpl Methods
public:
    HRESULT STDMETHODCALLTYPE Initialize(
        ID2D1EffectContext* argEffectContext,
        ID2D1TransformGraph* argTransformGraph);

    HRESULT STDMETHODCALLTYPE PrepareForRender(
        D2D1_CHANGE_TYPE argChangeType);

    IFACEMETHODIMP SetGraph(
        __in ID2D1TransformGraph* pTransformGraph);


// ID2D1TransformNode Methods
public:
    IFACEMETHODIMP_(UINT32) GetInputCount(void) const;


// ID2D1AnalysisTransform
public:
    HRESULT STDMETHODCALLTYPE ProcessAnalysisResults(
        _In_reads_(analysisDataCount) CONST BYTE* analysisData,
        UINT analysisDataCount);


// ID2D1ComputeTransform Methods
public:
    HRESULT STDMETHODCALLTYPE SetComputeInfo(
        ID2D1ComputeInfo* argComputeInfo);

    HRESULT STDMETHODCALLTYPE CalculateThreadgroups(
        _In_ CONST D2D1_RECT_L* outputRect,
        _Out_ UINT32* dimensionX,
        _Out_ UINT32* dimensionY,
        _Out_ UINT32* dimensionZ);


// ID2D1Transform Methods
public:
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


// Input Properties
public:
    HRESULT SetRed(FLOAT argRed);

    FLOAT GetRed(void) const;

    HRESULT SetGreen(FLOAT argGreen);

    FLOAT GetGreen(void) const;

    HRESULT SetBlue(FLOAT argBlue);

    FLOAT GetBlue(void) const;


// Analysis Properties
public:
    UINT32 GetPixelMatchCount(void) const;

    UINT32 GetPixelMatchFirstX(void) const;

    UINT32 GetPixelMatchFirstY(void) const;

    UINT32 GetPixelMismatchCount(void) const;

    UINT32 GetPixelMismatchFirstX(void) const;

    UINT32 GetPixelMismatchFirstY(void) const;


// Static Methods
public:
    static HRESULT STDMETHODCALLTYPE Create(__deref_out IUnknown** ppEffectImpl);

    static HRESULT Register(ID2D1Factory1* argFactory);


// Private Data Members
private:
    ID2D1ComputeInfo* mComputeInfo;

    FLOAT             mRed;
    FLOAT             mGreen;
    FLOAT             mBlue;

    UINT32            mPixelMatchCount;
    UINT32            mPixelMatchFirstX;
    UINT32            mPixelMatchFirstY;
    UINT32            mPixelMismatchCount;
    UINT32            mPixelMismatchFirstX;
    UINT32            mPixelMismatchFirstY;

    D2D1_RECT_L       mInputRect;

    LONG              mRefCount;

};