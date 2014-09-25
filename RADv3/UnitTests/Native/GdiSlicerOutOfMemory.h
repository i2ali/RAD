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
    class GdiSlicerOutOfMemory : public WEX::TestClass<GdiSlicerOutOfMemory>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(GdiSlicerOutOfMemory)
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
        CComPtr<RAD::ISlicerFactory> mSlicerFactory;
        CComPtr<RAD::IGdiSlicer>     mSlicer;
        HMODULE                        mGdi32;

    };
}