//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"

#include "DwmSlicer.h"
#include "GdiSlicer.h"

using namespace RAD::Managed;


//
// Constructor
//

SlicerFactory::SlicerFactory(RAD::ISlicerFactory* argSlicerFactory, HMODULE argSlicerInterfaceModule)
    : mSlicerFactory(argSlicerFactory), mSlicerInterfaceModule(argSlicerInterfaceModule)
{
}


//
// Destructor
//

SlicerFactory::~SlicerFactory(void)
{
    this->!SlicerFactory();
}


//
// Finalizer
//

SlicerFactory::!SlicerFactory(void)
{
    if (nullptr != this->mSlicerFactory)
    {
        this->mSlicerFactory->Release();
        this->mSlicerFactory = nullptr;
    }

    if (nullptr != this->mSlicerInterfaceModule)
    {
        ::FreeLibrary(this->mSlicerInterfaceModule);
        this->mSlicerInterfaceModule = nullptr;
    }
}


//
// Public Methods
//

HRESULT SlicerFactory::CreateDwmSlicer(
    [Out] IDwmSlicer^% argDwmSlicer)
{
    HMODULE hModule = ::LoadLibraryW(cRADLibraryName);

    if (nullptr == hModule)
    {
        return E_NOINTERFACE;
    }

    RAD::IDwmSlicer* dwmSlicer = nullptr;
    HRESULT hResult = this->mSlicerFactory->CreateDwmSlicer(&dwmSlicer);

    if (SUCCEEDED(hResult))
    {
        try
        {
            argDwmSlicer = gcnew DwmSlicer(dwmSlicer, this->mSlicerFactory, hModule);
        }
        catch (...)
        {
            argDwmSlicer = nullptr;
            ::FreeLibrary(hModule);
        }
    }
    else
    {
        argDwmSlicer = nullptr;
        ::FreeLibrary(hModule);
    }

    return hResult;
}


HRESULT SlicerFactory::CreateGdiSlicer(
    [Out] IGdiSlicer^% argGdiSlicer)
{
    HMODULE hModule = ::LoadLibraryW(cRADLibraryName);

    if (nullptr == hModule)
    {
        return E_NOINTERFACE;
    }

    RAD::IGdiSlicer* gdiSlicer = nullptr;
    HRESULT hResult = this->mSlicerFactory->CreateGdiSlicer(&gdiSlicer);

    if (SUCCEEDED(hResult))
    {
        try
        {
            argGdiSlicer = gcnew GdiSlicer(gdiSlicer, this->mSlicerFactory, hModule);
        }
        catch (...)
        {
            argGdiSlicer = nullptr;
            ::FreeLibrary(hModule);
        }
    }
    else
    {
        argGdiSlicer = nullptr;
        ::FreeLibrary(hModule);
    }

    return hResult;
}


//
// Static Methods
//

HRESULT SlicerFactory::Create(
    [Out] SlicerFactory^% argSlicerFactory,
    ComputeShaderModel argComputeShaderModelRequired)
{
    HMODULE hModule = ::LoadLibraryW(cRADLibraryName);

    if (nullptr == hModule)
    {
        return E_NOINTERFACE;
    }

    typedef HRESULT (_cdecl *CREATESLICERFACTORY)(RAD::ISlicerFactory** argSlicerFactory, RAD::ComputeShaderModel argComputeShaderModelRequired);
    CREATESLICERFACTORY createSlicerFactory = nullptr;

    createSlicerFactory = reinterpret_cast<CREATESLICERFACTORY>(::GetProcAddress(hModule, "CreateSlicerFactory"));

    if (nullptr == createSlicerFactory)
    {
        ::FreeLibrary(hModule);
        return E_NOINTERFACE;
    }

    RAD::ComputeShaderModel computeShaderModel;

    switch (argComputeShaderModelRequired)
    {
        case ComputeShaderModel::None:
            computeShaderModel = RAD::ComputeShaderModel::None;
            break;

        case ComputeShaderModel::CS_4_0:
            computeShaderModel = RAD::ComputeShaderModel::CS_4_0;
            break;

        case ComputeShaderModel::CS_4_1:
            computeShaderModel = RAD::ComputeShaderModel::CS_4_1;
            break;

        case ComputeShaderModel::CS_5_0:
            computeShaderModel = RAD::ComputeShaderModel::CS_5_0;
            break;

        default:
            ::FreeLibrary(hModule);
            return E_INVALIDARG;

    }

    RAD::ISlicerFactory* slicerFactory = nullptr;

    HRESULT hResult = createSlicerFactory(&slicerFactory, computeShaderModel);

    if (SUCCEEDED(hResult))
    {
        try
        {
            argSlicerFactory = gcnew SlicerFactory(slicerFactory, hModule);
        }
        catch (...)
        {
            argSlicerFactory = nullptr;
            ::FreeLibrary(hModule);
        }
    }
    else
    {
        argSlicerFactory = nullptr;
        ::FreeLibrary(hModule);
    }

    return hResult;
}