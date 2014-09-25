//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This interface provides methods for a slicer.
//
//------------------------------------------------------------------------------

#pragma once

#include "Channel.h"
#include "SystemBitmap.h"

namespace RAD { namespace Managed
{
    public interface class ISlicer
    {
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

    };
} }