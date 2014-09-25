//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This interface provides the methods necessary for creating slicers.
//
//------------------------------------------------------------------------------

#pragma once

#include "IDwmSlicer.h"
#include "IGdiSlicer.h"
#include "ComputeShaderModel.h"

namespace RAD { namespace Managed
{
    public ref class SlicerFactory sealed
    {
    // Constructor
    private:
        SlicerFactory(
            RAD::ISlicerFactory* argSlicerFactory,
            HMODULE argSlicerInterfaceModule);


    // Destructor
    public:
        ~SlicerFactory(void);


    // Finalizer
    public:
        !SlicerFactory(void);


    // Public Methods
    public:
        HRESULT CreateDwmSlicer(
            [Out] IDwmSlicer^% argDwmSlicer);

        HRESULT CreateGdiSlicer(
            [Out] IGdiSlicer^% argGdiSlicer);


    // Private Data Members
    private:
        RAD::ISlicerFactory* mSlicerFactory;
        HMODULE              mSlicerInterfaceModule;


    // Public Static Methods
    public:
        static HRESULT Create(
            [Out] SlicerFactory^% argSlicerFactory,
            ComputeShaderModel argComputeShaderModelRequired);

    };
} }