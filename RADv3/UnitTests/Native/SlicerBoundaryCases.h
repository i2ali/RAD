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
    class SlicerBoundaryCases : public WEX::TestClass<SlicerBoundaryCases>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(SlicerBoundaryCases)
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
        /// This test method creates a 100x100 capture rect that is centered
        /// on the corners of the primary output.
        ///
        TEST_METHOD(CaptureOnTheCornersOfThePrimaryDisplay)

        ///
        /// This test method creates a 1x1 capture rect on the inside corners
        /// of the primary output.
        ///
        TEST_METHOD(CaptureOneByOneInside)

        ///
        /// This test method creates a 1x1 capture rect on the outside corners
        /// of the primary output.
        ///
        TEST_METHOD(CaptureOneByOneOutside)

        ///
        /// This test method captures the full primary output.
        ///
        TEST_METHOD(CaptureFullPrimaryOutput)

        ///
        /// This test method captures the full primary output inflated by one
        /// pixel on each edge.
        ///
        TEST_METHOD(CaptureInflatedFullPrimaryOutput)


    private:
        HMODULE mModuleHandle;
        bool    mSlicing;
        ATOM    mWindowClass;
        HWND    mWindowHandle;

    };
}