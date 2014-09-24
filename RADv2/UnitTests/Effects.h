//-------------------------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//-------------------------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

using namespace WEX::Common;
using namespace WindowManager;

namespace UnitTests
{
    class Effects : public WEX::TestClass<Effects>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(Effects)
            TEST_CLASS_PROPERTY(L"TestOwner", L"mhaight")
        END_TEST_CLASS()


    // Class Setup / Cleanup
    public:
        TEST_CLASS_SETUP(ClassSetup)

        TEST_CLASS_CLEANUP(ClassCleanup)


    // Test Methods
    public:
        TEST_METHOD(LabConversion)

        TEST_METHOD(LabDistance)

        TEST_METHOD(LabColorComparerGraph)

        TEST_METHOD(LabImageComparerGraph)

        TEST_METHOD(RgbColorComparerGraph)

        TEST_METHOD(RgbImageComparerGraph)


    // Helper Methods
    private:
        struct ColorRect
        {
            ColorRect(D2D1_RECT_F argRect, RgbColor argColor) : rect(argRect), color(argColor) { }
            D2D1_RECT_F rect;
            RgbColor    color;
        };

        void RenderRects(ID2D1DeviceContext* argDeviceContext, D2D1_SIZE_U argSize, const std::vector<ColorRect>& argColorRects, ID2D1Bitmap1** argBitmap);


    private:
        CComPtr<RADv2::ISlicerFactory> mSlicerFactory;

    };
}