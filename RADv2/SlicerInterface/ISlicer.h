//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      This interface provides methods for a slicer.
//
//------------------------------------------------------------------------------

#pragma once

#include <d2d1_1.h>

#include "Channel.h"

namespace RADv2
{
    struct __declspec(uuid("{6c420031-fed2-403b-84dd-c649af6677e4}")) ISlicer;

    struct ISlicer : public IUnknown
    {
    // Interface methods
    public:
        virtual bool IsSlicing(void) = 0;

        virtual HRESULT EndCapture(
            __out UINT* argFramesCaptured = nullptr,
            __out ID2D1Bitmap1** argLeftImage = nullptr,
            __out ID2D1Bitmap1** argRightImage = nullptr) = 0;

        virtual HRESULT StartCapture(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel = static_cast<Channel>(1),
            __out UINT* argNumberOfFramesThatCanBeCaptured = nullptr) = 0;

    };
}