//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      The DwmSlicer uses the SlicerControl method to capture slices of the
//      screen.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

#include "ISlicer.h"
#include "SlicerFactory.h"
#include "SystemBitmap.h"

namespace RAD { namespace Managed
{
    private ref class SlicerBase : public ISlicer
    {
    // Constructor
    public:
        SlicerBase(
            RAD::ISlicer* argDwmSlicer,
            RAD::ISlicerFactory* argSlicerFactory,
            HMODULE argSlicerInterfaceModule);


    // Destructor
    public:
        virtual ~SlicerBase(void);


    // Finalizer
    public:
        !SlicerBase(void);


    // Public Methods
    public:
        virtual bool IsSlicing(void);

        virtual HRESULT CalculateMaxFramesThatCanBeCaptured(
            LONG argLeft,
            LONG argTop,
            LONG argRight,
            LONG argBottom,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            UINT% argNumberOfFramesThatCanBeCaptured);

        virtual HRESULT EndCapture(void);

        virtual HRESULT EndCapture(
            UINT% argFramesCaptured);

        virtual HRESULT EndCapture(
            UINT% argFramesCaptured,
            [Out] SystemBitmap^% argLeftImage);

        virtual HRESULT EndCapture(
            UINT% argFramesCaptured,
            [Out] SystemBitmap^% argLeftImage,
            [Out] SystemBitmap^% argRightImage);

        virtual HRESULT StartCapture(
            LONG argLeft,
            LONG argTop,
            LONG argRight,
            LONG argBottom,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip);

        virtual HRESULT StartCapture(
            LONG argLeft,
            LONG argTop,
            LONG argRight,
            LONG argBottom,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel);

        virtual HRESULT StartCapture(
            LONG argLeft,
            LONG argTop,
            LONG argRight,
            LONG argBottom,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            UINT% argNumberOfFramesThatCanBeCaptured);


    // Private Data Members
    private:
        RAD::ISlicer*        mSlicer;
        RAD::ISlicerFactory* mSlicerFactory;
        HMODULE              mSlicerInterfaceModule;

    };
} }