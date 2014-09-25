//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "ColorDistanceToRgb.h"
#include "ColorDistanceToRgbShader.h"

const GUID GUID_ColorDistanceToRgb = { 0x6b07d8ad, 0xdd10, 0x4f15, 0x92, 0xc0, 0x81, 0x41, 0x0e, 0xdb, 0x6a, 0x84 };

//
// Constructor
//

ColorDistanceToRgb::ColorDistanceToRgb(void)
    : mRefCount(0), mDrawInfo(nullptr)
{
}


//
// Destructor
//

ColorDistanceToRgb::~ColorDistanceToRgb(void)
{
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE ColorDistanceToRgb::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE ColorDistanceToRgb::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE ColorDistanceToRgb::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE ColorDistanceToRgb::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_ColorDistanceToRgb, g_ColorDistanceToRgb, sizeof(g_ColorDistanceToRgb));
    this->mDrawInfo->SetPixelShader(GUID_ColorDistanceToRgb);

    return hResult;
}


HRESULT STDMETHODCALLTYPE ColorDistanceToRgb::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP ColorDistanceToRgb::SetGraph(
    __in ID2D1TransformGraph *pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) ColorDistanceToRgb::GetInputCount(void) const
{
    return 1;
}


HRESULT STDMETHODCALLTYPE ColorDistanceToRgb::SetDrawInfo(
    ID2D1DrawInfo* argDrawInfo)
{
    this->mDrawInfo = argDrawInfo;
    this->mDrawInfo->AddRef();

    return S_OK;
}


IFACEMETHODIMP ColorDistanceToRgb::MapOutputRectToInputRects(
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


IFACEMETHODIMP ColorDistanceToRgb::MapInputRectsToOutputRect(
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* /*pInputOpaqueSubRects*/,
    UINT32 cTransformInputs,
    __out D2D1_RECT_L *pOutputRect,
    __out D2D1_RECT_L *pOutputOpaqueSubRect
    )
{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    *pOutputRect = *pInputRects;
    ZeroMemory(pOutputOpaqueSubRect, sizeof(*pOutputOpaqueSubRect));

    return S_OK;
}


IFACEMETHODIMP ColorDistanceToRgb::MapInvalidRect(
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

HRESULT ColorDistanceToRgb::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new ColorDistanceToRgb());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT ColorDistanceToRgb::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='ColorDistanceToRgb'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Take a color distance image and produces an RGB image.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>
        </Effect>
    );

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADColorDistanceToRgb,
        pszXml,
        nullptr,
        0,
        &ColorDistanceToRgb::Create));

    return hResult;
}
