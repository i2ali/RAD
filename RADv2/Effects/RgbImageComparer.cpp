//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "RgbImageComparer.h"
#include "RgbImageComparerShader.h"

const GUID GUID_RgbImageComparer = { 0x8f929e30, 0xac8b, 0x4671, 0x98, 0x74, 0x82, 0xb4, 0xf0, 0x99, 0x90, 0x75 };


//
// Constructor
//

RgbImageComparer::RgbImageComparer(void)
    : mComputeInfo(nullptr), mRefCount(0),
      mPixelMatchCount(0), mPixelMatchFirstX(0), mPixelMatchFirstY(0),
      mPixelMismatchCount(0), mPixelMismatchFirstX(0), mPixelMismatchFirstY(0)
{
}


//
// Destructor
//

RgbImageComparer::~RgbImageComparer(void)
{
    SafeRelease(&this->mComputeInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE RgbImageComparer::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE RgbImageComparer::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE RgbImageComparer::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE RgbImageComparer::Initialize(
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

    IFR(hResult = argTransformGraph->SetSingleTransformNode(static_cast<ID2D1TransformNode*>(this)));
    IFR(hResult = argEffectContext->LoadComputeShader(GUID_RgbImageComparer, g_RgbImageComparer, sizeof(g_RgbImageComparer)));
    IFR(hResult = this->mComputeInfo->SetComputeShader(GUID_RgbImageComparer));

    return hResult;
}


HRESULT STDMETHODCALLTYPE RgbImageComparer::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP RgbImageComparer::SetGraph(
    __in ID2D1TransformGraph* pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) RgbImageComparer::GetInputCount(void) const
{
    return 2;
}


//
// ID2D1AnalysisTransform
//

HRESULT STDMETHODCALLTYPE RgbImageComparer::ProcessAnalysisResults(
    _In_reads_(analysisDataCount) CONST BYTE* analysisData,
    UINT analysisDataCount)
{
    static const UINT32 bytesPerChanel = sizeof(UINT32);
    static const UINT32 chanelsPerTexel = 4;
    static const UINT32 texels = 2;
    static const UINT32 bufferSize = texels * chanelsPerTexel * bytesPerChanel;

    if (analysisDataCount != bufferSize)
    {
        return E_INVALIDARG;
    }

    const UINT32* data = reinterpret_cast<const UINT32*>(analysisData);
    this->mPixelMatchCount = data[0];
    this->mPixelMatchFirstX = data[1];
    this->mPixelMatchFirstY = data[2];
    this->mPixelMismatchCount = data[3];
    this->mPixelMismatchFirstX = data[4];
    this->mPixelMismatchFirstY = data[5];

    return S_OK;
}


//
// ID2D1ComputeTransform Methods
//

HRESULT STDMETHODCALLTYPE RgbImageComparer::SetComputeInfo(
    ID2D1ComputeInfo* argComputeInfo)
{
    this->mComputeInfo = argComputeInfo;
    this->mComputeInfo->AddRef();

    // We want to output 32bpc instead of 8bpc
    this->mComputeInfo->SetOutputBuffer(D2D1_BUFFER_PRECISION_32BPC_FLOAT, D2D1_CHANNEL_DEPTH_4);

    return S_OK;
}


HRESULT STDMETHODCALLTYPE RgbImageComparer::CalculateThreadgroups(
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

IFACEMETHODIMP RgbImageComparer::MapOutputRectToInputRects(
    __in const D2D1_RECT_L* pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
    UINT32 cTransformInputs) const
{
    if (cTransformInputs < 2)
    {
        return E_INVALIDARG;
    }

    pInputRects[0] = this->mInputRect;
    pInputRects[1] = this->mInputRect;

    return S_OK;
}


IFACEMETHODIMP RgbImageComparer::MapInputRectsToOutputRect(
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* pInputRects,
    __in_ecount_opt(cTransformInputs) const D2D1_RECT_L* /*pInputOpaqueSubRects*/,
    UINT32 cTransformInputs,
    __out D2D1_RECT_L* pOutputRect,
    __out D2D1_RECT_L* pOutputOpaqueSubRect)

{
    if (cTransformInputs < 2)
    {
        return E_INVALIDARG;
    }

    D2D1_RECT_L inputRect = pInputRects[0];

    for (UINT32 i = 1; i < cTransformInputs; ++i)
    {
        // Ignore the return value... We don't care whether the output is empty.
        UnionRect(&inputRect, &inputRect, &pInputRects[i]);
    }

    this->mInputRect = inputRect;

    UINT32 constants[] = { inputRect.left, inputRect.top, inputRect.right, inputRect.bottom };
    this->mComputeInfo->SetComputeShaderConstantBuffer(reinterpret_cast<const BYTE*>(constants), sizeof(constants));

    pOutputRect->left = 0;
    pOutputRect->top = 0;
    pOutputRect->bottom = 1;
    pOutputRect->right = 2; // 2 texels * 4 bytes per chanel * 4 chanels == 32 bytes of buffer

    *pOutputOpaqueSubRect = D2D1::RectL(0, 0, 0, 0);

    return S_OK;
}


IFACEMETHODIMP RgbImageComparer::MapInvalidRect(
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

UINT32 RgbImageComparer::GetPixelMatchCount(void) const
{
    return this->mPixelMatchCount;
}


UINT32 RgbImageComparer::GetPixelMatchFirstX(void) const
{
    return this->mPixelMatchFirstX;
}


UINT32 RgbImageComparer::GetPixelMatchFirstY(void) const
{
    return this->mPixelMatchFirstY;
}


UINT32 RgbImageComparer::GetPixelMismatchCount(void) const
{
    return this->mPixelMismatchCount;
}


UINT32 RgbImageComparer::GetPixelMismatchFirstX(void) const
{
    return this->mPixelMismatchFirstX;
}


UINT32 RgbImageComparer::GetPixelMismatchFirstY(void) const
{
    return this->mPixelMismatchFirstY;
}


//
// Static methods
//

HRESULT RgbImageComparer::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new RgbImageComparer());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT RgbImageComparer::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect type="Analysis">
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='RgbImageComparer'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Analysis'/>
            <Property name='Description' type='string' value='Checks a color distance image for noticeably different values.'/>
            <Inputs>
                <Input name='Left'/>
                <Input name='Right'/>
            </Inputs>

            <!-- Custom Properties -->
            <Property name="PixelMatchCount" type="uint32">
                <Property name="DisplayName" type="string" value="Matched Pixel Count" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

            <Property name="PixelMatchFirstX" type="uint32">
                <Property name="DisplayName" type="string" value="First Matched Pixel X" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

            <Property name="PixelMatchFirstY" type="uint32">
                <Property name="DisplayName" type="string" value="First Matched Pixel Y" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

            <Property name="PixelMismatchCount" type="uint32">
                <Property name="DisplayName" type="string" value="Mismatched Pixel Count" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

            <Property name="PixelMismatchFirstX" type="uint32">
                <Property name="DisplayName" type="string" value="First Mismatched Pixel X" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

            <Property name="PixelMismatchFirstY" type="uint32">
                <Property name="DisplayName" type="string" value="First Mismatched Pixel Y" />
                <Property name="Min" type="uint32" value="0" />
                <Property name="Max" type="uint32" value="4294967295" />
                <Property name="Default" type="uint32" value="0" />
            </Property>

        </Effect>
    );

    HRESULT hResult = S_OK;

    static const D2D1_PROPERTY_BINDING bindings[] =
    {
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMatchCount", &GetPixelMatchCount),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMatchFirstX", &GetPixelMatchFirstX),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMatchFirstY", &GetPixelMatchFirstY),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMismatchCount", &GetPixelMismatchCount),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMismatchFirstX", &GetPixelMismatchFirstX),
        D2D1_READONLY_VALUE_TYPE_BINDING(L"PixelMismatchFirstY", &GetPixelMismatchFirstY),
    };

    IFR(argFactory->RegisterEffectFromString(
        CLSID_RADV2RgbImageComparer,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        &RgbImageComparer::Create));

    return hResult;
}
