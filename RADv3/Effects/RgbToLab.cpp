//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "RgbToLab.h"
#include "RgbToLabShader.h"

const GUID GUID_RgbToLab = { 0x9209b7f6, 0xf218, 0x4322, 0x99, 0xc9, 0x86, 0xb4, 0x70, 0xb2, 0xfa, 0x31 };

//
// Constructor
//

RgbToLab::RgbToLab(void)
    : mDrawInfo(nullptr), mRefCount(0)
{
}


//
// Destructor
//

RgbToLab::~RgbToLab(void)
{
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE RgbToLab::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE RgbToLab::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE RgbToLab::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
{
    // Always set out parameter to nullptr, validating it first.
    if (nullptr == ppvObj)
    {
        return E_INVALIDARG;
    }
    else if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<ID2D1EffectImpl*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ID2D1EffectImpl))
    {
        *ppvObj = static_cast<ID2D1EffectImpl*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ID2D1DrawTransform))
    {
        *ppvObj = static_cast<ID2D1DrawTransform*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ID2D1Transform))
    {
        *ppvObj = static_cast<ID2D1Transform*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ID2D1TransformNode))
    {
        *ppvObj = static_cast<ID2D1TransformNode*>(this);
        this->AddRef();
        return NOERROR;
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }
}


HRESULT STDMETHODCALLTYPE RgbToLab::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_RgbToLab, g_RgbToLab, sizeof(g_RgbToLab));
    this->mDrawInfo->SetPixelShader(GUID_RgbToLab);

    return hResult;
}


HRESULT STDMETHODCALLTYPE RgbToLab::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP RgbToLab::SetGraph(
    __in ID2D1TransformGraph* pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) RgbToLab::GetInputCount(void) const
{
    return 1;
}


//
// ID2D1DrawTransform Methods
//

HRESULT STDMETHODCALLTYPE RgbToLab::SetDrawInfo(
    ID2D1DrawInfo* argDrawInfo)
{
    this->mDrawInfo = argDrawInfo;
    this->mDrawInfo->AddRef();

    // We want to output 32bpc instead of 8bpc
    this->mDrawInfo->SetOutputBuffer(D2D1_BUFFER_PRECISION_32BPC_FLOAT, D2D1_CHANNEL_DEPTH_4);

    return S_OK;
}


//
// ID2D1Transform Methods
//

IFACEMETHODIMP RgbToLab::MapOutputRectToInputRects(
    __in const D2D1_RECT_L* pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
    UINT32 cTransformInputs
    ) const
{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    *pInputRects = *pOutputRect;

    return S_OK;
}


IFACEMETHODIMP RgbToLab::MapInputRectsToOutputRect(
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* /*pInputOpaqueSubRects*/,
    UINT32 cTransformInputs,
    __out D2D1_RECT_L* pOutputRect,
    __out D2D1_RECT_L* pOutputOpaqueSubRect
    )
{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    *pOutputRect = *pInputRects;
    *pOutputOpaqueSubRect = D2D1::RectL(0, 0, 0, 0);

    return S_OK;
}


IFACEMETHODIMP RgbToLab::MapInvalidRect(
    UINT32 inputIndex,
    D2D1_RECT_L invalidInputRect,
    __out D2D1_RECT_L* pInvalidOutputRect
    ) const
{
    HRESULT hr = S_OK;

    *pInvalidOutputRect = invalidInputRect;

    return hr;
}


//
// Static methods
//

HRESULT RgbToLab::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new RgbToLab());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT RgbToLab::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='RgbToLab'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Converts from the RGB color space to the LAB color space.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>
        </Effect>
    );

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADRgbToLab,
        pszXml,
        nullptr,
        0,
        &RgbToLab::Create));

    return hResult;
}
