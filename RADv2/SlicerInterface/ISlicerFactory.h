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

namespace RADv2
{
    struct __declspec(uuid("{2f08f9c0-3e39-4d49-bc69-dcdd113cd058}")) ISlicerFactory;

    struct ISlicerFactory : public IUnknown
    {
    // Public Properties
    public:
        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        virtual HRESULT GetD2DDevice(ID2D1Device** argDevice) const = 0;

        /// <summary>
        /// Returns a D2D device.
        /// </summary>
        virtual ID2D1Device* GetD2DDeviceNoRef(void) const = 0;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        virtual HRESULT GetD2DFactory(ID2D1Factory1** argFactory) const = 0;

        /// <summary>
        /// Returns a D2D factory.
        /// </summary>
        virtual ID2D1Factory1* GetD2DFactoryNoRef(void) const = 0;

        /// <summary>
        /// Returns the feature level of the D2D device.
        /// </summary>
        virtual D3D_FEATURE_LEVEL GetD2DFeatureLevel(void) const = 0;

        /// <summary>
        /// Returns the highest feature level D3D11 device we have created.
        /// </summary>
        virtual D3D_FEATURE_LEVEL GetHighestFeatureLevel(void) const = 0;

        /// <summary>
        /// Returns the lowest feature level D3D11 device we have created.
        /// </summary>
        virtual D3D_FEATURE_LEVEL GetLowestFeatureLevel(void) const = 0;


    // Public Methods
    public:
        virtual HRESULT CreateDwmSlicer(
            IDwmSlicer** argDwmSlicer) = 0;

        virtual HRESULT CreateGdiSlicer(
            IGdiSlicer** argGdiSlicer) = 0;

        virtual HRESULT CreateFrame(
            const D2D1_SIZE_U& argFrameSize,
            UINT argFrameIndex,
            ID2D1Bitmap1* argAtlas,
            ID2D1Bitmap1** argFrame,
            D2D1_BITMAP_OPTIONS argOptions = D2D1_BITMAP_OPTIONS_NONE) = 0;

        virtual HRESULT LoadImageFromFile(
            wchar_t const * const argFileName,
            ID2D1Bitmap1** argBitmapOut) = 0;

        virtual HRESULT RenderOutput(
            ID2D1Image* argEffectOutput) = 0;

        virtual HRESULT SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            const GUID& argFileFormat) = 0;

        virtual HRESULT SaveImageToFile(
            wchar_t const * const argFileName,
            ID2D1Image* argImage,
            const GUID& argFileFormat,
            _In_opt_ PWSTR pszFrameWidth,
            _In_opt_ PWSTR pszFrameHeight) = 0;

    };
}