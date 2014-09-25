//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DebugImageData.h"
#include "DebugImageDataShader.h"

const GUID GUID_DebugImageData = { 0xfb7c827e, 0x9d52, 0x45c4, 0xbd, 0xb0, 0x1e, 0x99, 0xd8, 0x9f, 0x3c, 0xf2 };


//
// Constructor
//

DebugImageData::DebugImageData(void)
    : mComputeInfo(nullptr), mRefCount(0), mData(nullptr), mDataLength(0)
{
}


//
// Destructor
//

DebugImageData::~DebugImageData(void)
{
    SafeRelease(&this->mComputeInfo);

    if (nullptr != this->mData)
    {
        delete[] this->mData;
        this->mData = nullptr;
    }
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE DebugImageData::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE DebugImageData::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE DebugImageData::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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
    else if (riid == __uuidof(ID2D1ComputeTransform))
    {
        *ppvObj = static_cast<ID2D1ComputeTransform*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ID2D1AnalysisTransform))
    {
        *ppvObj = static_cast<ID2D1AnalysisTransform*>(this);
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


HRESULT STDMETHODCALLTYPE DebugImageData::Initialize(
    ID2D1EffectContext* argEffectContext,
    ID2D1TransformGraph* argTransformGraph)
{
    HRESULT hResult = S_OK;

    // Check for compute shader support
    D2D1_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hardwareOptions;
    IFR(argEffectContext->CheckFeatureSupport(D2D1_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hardwareOptions, sizeof(hardwareOptions)));
    if (!hardwareOptions.computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
    {
        return D2DERR_INSUFFICIENT_DEVICE_CAPABILITIES;
    }

    IFR(argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));
    IFR(argEffectContext->LoadComputeShader(GUID_DebugImageData, g_DebugImageData, sizeof(g_DebugImageData)));
    IFR(this->mComputeInfo->SetComputeShader(GUID_DebugImageData));

    return hResult;
}


HRESULT STDMETHODCALLTYPE DebugImageData::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP DebugImageData::SetGraph(
    __in ID2D1TransformGraph* pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) DebugImageData::GetInputCount(void) const
{
    return 1;
}


//
// ID2D1AnalysisTransform
//

HRESULT STDMETHODCALLTYPE DebugImageData::ProcessAnalysisResults(
    _In_reads_(analysisDataCount) CONST BYTE* analysisData,
    UINT analysisDataCount)
{
    if (analysisDataCount > 0)
    {
        if (nullptr != this->mData)
        {
            delete[] this->mData;
            this->mData = nullptr;
        }

        this->mDataLength = analysisDataCount;
        this->mData = new BYTE[analysisDataCount];
        ::memcpy(this->mData, analysisData, analysisDataCount);
        return NOERROR;
    }

    return E_INVALIDARG;
}


//
// ID2D1ComputeTransform Methods
//

HRESULT STDMETHODCALLTYPE DebugImageData::SetComputeInfo(
    ID2D1ComputeInfo* argComputeInfo)
{
    this->mComputeInfo = argComputeInfo;
    this->mComputeInfo->AddRef();

    // We want to output 32bpc instead of 8bpc
    //this->mComputeInfo->SetOutputBuffer(D2D1_BUFFER_PRECISION_32BPC_FLOAT, D2D1_CHANNEL_DEPTH_4);

    return S_OK;
}


HRESULT STDMETHODCALLTYPE DebugImageData::CalculateThreadgroups(
    _In_ CONST D2D1_RECT_L* outputRect,
    _Out_ UINT32* dimensionX,
    _Out_ UINT32* dimensionY,
    _Out_ UINT32* dimensionZ)
{
    *dimensionX = 1;
    *dimensionY = 1;
    *dimensionZ = 1;

    return S_OK;
}


//
// ID2D1Transform Methods
//

IFACEMETHODIMP DebugImageData::MapOutputRectToInputRects(
    __in const D2D1_RECT_L* pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
    UINT32 cTransformInputs) const
{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    *pInputRects = *pOutputRect;

    return S_OK;
}


IFACEMETHODIMP DebugImageData::MapInputRectsToOutputRect(
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* /*pInputOpaqueSubRects*/,
    UINT32 cTransformInputs,
    __out D2D1_RECT_L* pOutputRect,
    __out D2D1_RECT_L* pOutputOpaqueSubRect)

{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    D2D1_RECT_L inputRect = pInputRects[0];

    UINT32 constants[] = { inputRect.left, inputRect.top, inputRect.right, inputRect.bottom };
    this->mComputeInfo->SetComputeShaderConstantBuffer(reinterpret_cast<const BYTE *>(constants), sizeof(constants));

    *pOutputRect = *pInputRects;
    *pOutputOpaqueSubRect = D2D1::RectL(0, 0, 0, 0);

    return S_OK;
}


IFACEMETHODIMP DebugImageData::MapInvalidRect(
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
// Properties
//

BYTE* DebugImageData::GetImageData(void) const
{
    return this->mData;
}


UINT32 DebugImageData::GetImageDataLength(void) const
{
    return this->mDataLength;
}


//
// Static methods
//

HRESULT DebugImageData::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new DebugImageData());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT DebugImageData::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect type="Analysis">
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='DebugImageData'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Analysis'/>
            <Property name='Description' type='string' value='Gets texture data.'/>
            <Inputs>
                <Input name='Source'/>
            </Inputs>

            <!-- Custom Properties -->
            <Property name="ImageData" type="blob">
                <Property name="DisplayName" type="string" value="Image Data" />
            </Property>

            <Property name="ImageDataLength" type="uint32">
                <Property name="DisplayName" type="string" value="Iamge Data Length" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

        </Effect>
    );

    HRESULT hResult = S_OK;

    static const D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_READONLY_VALUE_TYPE_BINDING(L"ImageData", &GetImageData),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"ImageDataLength", &GetImageDataLength),
    };

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADDebugImageData,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        &DebugImageData::Create));

    return hResult;
}
