//-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//-----------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

using namespace WEX::Common;

namespace UnitTests
{
    ///
    /// BadParameters
    ///
    class SlicerBadParameters : public WEX::TestClass<SlicerBadParameters>
    {
    // TAEF Class Information
    public:
        BEGIN_TEST_CLASS(SlicerBadParameters) //The metadata specified below applies to all the test in this class
            TEST_CLASS_PROPERTY(L"Priority", L"2")
            TEST_CLASS_PROPERTY(L"TestOwner", L"mhaight")
        END_TEST_CLASS()


    // Class Setup / Cleanup
    public:
        TEST_CLASS_SETUP(ClassSetup)

        TEST_CLASS_CLEANUP(ClassCleanup)


    // Test Setup / Cleanup
    public:
        TEST_METHOD_SETUP(TestSetup)

        TEST_METHOD_CLEANUP(TestCleanup)


    // Slicer Command Tests
    public:
        ///
        /// Tests the API when supplied with a bad SLICER_COMMAND.
        ///
        TEST_METHOD(SlicerContext_BadCommand)


    // Normal Initialize and Deinitialize Tests
    public:
        ///
        /// Tests the API when performing a normal Initialize and Deinitialize.
        ///
        TEST_METHOD(SlicerContext_InitializeAndDeinitialize)


    // SlicerContextInitialize Command Tests
    public:
        ///
        /// Tests Initialize command when supplying it with a bad Window Handle.
        ///
        TEST_METHOD(SlicerContextInitialize_BadWindowHandle)

        ///
        /// Tests Initialize command when supplying it with bad variable
        /// arguments.
        ///
        TEST_METHOD(SlicerContextInitialize_BadVariableArguments)

        ///
        /// Tests Initialize command when supplying it with a valid Message Only
        /// Window Handle.
        ///
        TEST_METHOD(SlicerContextInitialize_MessageOnlyWindow)


    // SlicerContextDeinitialize Command Tests
    public:
        ///
        /// Tests Deinitialize command when supplying it with a bad Window
        /// Handle.
        ///
        TEST_METHOD(SlicerContextDeinitialize_BadWindowHandle)

        ///
        /// Tests Deinitialize command when supplying it with bad variable
        /// arguments.
        ///
        TEST_METHOD(SlicerContextDeinitialize_BadVariableArguments)

        ///
        /// Tests Deinitialize command when called before initialize.
        ///
        TEST_METHOD(SlicerContextDeinitialize_BeforeInitialize)


    // SlicerContextSetDestinationTextures Command Tests
    public:
        ///
        /// Tests SlicerContextSetDestinationTextures command when called before initialize.
        ///
        TEST_METHOD(SlicerContextSetDestinationTextures_BeforeInitialize)

        ///
        /// Tests SlicerContextSetDestinationTextures command when called with a
        /// bad window handle.
        ///
        TEST_METHOD(SlicerContextSetDestinationTextures_BadWindowHandle)

        ///
        /// Tests SlicerContextSetDestinationTextures command when called before initialize.
        ///
        TEST_METHOD(SlicerContextSetDestinationTextures_BadVariableArguments)


    // SlicerContextBeginCapture Command Tests
    public:
        TEST_METHOD(SlicerContextBeginCapture_BeforeInitialize)

        TEST_METHOD(SlicerContextBeginCapture_BeforeSetDestinationTextures)

        TEST_METHOD(SlicerContextBeginCapture_BadWindowHandle)

        TEST_METHOD(SlicerContextBeginCapture_BadVariableArguments)


    // SlicerContextEndCapture Command Tests
    public:
        TEST_METHOD(SlicerContextEndCapture_BeforeInitialize)

        TEST_METHOD(SlicerContextEndCapture_BeforeBeginCapture)

        TEST_METHOD(SlicerContextEndCapture_BadVariableArguments)

        TEST_METHOD(SlicerContextEndCapture_BadTextureSize)


    // Private Data Members
    private:
        HMODULE mModuleHandle;
        bool    mSlicing;
        ATOM    mWindowClass;
        HWND    mWindowHandle;
        HWND    mWindowHandleMessageOnly;

        RECT mCaptureRect;
        UINT mLargeTextureWidth;
        UINT mLargeTextureHeight;
        UINT mSmallTextureWidth;
        UINT mSmallTextureHeight;

        std::map<std::shared_ptr<ID3D11Texture2D>, SLICER_SHARED_TEXTURE> mLargeTexture;
        std::map<std::shared_ptr<ID3D11Texture2D>, SLICER_SHARED_TEXTURE> mSmallTexture;

    };
}