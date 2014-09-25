//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This interface provides the methods necessary for creating slicers.
//
//------------------------------------------------------------------------------

#pragma once

#include "ID2DContext.h"
#include "IDwmSlicer.h"
#include "IGdiSlicer.h"

namespace RAD
{
    struct __declspec(uuid("{ab98c3a5-e458-45fa-960b-cbc4038d9ff6}")) ISlicerFactory;

    struct ISlicerFactory : public IUnknown
    {
    // Public Methods
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateDwmSlicer(
            IDwmSlicer** argDwmSlicer) = 0;

        virtual HRESULT STDMETHODCALLTYPE CreateGdiSlicer(
            IGdiSlicer** argGdiSlicer) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetD2DContext(
            ID2DContext** argD2DContext) = 0;

        virtual ID2DContext* STDMETHODCALLTYPE GetD2DContextNoRef(void) = 0;

    };
}