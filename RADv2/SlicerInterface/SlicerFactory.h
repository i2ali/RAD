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

namespace RADv2
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


    // Public Properties
    public:
        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        HRESULT GetD2DDevice(ID2D1Device** argDevice) const;

        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        ID2D1Device* GetD2DDeviceNoRef(void) const;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        HRESULT GetD2DFactory(ID2D1Factory1** argFactory) const;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        ID2D1Factory1* GetD2DFactoryNoRef(void) const;

        /// <summary>
        /// Returns the feature level of the D2D device.
        /// </summary>
        D3D_FEATURE_LEVEL GetD2DFeatureLevel(void) const;

        /// <summary>
        /// Returns the highest feature level D3D11 device we have created.
        /// </summary>
        D3D_FEATURE_LEVEL GetHighestFeatureLevel(void) const;

        /// <summary>
        /// Returns the lowest feature level D3D11 device we have created.
        /// </summary>
        D3D_FEATURE_LEVEL GetLowestFeatureLevel(void) const;


    // Public Methods
    public:
        HRESULT CreateDwmSlicer(
            IDwmSlicer** argDwmSlicer);

        HRESULT CreateGdiSlicer(
            IGdiSlicer** argGdiSlicer);

        HRESULT CreateFrame(
            const D2D1_SIZE_U& argFrameSize,
            UINT argFrameIndex,
            ID2D1Bitmap1* argAtlas,
            ID2D1Bitmap1** argFrame,
            D2D1_BITMAP_OPTIONS argOptions = D2D1_BITMAP_OPTIONS_NONE);

        HRESULT LoadImageFromFile(
            wchar_t const * const argFileName,
            ID2D1Bitmap1** argBitmapOut);

        HRESULT RenderOutput(
            ID2D1Image* argEffectOutput);
               
        HRESULT SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            const GUID& argFileFormat);

        HRESULT SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            const GUID& argFileFormat,
            _In_opt_ PWSTR pszFrameWidth,
            _In_opt_ PWSTR pszFrameHeight);


    // Private Data Members
    private:
        // Factories
        std::shared_ptr<IDXGIFactory>  mDxgiFactory;
        std::shared_ptr<ID2D1Factory1> mD2DFactory;

        // List of adapters and D3D devices
        std::vector<std::shared_ptr<IDXGIAdapter>> mDxgiAdapters;
        std::vector<std::shared_ptr<ID3D11Device>> mD3DDevices;
        std::vector<D3D_FEATURE_LEVEL> mD3DFeatureLevels;

        // The D2D device created on the best adapter
        std::shared_ptr<ID2D1Device>        mD2DDevice;
        std::shared_ptr<ID2D1DeviceContext> mD2DDeviceContext;
        D3D_FEATURE_LEVEL              mD2DFeatureLevel;

        // Highest and lowest feature level index
        UINT mHighestFeatureLevelIndex;
        UINT mLowestFeatureLevelIndex;

        // Determines if the D2D device will be hardware or software
        ComputeShaderModel mComputeShaderModelRequired;

        // IUnknown ref count
        LONG mRefCount;

        // ISSUE--mhaight--02/19/11
        //  Friend functions are scoped to the surrounding namespace, however, we
        //  cannot scope to the global namespace, so this function is in the RADv2
        //  namespace.
        friend HRESULT CreateSlicerFactory(ISlicerFactory** argSlicerFactory, ComputeShaderModel argComputeShaderModelRequired);

    };
}