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
    class DwmSlicerOutOfMemory : public WEX::TestClass<DwmSlicerOutOfMemory>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(DwmSlicerOutOfMemory)
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
        TEST_METHOD(CapturePrimaryDisplay)


    private:
        CComPtr<RADv2::ISlicerFactory> mSlicerFactory;
        CComPtr<RADv2::IDwmSlicer>     mSlicer;
        HMODULE                        mGdi32;

    };
}