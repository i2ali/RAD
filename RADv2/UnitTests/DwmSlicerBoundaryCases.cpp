//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicerBoundaryCases.h"

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool DwmSlicerBoundaryCases::ClassSetup(void)
{
    VERIFY_SUCCEEDED(::CreateSlicerFactory(&this->mSlicerFactory));
    VERIFY_SUCCEEDED(this->mSlicerFactory->CreateDwmSlicer(&this->mSlicer));

    return true;
}


bool DwmSlicerBoundaryCases::ClassCleanup(void)
{
    return true;
}


bool DwmSlicerBoundaryCases::MethodSetup(void)
{
    return true;
}


bool DwmSlicerBoundaryCases::MethodCleanup(void)
{
    return true;
}


void DwmSlicerBoundaryCases::CaptureOnTheCornersOfThePrimaryDisplay(void)
{
    // Get the coordinates of the primary display
    auto primaryOutput = ::GetPrimaryOutput();

    DXGI_OUTPUT_DESC desc = {};
    primaryOutput->GetDesc(&desc);

    RECT topLeft;
    topLeft.top =    desc.DesktopCoordinates.top - 50;
    topLeft.left =   desc.DesktopCoordinates.left - 50;
    topLeft.bottom = desc.DesktopCoordinates.top + 50;
    topLeft.right =  desc.DesktopCoordinates.left + 50;

    RECT topRight;
    topRight.top =    desc.DesktopCoordinates.top - 50;
    topRight.left =   desc.DesktopCoordinates.right - 50;
    topRight.bottom = desc.DesktopCoordinates.top + 50;
    topRight.right =  desc.DesktopCoordinates.right + 50;

    RECT bottomLeft;
    bottomLeft.top =    desc.DesktopCoordinates.bottom - 50;
    bottomLeft.left =   desc.DesktopCoordinates.left - 50;
    bottomLeft.bottom = desc.DesktopCoordinates.bottom + 50;
    bottomLeft.right =  desc.DesktopCoordinates.left + 50;

    RECT bottomRight;
    bottomRight.top =    desc.DesktopCoordinates.bottom - 50;
    bottomRight.left =   desc.DesktopCoordinates.right - 50;
    bottomRight.bottom = desc.DesktopCoordinates.bottom + 50;
    bottomRight.right =  desc.DesktopCoordinates.right + 50;

    RECT rects[] =
    {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };

    for (int current = 0; current < ARRAYSIZE(rects); ++current)
    {
        // Non-stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::Left, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }

        // Windows 8 Bugs #11553
        // Won't fix for Windows 8; fix needed for Windows 9 MQ
        // TEST - Uncomment this code when the bug is fixed
        /*
        // Stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }
        */
    }
}


void DwmSlicerBoundaryCases::CaptureOneByOneInside(void)
{
    // Get the coordinates of the primary display
    auto primaryOutput = ::GetPrimaryOutput();

    DXGI_OUTPUT_DESC desc = {};
    primaryOutput->GetDesc(&desc);

    RECT topLeft;
    topLeft.top =    desc.DesktopCoordinates.top;
    topLeft.left =   desc.DesktopCoordinates.left;
    topLeft.bottom = desc.DesktopCoordinates.top + 1;
    topLeft.right =  desc.DesktopCoordinates.left + 1;

    RECT topRight;
    topRight.top =    desc.DesktopCoordinates.top;
    topRight.left =   desc.DesktopCoordinates.right - 1;
    topRight.bottom = desc.DesktopCoordinates.top + 1;
    topRight.right =  desc.DesktopCoordinates.right;

    RECT bottomLeft;
    bottomLeft.top =    desc.DesktopCoordinates.bottom - 1;
    bottomLeft.left =   desc.DesktopCoordinates.left;
    bottomLeft.bottom = desc.DesktopCoordinates.bottom;
    bottomLeft.right =  desc.DesktopCoordinates.left + 1;

    RECT bottomRight;
    bottomRight.top =    desc.DesktopCoordinates.bottom - 1;
    bottomRight.left =   desc.DesktopCoordinates.right - 1;
    bottomRight.bottom = desc.DesktopCoordinates.bottom;
    bottomRight.right =  desc.DesktopCoordinates.right;

    RECT rects[] =
    {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };

    for (int current = 0; current < ARRAYSIZE(rects); ++current)
    {
        // Non-stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::Left, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }

        // Windows 8 Bugs #11553
        // Won't fix for Windows 8; fix needed for Windows 9 MQ
        // TEST - Uncomment this code when the bug is fixed
        /*
        // Stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }
        */
    }
}


void DwmSlicerBoundaryCases::CaptureOneByOneOutside(void)
{
    // Get the coordinates of the primary display
    auto primaryOutput = ::GetPrimaryOutput();

    DXGI_OUTPUT_DESC desc = {};
    primaryOutput->GetDesc(&desc);

    RECT topLeft;
    topLeft.top =    desc.DesktopCoordinates.top - 1;
    topLeft.left =   desc.DesktopCoordinates.left - 1;
    topLeft.bottom = desc.DesktopCoordinates.top;
    topLeft.right =  desc.DesktopCoordinates.left;

    RECT topRight;
    topRight.top =    desc.DesktopCoordinates.top - 1;
    topRight.left =   desc.DesktopCoordinates.right;
    topRight.bottom = desc.DesktopCoordinates.top;
    topRight.right =  desc.DesktopCoordinates.right + 1;

    RECT bottomLeft;
    bottomLeft.top =    desc.DesktopCoordinates.bottom;
    bottomLeft.left =   desc.DesktopCoordinates.left - 1;
    bottomLeft.bottom = desc.DesktopCoordinates.bottom + 1;
    bottomLeft.right =  desc.DesktopCoordinates.left;

    RECT bottomRight;
    bottomRight.top =    desc.DesktopCoordinates.bottom;
    bottomRight.left =   desc.DesktopCoordinates.right;
    bottomRight.bottom = desc.DesktopCoordinates.bottom + 1;
    bottomRight.right =  desc.DesktopCoordinates.right + 1;

    RECT rects[] =
    {
        topLeft,
        topRight,
        bottomLeft,
        bottomRight,
    };

    for (int current = 0; current < ARRAYSIZE(rects); ++current)
    {
        // Non-stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_FAILED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::Left, &numberOfFramesThatCanBeCaptured));
        }

        // Stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_FAILED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
        }
    }
}


void DwmSlicerBoundaryCases::CaptureFullPrimaryOutput(void)
{
    // Get the coordinates of the primary display
    auto primaryOutput = ::GetPrimaryOutput();

    DXGI_OUTPUT_DESC desc = {};
    primaryOutput->GetDesc(&desc);

    RECT topLeft;
    topLeft.top =    desc.DesktopCoordinates.top;
    topLeft.left =   desc.DesktopCoordinates.left;
    topLeft.bottom = desc.DesktopCoordinates.bottom;
    topLeft.right =  desc.DesktopCoordinates.right;

    RECT rects[] =
    {
        topLeft,
    };

    for (int current = 0; current < ARRAYSIZE(rects); ++current)
    {
        // Non-stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::Left, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }

        // Windows 8 Bugs #11553
        // Won't fix for Windows 8; fix needed for Windows 9 MQ
        // TEST - Uncomment this code when the bug is fixed
        /*
        // Stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }
        */
    }
}


void DwmSlicerBoundaryCases::CaptureInflatedFullPrimaryOutput(void)
{
    // Get the coordinates of the primary display
    auto primaryOutput = ::GetPrimaryOutput();

    DXGI_OUTPUT_DESC desc = {};
    primaryOutput->GetDesc(&desc);

    RECT topLeft;
    topLeft.top =    desc.DesktopCoordinates.top - 1;
    topLeft.left =   desc.DesktopCoordinates.left - 1;
    topLeft.bottom = desc.DesktopCoordinates.bottom + 1;
    topLeft.right =  desc.DesktopCoordinates.right + 1;

    RECT rects[] =
    {
        topLeft,
    };

    for (int current = 0; current < ARRAYSIZE(rects); ++current)
    {
        // Non-stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::Left, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }

        // Windows 8 Bugs #11553
        // Won't fix for Windows 8; fix needed for Windows 9 MQ
        // TEST - Uncomment this code when the bug is fixed
        /*
        // Stereo
        {
            UINT numberOfFramesThatCanBeCaptured = 0;
            VERIFY_SUCCEEDED(this->mSlicer->StartCapture(rects[current], 1, 0, RADv2::Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
            Log::Comment(String().Format(L"Number of frames that can be captured: %d", numberOfFramesThatCanBeCaptured));

            ::Sleep(500);

            UINT framesCaptured = 0;
            CComPtr<ID2D1Bitmap1> leftChannel;
            CComPtr<ID2D1Bitmap1> rightChannel;
            VERIFY_SUCCEEDED(this->mSlicer->EndCapture(&framesCaptured, &leftChannel, &rightChannel));

            Log::Comment(String().Format(L"Frames captured: %d", framesCaptured));
        }
        */
    }
}