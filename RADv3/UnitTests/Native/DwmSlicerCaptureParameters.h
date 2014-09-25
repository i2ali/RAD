//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

using namespace WEX::Common;

namespace UnitTests
{
    class DwmSlicerCaptureParameters : public WEX::TestClass<DwmSlicerCaptureParameters>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(DwmSlicerCaptureParameters)
            TEST_CLASS_PROPERTY(L"TestOwner", L"mhaight")
        END_TEST_CLASS()


    // Class Setup / Cleanup
    public:
        TEST_CLASS_SETUP(ClassSetup)

        TEST_CLASS_CLEANUP(ClassCleanup)


    // Test Setup / Cleanup
    public:
        TEST_METHOD_SETUP(MethodSetup)

        TEST_METHOD_CLEANUP(MethodCleanup)


    // Test Methods
    public:
        ///
        /// This test captures only left channel
        ///
        TEST_METHOD(CaptureLeftChannel)

        ///
        /// This test captures only right channel
        ///
        TEST_METHOD(CaptureRightChannel)

        ///
        /// This test captures left and right channel
        ///
        TEST_METHOD(CaptureLeftAndRightChannel)


    private:
        CComPtr<RAD::ID2DContext>    mD2DContext;
        CComPtr<RAD::ISlicerFactory> mSlicerFactory;
        CComPtr<RAD::IDwmSlicer>     mSlicer;

    };
}