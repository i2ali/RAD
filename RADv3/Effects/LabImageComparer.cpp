//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "LabImageComparer.h"
#include "LabImageComparerShader.h"

const GUID GUID_LabImageComparer = { 0x65c3308c, 0x2bab, 0x4088, 0xbb, 0x3c, 0xd4, 0xbc, 0x04, 0xb9, 0xe8, 0x4d };

//
// Constructor
//

LabImageComparer::LabImageComparer(void)
    : mDrawInfo(nullptr), mRefCount(0)
{
}


//
// Destructor
//

LabImageComparer::~LabImageComparer(void)
{
    SafeRelease(&this->mDrawInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE LabImageComparer::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE LabImageComparer::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE LabImageComparer::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE LabImageComparer::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;
    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));

    argEffectContext->LoadPixelShader(GUID_LabImageComparer, g_LabImageComparer, sizeof(g_LabImageComparer));
    this->mDrawInfo->SetPixelShader(GUID_LabImageComparer);

    return hResult;
}


HRESULT STDMETHODCALLTYPE LabImageComparer::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP LabImageComparer::SetGraph(
    __in ID2D1TransformGraph *pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) LabImageComparer::GetInputCount(void) const
{
    return 2;
}


//
// ID2D1DrawTransform Methods
//

HRESULT STDMETHODCALLTYPE LabImageComparer::SetDrawInfo(
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

IFACEMETHODIMP LabImageComparer::MapOutputRectToInputRects(
    __in const D2D1_RECT_L* pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
    UINT32 cTransformInputs
    ) const
{
    if (cTransformInputs < 2)
    {
        return E_INVALIDARG;
    }

    pInputRects[0] = *pOutputRect;
    pInputRects[1] = *pOutputRect;

    return S_OK;
}

IFACEMETHODIMP LabImageComparer::MapInputRectsToOutputRect(
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* /*pInputOpaqueSubRects*/,
    UINT32 cTransformInputs,
    __out D2D1_RECT_L* pOutputRect,
    __out D2D1_RECT_L* pOutputOpaqueSubRect
    )
{
    if (cTransformInputs < 2)
    {
        return E_INVALIDARG;
    }

    D2D1_RECT_L output = *pInputRects;

    for (UINT32 i = 1; i < cTransformInputs; i++)
    {
        // Ignore the return value... We don't care whether the output is empty.
        UnionRect(&output, &output, &pInputRects[i]);
    }

    *pOutputRect = output;
    *pOutputOpaqueSubRect = D2D1::RectL(0, 0, 0, 0);

    return S_OK;
}


IFACEMETHODIMP LabImageComparer::MapInvalidRect(
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
// Static methods
//

HRESULT LabImageComparer::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new LabImageComparer());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT LabImageComparer::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect>
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='LabImageComparer'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Filter'/>
            <Property name='Description' type='string' value='Calculates the distance between each texel of two LAB images.'/>
            <Inputs>
                <Input name='Left'/>
                <Input name='Right'/>
            </Inputs>
        </Effect>
    );

    HRESULT hResult = S_OK;

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADLabImageComparer,
        pszXml,
        nullptr,
        0,
        &LabImageComparer::Create));

    return hResult;
}
