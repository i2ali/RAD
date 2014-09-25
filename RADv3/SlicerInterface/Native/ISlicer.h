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

namespace RAD
{
    struct __declspec(uuid("{b90b5378-de44-40d6-83e5-38e68014717c}")) ISlicer;

    struct ISlicer : public IUnknown
    {
    // Interface methods
    public:
        virtual bool STDMETHODCALLTYPE IsSlicing(void) = 0;

        virtual HRESULT STDMETHODCALLTYPE CalculateMaxFramesThatCanBeCaptured(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            __out UINT* argNumberOfFramesThatCanBeCaptured) = 0;

        virtual HRESULT STDMETHODCALLTYPE EndCapture(
            __out UINT* argFramesCaptured = nullptr,
            __out ID2D1Bitmap1** argLeftImage = nullptr,
            __out ID2D1Bitmap1** argRightImage = nullptr) = 0;

        virtual HRESULT STDMETHODCALLTYPE StartCapture(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel = static_cast<Channel>(1),
            __out UINT* argNumberOfFramesThatCanBeCaptured = nullptr) = 0;

    };
}