//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Provides a full implementation of the ISlicerFactory.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"
#include "ISlicerFactory.h"
#include "ComputeShaderModel.h"
#include "D2DContext.h"

namespace RAD
{
    /// <summary>
    /// This is the full implementation of the ISlicerFactory interface.  It
    /// provides the support necessary for slicers.
    /// </summary>
    class SlicerFactory : public ISlicerFactory
    {
    // Constructor
    private:
        SlicerFactory(ComputeShaderModel argComputeShaderModelRequired);

        HRESULT Initialize(void);


    // Destructor
    private:
        ~SlicerFactory(void);


    // IUnknown Methods
    public:
        ULONG STDMETHODCALLTYPE AddRef(void);

        ULONG STDMETHODCALLTYPE Release(void);

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            LPVOID* ppvObj);


    // Public Methods
    public:
        HRESULT STDMETHODCALLTYPE CreateDwmSlicer(
            IDwmSlicer** argDwmSlicer);

        HRESULT STDMETHODCALLTYPE CreateGdiSlicer(
            IGdiSlicer** argGdiSlicer);

        HRESULT STDMETHODCALLTYPE GetD2DContext(
            ID2DContext** argD2DContext);

        ID2DContext* STDMETHODCALLTYPE GetD2DContextNoRef(void);


    // Private Data Members
    private:
        ComputeShaderModel mComputeShaderModelRequired;
        D2DContext*        mD2DContext;
        LONG               mRefCount;


    // Private Friend Functions
    private:
        // ISSUE--mhaight--02/19/11
        //  Friend functions are scoped to the surrounding namespace, however, we
        //  cannot scope to the global namespace, so this function is in the RAD
        //  namespace.
        friend HRESULT CreateSlicerFactory1(ISlicerFactory** argSlicerFactory, ComputeShaderModel argComputeShaderModelRequired);

    };
}
