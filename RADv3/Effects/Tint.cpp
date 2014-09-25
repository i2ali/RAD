//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "Tint.h"
#include "TintShader.h"

const GUID GUID_Tint = { 0xe2df7554, 0xa429, 0x49fe, 0x91, 0x6b, 0xbd, 0x5c, 0x61, 0x7d, 0x86, 0x1c };

//
// Constructor
//
Tint::Tint(void)
    : mDrawInfo(nullptr),
      mTintRed(0.0f), mTintGreen(0.0f), mTintBlue(0.0f), mTintAlpha(0.0f),
      mRefCount(0)
{
}


//
// Destructor
//
Tint::~Tint(void)
{
    //SafeRelease(&mEffectContext);
    //SafeRelease(&mTransformGraph);
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE Tint::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE Tint::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE Tint::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE Tint::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    // Uncomment if the context and transform graph need to be saved:

    //mEffectContext = argEffectContext;
    //mEffectContext->AddRef();

    //mTransformGraph = argTransformGraph;
    //mTransformGraph->AddRef();

    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_Tint, g_Tint, sizeof(g_Tint));
    this->mDrawInfo->SetPixelShader(GUID_Tint);

    return hResult;
}


HRESULT STDMETHODCALLTYPE Tint::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    HRESULT hResult;

    float params[] = 
    {
        mTintRed,
        mTintGreen,
        mTintBlue,
        mTintAlpha,
    };

    IFR(this->mDrawInfo->SetPixelShaderConstantBuffer(reinterpret_cast<const BYTE*>(params), sizeof(params)));

    return S_OK;
}


IFACEMETHODIMP Tint::SetGraph(
    __in ID2D1TransformGraph *pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//
IFACEMETHODIMP_(UINT32) Tint::GetInputCount(void) const
{
    return 1;
}


HRESULT STDMETHODCALLTYPE Tint::SetDrawInfo(
    ID2D1DrawInfo* argDrawInfo)
{
    this->mDrawInfo = argDrawInfo;
    this->mDrawInfo->AddRef();

    return S_OK;
}


IFACEMETHODIMP Tint::MapOutputRectToInputRects(
    __in const D2D1_RECT_L *pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L *pInputRects,
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


IFACEMETHODIMP Tint::MapInputRectsToOutputRect(
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

IFACEMETHODIMP Tint::MapInvalidRect(
    UINT32 inputIndex,
    D2D1_RECT_L invalidInputRect,
    __out D2D1_RECT_L *pInvalidOutputRect 
    ) const
{
    HRESULT hr = S_OK;

    *pInvalidOutputRect = invalidInputRect;

    return hr;
}

//
// Properties
//

HRESULT Tint::SetTintRed(FLOAT argRed)
{
    this->mTintRed = argRed;
    return S_OK;
}


FLOAT Tint::GetTintRed(void) const
{
    return this->mTintRed;
}


HRESULT Tint::SetTintGreen(FLOAT argGreen)
{
    this->mTintGreen = argGreen;
    return S_OK;
}


FLOAT Tint::GetTintGreen(void) const
{
    return this->mTintGreen;
}


HRESULT Tint::SetTintBlue(FLOAT argBlue)
{
    this->mTintBlue = argBlue;
    return S_OK;
}


FLOAT Tint::GetTintBlue(void) const
{
    return this->mTintBlue;
}


HRESULT Tint::SetTintAlpha(FLOAT argAlpha)
{
    this->mTintAlpha = argAlpha;
    return S_OK;
}


FLOAT Tint::GetTintAlpha(void) const
{
    return this->mTintAlpha;
}


//
// Static methods
//

HRESULT Tint::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new Tint());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT Tint::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='Tint'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Applies color tint.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>

            <!-- Custom Properties -->
            <Property name='Red' type='float'>
                <Property name='DisplayName' type='string' value='Red'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
            <Property name='Green' type='float'>
                <Property name='DisplayName' type='string' value='Green'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
            <Property name='Blue' type='float'>
                <Property name='DisplayName' type='string' value='Blue'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
            <Property name='Alpha' type='float'>
                <Property name='DisplayName' type='string' value='Alpha'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
        </Effect>
    );

    static const D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_VALUE_TYPE_BINDING(L"Red",   &SetTintRed,   &GetTintRed),
        D2D1_VALUE_TYPE_BINDING(L"Green", &SetTintGreen, &GetTintGreen),
        D2D1_VALUE_TYPE_BINDING(L"Blue",  &SetTintBlue,  &GetTintBlue),
        D2D1_VALUE_TYPE_BINDING(L"Alpha", &SetTintAlpha, &GetTintAlpha),
    };

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADTint,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        &Tint::Create));

    return hResult;
}
