//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "CheckColorDistance.h"
#include "CheckColorDistanceShader.h"

const GUID GUID_CheckColorDistance = { 0x3e89c2cb, 0xd677 ,0x4c63, 0x9e, 0xc4, 0x20, 0x55, 0x6d, 0x21, 0xa7, 0xc1 };

//
// Constructor
//

CheckColorDistance::CheckColorDistance(void)
    : mComputeInfo(nullptr), mRefCount(0),
      mPixelMatchCount(0), mPixelMatchFirstX(0), mPixelMatchFirstY(0),
      mPixelMismatchCount(0), mPixelMismatchFirstX(0), mPixelMismatchFirstY(0)
{
}


//
// Destructor
//

CheckColorDistance::~CheckColorDistance(void)
{
    SafeRelease(&this->mComputeInfo);
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE CheckColorDistance::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);

    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE CheckColorDistance::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE CheckColorDistance::QueryInterface(__in REFIID riid, __deref_opt_out LPVOID* ppvObj)
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


HRESULT STDMETHODCALLTYPE CheckColorDistance::Initialize(
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
    IFR(argEffectContext->LoadComputeShader(GUID_CheckColorDistance, g_CheckColorDistance, sizeof(g_CheckColorDistance)));
    IFR(this->mComputeInfo->SetComputeShader(GUID_CheckColorDistance));

    return hResult;
}


HRESULT STDMETHODCALLTYPE CheckColorDistance::PrepareForRender(
    D2D1_CHANGE_TYPE argChangeType)
{
    return S_OK;
}


IFACEMETHODIMP CheckColorDistance::SetGraph(
    __in ID2D1TransformGraph* pTransformGraph)
{
    // No need to implement this on effects without variable input counts
    return E_NOTIMPL;
}


//
// ID2D1TransformNode Methods
//

IFACEMETHODIMP_(UINT32) CheckColorDistance::GetInputCount(void) const
{
    return 1;
}


//
// ID2D1AnalysisTransform
//

HRESULT STDMETHODCALLTYPE CheckColorDistance::ProcessAnalysisResults(
    _In_reads_(analysisDataCount) CONST BYTE* analysisData,
    UINT analysisDataCount)
{
    if (analysisDataCount != this->mBufferSize)
    {
        return E_INVALIDARG;
    }

    UINT32 width = this->mInputRect.right - this->mInputRect.left;
    UINT32 height = this->mInputRect.bottom - this->mInputRect.top;

    UINT* data = (UINT*)analysisData;

    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    for (UINT y = 0; y < height; ++y)
    {
        for (UINT x = 0; x < width; ++x)
        {
            UINT32 index = (y * width) + x;

            if (data[index])
            {
                if (0 == pixelMatchCount)
                {
                    pixelMatchFirstX = x;
                    pixelMatchFirstY = y;
                }

                pixelMatchCount++;
            }
            else
            {
                if (0 == pixelMismatchCount)
                {
                    pixelMismatchFirstX = x;
                    pixelMismatchFirstY = y;
                }

                pixelMismatchCount++;
            }
        }
    }

    this->mPixelMatchCount = pixelMatchCount;
    this->mPixelMatchFirstX = pixelMatchFirstX;
    this->mPixelMatchFirstY = pixelMatchFirstY;
    this->mPixelMismatchCount = pixelMismatchCount;
    this->mPixelMismatchFirstX = pixelMismatchFirstX;
    this->mPixelMismatchFirstY = pixelMismatchFirstY;

    return S_OK;
}


//
// ID2D1ComputeTransform Methods
//

HRESULT STDMETHODCALLTYPE CheckColorDistance::SetComputeInfo(
    ID2D1ComputeInfo* argComputeInfo)
{
    this->mComputeInfo = argComputeInfo;
    this->mComputeInfo->AddRef();

    return this->mComputeInfo->SetOutputBuffer(D2D1_BUFFER_PRECISION_32BPC_FLOAT, D2D1_CHANNEL_DEPTH_4);
}


HRESULT STDMETHODCALLTYPE CheckColorDistance::CalculateThreadgroups(
    _In_ CONST D2D1_RECT_L* outputRect,
    _Out_ UINT32* dimensionX,
    _Out_ UINT32* dimensionY,
    _Out_ UINT32* dimensionZ)
{
    // Dispatch the same number of thread groups as the area of the input rect
    *dimensionX = this->mInputRect.right - this->mInputRect.left;
    *dimensionY = this->mInputRect.bottom - this->mInputRect.top;
    *dimensionZ = 1;

    return S_OK;
}


//
// ID2D1Transform Methods
//

IFACEMETHODIMP CheckColorDistance::MapOutputRectToInputRects(
    __in const D2D1_RECT_L* pOutputRect,
    __out_ecount(cTransformInputs) D2D1_RECT_L* pInputRects,
    UINT32 cTransformInputs) const
{
    if (cTransformInputs < 1)
    {
        return E_INVALIDARG;
    }

    *pInputRects = this->mInputRect;

    return S_OK;
}


IFACEMETHODIMP CheckColorDistance::MapInputRectsToOutputRect(
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

    D2D1_RECT_L inputRect = this->mInputRect = pInputRects[0];

    UINT32 width = inputRect.right - inputRect.left;
    UINT32 height = inputRect.bottom - inputRect.top;

    // Set the image stride on the shader, equal to the width of the image
    // being processed.
    this->mComputeInfo->SetComputeShaderConstantBuffer(reinterpret_cast<const BYTE *>(&width), sizeof(width));

    // Calculate the buffer size
    //
    // DImage will always allocate a view of float4 per element
    // However, the shader view is uint per element
    // To account for this, the buffer size will be divided by 4
    UINT bufferSize = width * height;

    // Check that the bufferSize is a multiple of 4
    if ((bufferSize & 0x3) > 0)
    {
        // This will round the bufferSize to the next multiple of 4
        bufferSize += 4;
    }

    // Divide the buffer by 4
    bufferSize = bufferSize >> 2;

    // Find the ceiling of the square root of the buffer size
    bufferSize = static_cast<UINT>(::ceil(::sqrt(static_cast<double>(bufferSize))));

    // Save the actual byte count of the buffer size (bufferSize^2 * numChannels * sizesof(UINT32))
    this->mBufferSize = bufferSize * bufferSize * 4 * sizeof(UINT32);

    // Creating a square buffer because there are limits on single dimension
    // sizes depending on the feature level of the hardware (i.e. some hardware
    // cannot create a buffer size 1 x 518400, where 518400 is 1920 * 1080 / 4)
    pOutputRect->left = 0;
    pOutputRect->top = 0;
    pOutputRect->bottom = bufferSize;
    pOutputRect->right = bufferSize;

    *pOutputOpaqueSubRect = D2D1::RectL(0, 0, 0, 0);

    return S_OK;
}


IFACEMETHODIMP CheckColorDistance::MapInvalidRect(
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

UINT32 CheckColorDistance::GetPixelMatchCount(void) const
{
    return this->mPixelMatchCount;
}


UINT32 CheckColorDistance::GetPixelMatchFirstX(void) const
{
    return this->mPixelMatchFirstX;
}


UINT32 CheckColorDistance::GetPixelMatchFirstY(void) const
{
    return this->mPixelMatchFirstY;
}


UINT32 CheckColorDistance::GetPixelMismatchCount(void) const
{
    return this->mPixelMismatchCount;
}


UINT32 CheckColorDistance::GetPixelMismatchFirstX(void) const
{
    return this->mPixelMismatchFirstX;
}


UINT32 CheckColorDistance::GetPixelMismatchFirstY(void) const
{
    return this->mPixelMismatchFirstY;
}


//
// Static methods
//

HRESULT CheckColorDistance::Create(__deref_out IUnknown** ppEffectImpl)
{
    *ppEffectImpl = static_cast<ID2D1EffectImpl*>(new CheckColorDistance());

    if (nullptr == *ppEffectImpl)
    {
        return E_OUTOFMEMORY;
    }

    (*ppEffectImpl)->AddRef();

    return S_OK;
}


HRESULT CheckColorDistance::Register(ID2D1Factory1* argFactory)
{
    IFRNULL(argFactory);

    static PCWSTR pszXml =
    XML(
        <Effect type="Analysis">
            <!-- System Properties -->
            <Property name='DisplayName' type='string' value='CheckColorDistance'/>
            <Property name='Author' type='string' value='Microsoft Corporation'/>
            <Property name='Category' type='string' value='Analysis'/>
            <Property name='Description' type='string' value='Checks a color distance image for noticeably different values.'/>
            <Inputs>
                <Input name='Source'/>
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
        CLSID_RADCheckColorDistance,
        pszXml,
        bindings,
        ARRAYSIZE(bindings),
        &CheckColorDistance::Create));

    return hResult;
}
