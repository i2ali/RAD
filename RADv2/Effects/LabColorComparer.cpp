//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "LabColorComparer.h"
#include "LabColorComparerShader.h"

const GUID GUID_LabColorComparer = { 0x7cdb2cef, 0xa411 ,0x4a7f, 0xa0, 0xed, 0xcd, 0xe9, 0xc4, 0xd8, 0x49, 0xad };

//
// Constructor
//

LabColorComparer::LabColorComparer(void)
    : mDrawInfo(nullptr), mRefCount(0)
{
}


//
// Destructor
//

LabColorComparer::~LabColorComparer(void)
{
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE LabColorComparer::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE LabColorComparer::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE LabColorComparer::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE LabColorComparer::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_LabColorComparer, g_LabColorComparer, sizeof(g_LabColorComparer));
    this->mDrawInfo->SetPixelShader(GUID_LabColorComparer);

    return hResult;
}


HRESULT STDMETHODCALLTYPE LabColorComparer::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    HRESULT hResult;

    float params[] = 
    {
        this->mL,
        this->mA,
        this->mB,
        1.0f,
    };

    IFR(this->mDrawInfo->SetPixelShaderConstantBuffer(reinterpret_cast<const BYTE*>(params), sizeof(params)));

    return S_OK;
}


IFACEMETHODIMP LabColorComparer::SetGraph(
    __in ID2D1TransformGraph *pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) LabColorComparer::GetInputCount(void) const
{
    return 1;
}


//
// ID2D1DrawTransform Methods
//

HRESULT STDMETHODCALLTYPE LabColorComparer::SetDrawInfo(
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

IFACEMETHODIMP LabColorComparer::MapOutputRectToInputRects(
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

IFACEMETHODIMP LabColorComparer::MapInputRectsToOutputRect(
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


IFACEMETHODIMP LabColorComparer::MapInvalidRect(
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

HRESULT LabColorComparer::SetL(FLOAT argL)
{
    this->mL = argL;
    return S_OK;
}


FLOAT LabColorComparer::GetL(void) const
{
    return this->mL;
}


HRESULT LabColorComparer::SetA(FLOAT argA)
{
    this->mA = argA;
    return S_OK;
}


FLOAT LabColorComparer::GetA(void) const
{
    return this->mA;
}


HRESULT LabColorComparer::SetB(FLOAT argB)
{
    this->mB = argB;
    return S_OK;
}


FLOAT LabColorComparer::GetB(void) const
{
    return this->mB;
}


//
// Static methods
//

HRESULT LabColorComparer::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new LabColorComparer());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT LabColorComparer::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='LabColorComparer'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Calculates the distance between each texel of a LAB image and a color.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>

            <!-- Custom Properties -->
            <Property name='L' type='float'>
                <Property name='DisplayName' type='string' value='L Component'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
            <Property name='A' type='float'>
                <Property name='DisplayName' type='string' value='A Component'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
            <Property name='B' type='float'>
                <Property name='DisplayName' type='string' value='B Component'/>
                <Property name='Min' type='float' value='0.0'/>
                <Property name='Max' type='float' value='1.0'/>
                <Property name='Default' type='float' value='0.0'/>
            </Property>
        </Effect>
    );

    static const D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_VALUE_TYPE_BINDING(L"L", &SetL, &GetL),
        D2D1_VALUE_TYPE_BINDING(L"A", &SetA, &GetA),
        D2D1_VALUE_TYPE_BINDING(L"B", &SetB, &GetB),
    };

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADV2LabColorComparer,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        &LabColorComparer::Create));

    return hResult;
}
