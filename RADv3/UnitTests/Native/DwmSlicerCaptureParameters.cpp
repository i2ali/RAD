//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicerCaptureParameters.h"

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool DwmSlicerCaptureParameters::ClassSetup(void)
{
    VERIFY_SUCCEEDED(::CreateSlicerFactory(&this->mSlicerFactory));
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DContext(&this->mD2DContext));
    VERIFY_SUCCEEDED(this->mSlicerFactory->CreateDwmSlicer(&this->mSlicer));

    return true;
}


bool DwmSlicerCaptureParameters::ClassCleanup(void)
{
    return true;
}


bool DwmSlicerCaptureParameters::MethodSetup(void)
{
    return true;
}


bool DwmSlicerCaptureParameters::MethodCleanup(void)
{
    return true;
}


void DwmSlicerCaptureParameters::CaptureLeftChannel(void)
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
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftChannel-1", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftChannel-2", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftChannel-3", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftChannel-4", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
}


void DwmSlicerCaptureParameters::CaptureRightChannel(void)
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
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureRightChannel-1", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureRightChannel-2", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureRightChannel-3", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureRightChannel-4", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
}


void DwmSlicerCaptureParameters::CaptureLeftAndRightChannel(void)
{
    // Windows 8 Bugs #11553
    // Won't fix for Windows 8; fix needed for Windows 9 MQ
    // TEST - Uncomment this code when the bug is fixed
    /*
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
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Left-1", leftChannel, RAD::ImageContainerFormat::Png));
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Right-1", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, &leftChannel, nullptr);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Left-2", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Right-3", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(&framesCaptured, nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Left-4", leftChannel, RAD::ImageContainerFormat::Png));
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Right-4", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         &leftChannel, nullptr);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Left-5", leftChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      &rightChannel);
    VERIFY_SUCCEEDED(this->mD2DContext->SaveImageToFile(L"CaptureLeftAndRightChannel-Right-6", rightChannel, RAD::ImageContainerFormat::Png));
    leftChannel = rightChannel = nullptr;

    doCapture(nullptr,         nullptr,      nullptr);
    leftChannel = rightChannel = nullptr;
    */
}