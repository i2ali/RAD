//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This interface provides the methods necessary for getting the DirectX
//      objects created for the slicer interface.
//
//------------------------------------------------------------------------------

#pragma once

#include "ImageContainerFormat.h"

namespace RAD
{
    struct __declspec(uuid("{09df6099-29df-40b4-950d-220654865b0d}")) ID2DContext;

    struct ID2DContext : public IUnknown
    {
    // Public Properties
    public:
        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        virtual HRESULT STDMETHODCALLTYPE GetD2DDevice(ID2D1Device** argDevice) const = 0;

        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        virtual ID2D1Device* STDMETHODCALLTYPE GetD2DDeviceNoRef(void) const = 0;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        virtual HRESULT STDMETHODCALLTYPE GetD2DFactory(ID2D1Factory1** argFactory) const = 0;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        virtual ID2D1Factory1* STDMETHODCALLTYPE GetD2DFactoryNoRef(void) const = 0;

        /// <summary>
        /// Returns the feature level of the D2D device.
        /// </summary>
        virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetD2DFeatureLevel(void) const = 0;

        /// <summary>
        /// Returns the highest feature level D3D11 device we have created.
        /// </summary>
        virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetHighestFeatureLevel(void) const = 0;

        /// <summary>
        /// Returns the lowest feature level D3D11 device we have created.
        /// </summary>
        virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetLowestFeatureLevel(void) const = 0;


    // Public Methods
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateFrame(
            const D2D1_SIZE_U& argFrameSize,
            UINT argFrameIndex,
            ID2D1Bitmap1* argAtlas,
            ID2D1Bitmap1** argFrame,
            D2D1_BITMAP_OPTIONS argOptions = D2D1_BITMAP_OPTIONS_NONE) = 0;

        virtual HRESULT STDMETHODCALLTYPE LoadImageFromFile(
            wchar_t const * const argFileName,
            ID2D1Bitmap1** argBitmapOut) = 0;

        virtual HRESULT STDMETHODCALLTYPE RenderOutput(
            ID2D1Image* argEffectOutput) = 0;

        virtual HRESULT STDMETHODCALLTYPE SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            ImageContainerFormat argFormat) = 0;

    };
}