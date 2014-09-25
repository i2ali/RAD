//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "SlicerFactory.h"
#include "DwmSlicer.h"
#include "GdiSlicer.h"

using namespace RAD;

//
// The function is declared in ISlicerInterface.h
//

HRESULT CreateSlicerFactory(RAD::ISlicerFactory** argSlicerFactory, RAD::ComputeShaderModel argComputeShaderModelRequired)
{
    return RAD::CreateSlicerFactory1(argSlicerFactory, argComputeShaderModelRequired);
}


//
// The function is declared as a friend in SlicerFactory.h
//

HRESULT RAD::CreateSlicerFactory1(RAD::ISlicerFactory** argSlicerFactory, RAD::ComputeShaderModel argComputeShaderModelRequired)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argSlicerFactory);

    // Set the output to nullptr
    *argSlicerFactory = nullptr;

    // Create a new slicer factory
    SlicerFactory* slicerFactory = new SlicerFactory(argComputeShaderModelRequired);
    IFCOOM(slicerFactory);

    // Initialize the slicer factory
    IFC(slicerFactory->Initialize());

    // Set the output to the new slicer factory and AddRef
    *argSlicerFactory = slicerFactory;
    slicerFactory->AddRef();


Cleanup:

    // Initialize failed, then delete the slicer factory
    if (FAILED(hResult))
    {
        if (nullptr != slicerFactory)
        {
            delete slicerFactory;
        }
    }

    return hResult;
}


//
// Constructor
//

SlicerFactory::SlicerFactory(RAD::ComputeShaderModel argComputeShaderModelRequired)
    : mComputeShaderModelRequired(argComputeShaderModelRequired),
      mD2DContext(nullptr),
      mRefCount(0)
{
}


HRESULT SlicerFactory::Initialize(void)
{
    HRESULT hResult;

    // Create a new D2DContext
    D2DContext* context = new D2DContext(this->mComputeShaderModelRequired);
    IFCOOM(context);

    // Initialize the D2DContext
    IFC(context->Initialize());

    // Assign and AddRef
    this->mD2DContext = context;
    context->AddRef();


Cleanup:

    if (FAILED(hResult))
    {
        if (nullptr != context)
        {
            delete context;
        }
    }

    return hResult;
}


//
// Destructor
//

SlicerFactory::~SlicerFactory(void)
{
    if (nullptr != this->mD2DContext)
    {
        this->mD2DContext->Release();
        this->mD2DContext = nullptr;
    }
}


//
// IUnknown Methods
//

ULONG STDMETHODCALLTYPE SlicerFactory::AddRef(void)
{
    ::InterlockedIncrement(&this->mRefCount);
    return this->mRefCount;
}


ULONG STDMETHODCALLTYPE SlicerFactory::Release(void)
{
    ULONG value = ::InterlockedDecrement(&this->mRefCount);

    if (0 == this->mRefCount)
    {
        delete this;
    }

    return value;
}


HRESULT STDMETHODCALLTYPE SlicerFactory::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
    if (nullptr == ppvObj)
    {
        return E_INVALIDARG;
    }
    else if (riid == IID_IUnknown)
    {
        *ppvObj = static_cast<IUnknown*>(this);
        this->AddRef();
        return NOERROR;
    }
    else if (riid == __uuidof(ISlicerFactory))
    {
        *ppvObj = static_cast<ISlicerFactory*>(this);
        this->AddRef();
        return NOERROR;
    }
    else
    {
        *ppvObj = nullptr;
        return E_NOINTERFACE;
    }
}


//
// Public Methods
//

HRESULT SlicerFactory::CreateDwmSlicer(IDwmSlicer** argDwmSlicer)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argDwmSlicer);

    *argDwmSlicer = nullptr;

    // Create a new DWM slicer
    DwmSlicer* dwmSlicer = new DwmSlicer();
    IFCOOM(dwmSlicer);

    // Initialize the slicer
    IFC(dwmSlicer->Initialize(this->mD2DContext->mDxgiAdapters, this->mD2DContext->mD3DDevices, this->mD2DContext->mD2DDevice));

    // Assign and AddRef
    *argDwmSlicer = dwmSlicer;
    dwmSlicer->AddRef();


Cleanup:

    if (FAILED(hResult))
    {
        if (nullptr != dwmSlicer)
        {
            delete dwmSlicer;
        }
    }

    return hResult;
}


HRESULT SlicerFactory::CreateGdiSlicer(IGdiSlicer** argGdiSlicer)
{
    HRESULT hResult;

    // Validate the argument
    IFRNULL(argGdiSlicer);

    *argGdiSlicer = nullptr;

    // Create a new GDI slicer
    GdiSlicer* gdiSlicer = new GdiSlicer();
    IFCOOM(gdiSlicer);

    // Initialize the slicer
    IFC(gdiSlicer->Initialize(this->mD2DContext->mD2DDevice, this->mD2DContext->mD2DFeatureLevel));

    // Assign and AddRef
    *argGdiSlicer = gdiSlicer;
    gdiSlicer->AddRef();


Cleanup:

    if (FAILED(hResult))
    {
        if (nullptr != gdiSlicer)
        {
            delete gdiSlicer;
        }
    }

    return hResult;
}


HRESULT STDMETHODCALLTYPE SlicerFactory::GetD2DContext(ID2DContext** argD2DContext)
{
    HRESULT hResult = NOERROR;

    // Validate the argument
    IFRNULL(argD2DContext);

    // Assign and AddRef
    *argD2DContext = this->mD2DContext;
    this->mD2DContext->AddRef();

    return hResult;
}


ID2DContext* STDMETHODCALLTYPE SlicerFactory::GetD2DContextNoRef(void)
{
    return this->mD2DContext;
}