//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "Invert.h"
#include "InvertShader.h"

const GUID GUID_Invert = { 0x1040cd61, 0x2ace, 0x4b99, 0x89, 0xfd, 0x9b, 0xf0, 0x4e, 0x88, 0x4b, 0x6e };

//
// Constructor
//
Invert::Invert(void)
    : mDrawInfo(nullptr), mRefCount(0)
{
}


//
// Destructor
//
Invert::~Invert(void)
{
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE Invert::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE Invert::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE Invert::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE Invert::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_Invert, g_Invert, sizeof(g_Invert));
    this->mDrawInfo->SetPixelShader(GUID_Invert);

    return hResult;
}


HRESULT STDMETHODCALLTYPE Invert::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP Invert::SetGraph(
    __in ID2D1TransformGraph *pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) Invert::GetInputCount(void) const
{
    return 1;
}


HRESULT STDMETHODCALLTYPE Invert::SetDrawInfo(
    ID2D1DrawInfo* argDrawInfo)
{
    this->mDrawInfo = argDrawInfo;
    this->mDrawInfo->AddRef();

    return S_OK;
}


IFACEMETHODIMP Invert::MapOutputRectToInputRects(
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


IFACEMETHODIMP Invert::MapInputRectsToOutputRect(
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
    ZeroMemory(pOutputOpaqueSubRect, sizeof(*pOutputOpaqueSubRect));

    return S_OK;
}

IFACEMETHODIMP Invert::MapInvalidRect(
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

HRESULT Invert::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new Invert());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT Invert::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='Invert'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Applies color invert.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>
        </Effect>
    );

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADV2Invert,
        pszXml,
        nullptr,
        0,
        &Invert::Create));

    return hResult;
}
