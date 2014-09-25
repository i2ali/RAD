//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Provides an implementation of the ID2DContext interface.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"
#include "ID2DContext.h"
#include "ComputeShaderModel.h"

namespace RAD
{
    /// <summary>
    /// This is the full implementation of the ID2DContext interface.  It
    /// contains all of the D2D and D3D objects required for creating textures.
    /// </summary>
    class D2DContext : public ID2DContext
    {
    // Constructor
    private:
        D2DContext(ComputeShaderModel argComputeShaderModelRequired);

        HRESULT Initialize(void);


    // Destructor
    private:
        ~D2DContext(void);


    // IUnknown Methods
    public:
        ULONG STDMETHODCALLTYPE AddRef(void);

        ULONG STDMETHODCALLTYPE Release(void);

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            LPVOID* ppvObj);


    // Public Properties
    public:
        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        HRESULT STDMETHODCALLTYPE GetD2DDevice(ID2D1Device** argDevice) const;

        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        ID2D1Device* STDMETHODCALLTYPE GetD2DDeviceNoRef(void) const;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        HRESULT STDMETHODCALLTYPE GetD2DFactory(ID2D1Factory1** argFactory) const;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        ID2D1Factory1* STDMETHODCALLTYPE GetD2DFactoryNoRef(void) const;

        /// <summary>
        /// Returns the feature level of the D2D device.
        /// </summary>
        D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetD2DFeatureLevel(void) const;

        /// <summary>
        /// Returns the highest feature level D3D11 device we have created.
        /// </summary>
        D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetHighestFeatureLevel(void) const;

        /// <summary>
        /// Returns the lowest feature level D3D11 device we have created.
        /// </summary>
        D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetLowestFeatureLevel(void) const;


    // Public Methods
    public:
        HRESULT STDMETHODCALLTYPE CreateFrame(
            const D2D1_SIZE_U& argFrameSize,
            UINT argFrameIndex,
            ID2D1Bitmap1* argAtlas,
            ID2D1Bitmap1** argFrame,
            D2D1_BITMAP_OPTIONS argOptions = D2D1_BITMAP_OPTIONS_NONE);

        HRESULT STDMETHODCALLTYPE LoadImageFromFile(
            wchar_t const * const argFileName,
            ID2D1Bitmap1** argBitmapOut);

        HRESULT STDMETHODCALLTYPE RenderOutput(
            ID2D1Image* argEffectOutput);

        HRESULT STDMETHODCALLTYPE SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            ImageContainerFormat argFormat);


    // Private Data Members
    private:
        // Factories
        std::shared_ptr<IDXGIFactory>  mDxgiFactory;
        std::shared_ptr<ID2D1Factory1> mD2DFactory;

        // List of adapters and D3D devices
        std::vector<std::shared_ptr<IDXGIAdapter>> mDxgiAdapters;
        std::vector<std::shared_ptr<ID3D11Device>> mD3DDevices;
        std::vector<D3D_FEATURE_LEVEL>             mD3DFeatureLevels;

        // The D2D device created on the best adapter
        std::shared_ptr<ID2D1Device>        mD2DDevice;
        std::shared_ptr<ID2D1DeviceContext> mD2DDeviceContext;
        D3D_FEATURE_LEVEL                   mD2DFeatureLevel;

        // Highest and lowest feature level index
        UINT mHighestFeatureLevelIndex;
        UINT mLowestFeatureLevelIndex;

        // Determines if the D2D device will be hardware or software
        ComputeShaderModel mComputeShaderModelRequired;

        // IUnknown ref count
        LONG mRefCount;

        // Friends
        friend class SlicerFactory;

    };
}
