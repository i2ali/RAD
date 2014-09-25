//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "GdiSlicerCaptureParameters.h"

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool GdiSlicerCaptureParameters::ClassSetup(void)
{
    VERIFY_SUCCEEDED(::CreateSlicerFactory(&this->mSlicerFactory));
    VERIFY_SUCCEEDED(this->mSlicerFactory->CreateGdiSlicer(&this->mSlicer));

    return true;
}


bool GdiSlicerCaptureParameters::ClassCleanup(void)
{
    return true;
}


bool GdiSlicerCaptureParameters::MethodSetup(void)
{
    return true;
}


bool GdiSlicerCaptureParameters::MethodCleanup(void)
{
    return true;
}


void GdiSlicerCaptureParameters::CaptureLeftChannel(void)
{
    auto doCapture = [this](UINT* argFramesCaptured, ID2D1Bitmap1** argLeftChannel, ID2D1Bitmap1** argRightChannel)
    {
        RECT topLeft;
        topLeft.top =    0;
        topLeft.left =   0;
        topLeft.bottom = 100;
        topLeft.right =  100;

        UINT numberOfFramesThatCanBeCaptured = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 60, 0, RAD::Channel::Left, &numberOfFramesThatCanBeCaptured));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

        ::Sleep(500);

        UINT framesCaptured = 0;
        CComPtr<ID2D1Bitmap1> leftChannel;
        CComPtr<ID2D1Bitmap1> rightChannel;

        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(argFramesCaptured, argLeftChannel, argRightChannel));

        if (nullptr != argFramesCaptured)
        {
            Log::Comment(String().Format(L"Frames Captured: %d", *argFramesCaptured));
        }

        if (nullptr != argLeftChannel)
        {
            VERIFY_IS_NOT_NULL(*argLeftChannel);
        }

        if (nullptr != argRightChannel)
        {
            VERIFY_IS_NULL(*argRightChannel);
        }
    };

    UINT framesCaptured = 0;
    CComPtr<ID2D1Bitmap1> leftChannel;
    CComPtr<ID2D1Bitmap1> rightChannel;

    doCapture(&framesCaptured, &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
}


void GdiSlicerCaptureParameters::CaptureRightChannel(void)
{
    auto doCapture = [this](UINT* argFramesCaptured, ID2D1Bitmap1** argLeftChannel, ID2D1Bitmap1** argRightChannel)
    {
        RECT topLeft;
        topLeft.top =    0;
        topLeft.left =   0;
        topLeft.bottom = 100;
        topLeft.right =  100;

        UINT numberOfFramesThatCanBeCaptured = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 60, 0, RAD::Channel::Right, &numberOfFramesThatCanBeCaptured));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

        ::Sleep(500);

        UINT framesCaptured = 0;
        CComPtr<ID2D1Bitmap1> leftChannel;
        CComPtr<ID2D1Bitmap1> rightChannel;

        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(argFramesCaptured, argLeftChannel, argRightChannel));

        if (nullptr != argFramesCaptured)
        {
            Log::Comment(String().Format(L"Frames Captured: %d", *argFramesCaptured));
        }

        if (nullptr != argLeftChannel)
        {
            VERIFY_IS_NULL(*argLeftChannel);
        }

        if (nullptr != argRightChannel)
        {
            VERIFY_IS_NOT_NULL(*argRightChannel);
        }
    };

    UINT framesCaptured = 0;
    CComPtr<ID2D1Bitmap1> leftChannel;
    CComPtr<ID2D1Bitmap1> rightChannel;

    doCapture(&framesCaptured, &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
}


void GdiSlicerCaptureParameters::CaptureLeftAndRightChannel(void)
{
    auto doCapture = [this](UINT* argFramesCaptured, ID2D1Bitmap1** argLeftChannel, ID2D1Bitmap1** argRightChannel)
    {
        RECT topLeft;
        topLeft.top =    0;
        topLeft.left =   0;
        topLeft.bottom = 100;
        topLeft.right =  100;

        UINT numberOfFramesThatCanBeCaptured = 0;
        VERIFY_SUCCEEDED(this->mSlicer->StartCapture(topLeft, 60, 0, RAD::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
        Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

        ::Sleep(500);

        UINT framesCaptured = 0;
        CComPtr<ID2D1Bitmap1> leftChannel;
        CComPtr<ID2D1Bitmap1> rightChannel;

        VERIFY_SUCCEEDED(this->mSlicer->EndCapture(argFramesCaptured, argLeftChannel, argRightChannel));

        if (nullptr != argFramesCaptured)
        {
            Log::Comment(String().Format(L"Frames Captured: %d", *argFramesCaptured));
        }

        if (nullptr != argLeftChannel)
        {
            VERIFY_IS_NOT_NULL(*argLeftChannel);
        }

        if (nullptr != argRightChannel)
        {
            VERIFY_IS_NOT_NULL(*argRightChannel);
        }
    };

    UINT framesCaptured = 0;
    CComPtr<ID2D1Bitmap1> leftChannel;
    CComPtr<ID2D1Bitmap1> rightChannel;

    doCapture(&framesCaptured, &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
}