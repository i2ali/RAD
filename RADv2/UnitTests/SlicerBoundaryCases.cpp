//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "SlicerBoundaryCases.h"

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool SlicerBoundaryCases::ClassSetup(void)
{
    this->mModuleHandle = ::GetModuleHandleW(nullptr);
    DWORD processId = ::GetCurrentProcessId();
    DWORD threadId = ::GetCurrentThreadId();

    String className = String().Format(L"DwmSlicerControlWindow<%d-%d-%d>", processId, threadId, reinterpret_cast<UINT>(this));

    WNDCLASSEXW wndClass;
    wndClass.cbClsExtra = 0;
    wndClass.cbSize = sizeof(wndClass);
    wndClass.cbWndExtra = 0;
    wndClass.hbrBackground = nullptr;
    wndClass.hCursor = nullptr;
    wndClass.hIcon = nullptr;
    wndClass.hIconSm = nullptr;
    wndClass.hInstance = this->mModuleHandle;
    wndClass.lpfnWndProc = ::DefWindowProcW;
    wndClass.lpszClassName = className.GetBuffer();
    wndClass.lpszMenuName = nullptr;
    wndClass.style = 0;

    this->mWindowClass = ::RegisterClassExW(&wndClass);
    VERIFY_ARE_NOT_EQUAL(0, this->mWindowClass);

    this->mWindowHandle = ::CreateWindowExW(
        0,
        MAKEINTATOM(this->mWindowClass),
        L"DwmSlicerControlWindow",
        0,
        0,
        0,
        64,
        64,
        nullptr,
        nullptr,
        this->mModuleHandle,
        nullptr);

    VERIFY_IS_NOT_NULL(this->mWindowHandle);

    return true;
}


bool SlicerBoundaryCases::ClassCleanup(void)
{
    if (nullptr != this->mWindowHandle)
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(::DestroyWindow(this->mWindowHandle));
        this->mWindowHandle = nullptr;
    }

    if (0 != this->mWindowClass)
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(::UnregisterClassW(MAKEINTATOM(this->mWindowClass), this->mModuleHandle));
        this->mWindowClass = 0;
    }

    return true;
}


bool SlicerBoundaryCases::MethodSetup(void)
{
    this->mSlicing = false;
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, ::SlicerContextInitialize, nullptr, 0));
    return true;
}


bool SlicerBoundaryCases::MethodCleanup(void)
{
    if (true == this->mSlicing)
    {
        SLICER_CAPTURE_RESULT captureResult = {};
        ::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult));
        this->mSlicing = false;
    }

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
    return true;
}


void SlicerBoundaryCases::CaptureOnTheCornersOfThePrimaryDisplay(void)
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
        // Get the outputs that intersect this caputre rect
        auto outputMap = ::GetOutputsFromScreenRect(rects[current]);

        UINT textureWidth = rects[current].right - rects[current].left;
        UINT textureHeight = rects[current].bottom - rects[current].top;
        auto textures = ::CreateTexturesForOutputs(outputMap, textureWidth, textureHeight);

        // Set the textures
        auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[textures.size()]);
        SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

        // Fill out the texture information
        sharedTextures->Width = textureWidth;
        sharedTextures->Height = textureHeight;
        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTextures->cSharedTextures = textures.size();

        int i = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter)
        {
            sharedTextures->Textures[i++] = iter->second;
        }

        // Set the destination shared textures
        BOOL result = ::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize);

        ::free(sharedTextures);

        VERIFY_WIN32_BOOL_SUCCEEDED(result, L"::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize)");

        // Start capturing
        SLICER_CAPTURE_PARAM captureParams;
        captureParams.cFramesToSkip = 0;
        captureParams.cMaxFrames = 1;
        captureParams.rcCapture.bottom = rects[current].bottom;
        captureParams.rcCapture.left = rects[current].left;
        captureParams.rcCapture.right = rects[current].right;
        captureParams.rcCapture.top = rects[current].top;

        // Start capturing
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
        this->mSlicing = true;

        ::Sleep(500);

        // End capturing
        SLICER_CAPTURE_RESULT captureResult = {};
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
        this->mSlicing = false;

        VERIFY_SUCCEEDED(captureResult.CaptureResult);

        Log::Comment(String().Format(L"Frames captured: %d", captureResult.cFramesCaptured));
    }
}


void SlicerBoundaryCases::CaptureOneByOneInside(void)
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
        // Get the outputs that intersect this caputre rect
        auto outputMap = ::GetOutputsFromScreenRect(rects[current]);

        UINT textureWidth = rects[current].right - rects[current].left;
        UINT textureHeight = rects[current].bottom - rects[current].top;
        auto textures = ::CreateTexturesForOutputs(outputMap, textureWidth, textureHeight);

        // Set the textures
        auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[textures.size()]);
        SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

        // Fill out the texture information
        sharedTextures->Width = textureWidth;
        sharedTextures->Height = textureHeight;
        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTextures->cSharedTextures = textures.size();

        int i = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter)
        {
            sharedTextures->Textures[i++] = iter->second;
        }

        // Set the destination shared textures
        BOOL result = ::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize);

        ::free(sharedTextures);

        VERIFY_WIN32_BOOL_SUCCEEDED(result, L"::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize)");

        // Start capturing
        SLICER_CAPTURE_PARAM captureParams;
        captureParams.cFramesToSkip = 0;
        captureParams.cMaxFrames = 1;
        captureParams.rcCapture.bottom = rects[current].bottom;
        captureParams.rcCapture.left = rects[current].left;
        captureParams.rcCapture.right = rects[current].right;
        captureParams.rcCapture.top = rects[current].top;

        // Start capturing
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
        this->mSlicing = true;

        ::Sleep(500);

        // End capturing
        SLICER_CAPTURE_RESULT captureResult = {};
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
        this->mSlicing = false;

        VERIFY_SUCCEEDED(captureResult.CaptureResult);

        Log::Comment(String().Format(L"Frames captured: %d", captureResult.cFramesCaptured));
    }
}


void SlicerBoundaryCases::CaptureOneByOneOutside(void)
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
        // Get the outputs that intersect this caputre rect
        auto outputMap = ::GetOutputsFromScreenRect(rects[current]);

        UINT textureWidth = rects[current].right - rects[current].left;
        UINT textureHeight = rects[current].bottom - rects[current].top;
        auto textures = ::CreateTexturesForOutputs(outputMap, textureWidth, textureHeight);

        // Set the textures
        auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[textures.size()]);
        SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

        // Fill out the texture information
        sharedTextures->Width = textureWidth;
        sharedTextures->Height = textureHeight;
        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTextures->cSharedTextures = textures.size();

        int i = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter)
        {
            sharedTextures->Textures[i++] = iter->second;
        }

        // Set the destination shared textures
        BOOL result = ::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize);

        ::free(sharedTextures);

        VERIFY_WIN32_BOOL_SUCCEEDED(result, L"::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize)");

        // Start capturing
        SLICER_CAPTURE_PARAM captureParams;
        captureParams.cFramesToSkip = 0;
        captureParams.cMaxFrames = 1;
        captureParams.rcCapture.bottom = rects[current].bottom;
        captureParams.rcCapture.left = rects[current].left;
        captureParams.rcCapture.right = rects[current].right;
        captureParams.rcCapture.top = rects[current].top;

        // Start capturing
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
        this->mSlicing = true;

        ::Sleep(500);

        // End capturing
        SLICER_CAPTURE_RESULT captureResult = {};
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
        this->mSlicing = false;

        VERIFY_SUCCEEDED(captureResult.CaptureResult);

        Log::Comment(String().Format(L"Frames captured: %d", captureResult.cFramesCaptured));
    }
}


void SlicerBoundaryCases::CaptureFullPrimaryOutput(void)
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
        // Get the outputs that intersect this caputre rect
        auto outputMap = ::GetOutputsFromScreenRect(rects[current]);

        UINT textureWidth = rects[current].right - rects[current].left;
        UINT textureHeight = rects[current].bottom - rects[current].top;
        auto textures = ::CreateTexturesForOutputs(outputMap, textureWidth, textureHeight);

        // Set the textures
        auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[textures.size()]);
        SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

        // Fill out the texture information
        sharedTextures->Width = textureWidth;
        sharedTextures->Height = textureHeight;
        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTextures->cSharedTextures = textures.size();

        int i = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter)
        {
            sharedTextures->Textures[i++] = iter->second;
        }

        // Set the destination shared textures
        BOOL result = ::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize);

        ::free(sharedTextures);

        VERIFY_WIN32_BOOL_SUCCEEDED(result, L"::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize)");

        // Start capturing
        SLICER_CAPTURE_PARAM captureParams;
        captureParams.cFramesToSkip = 0;
        captureParams.cMaxFrames = 1;
        captureParams.rcCapture.bottom = rects[current].bottom;
        captureParams.rcCapture.left = rects[current].left;
        captureParams.rcCapture.right = rects[current].right;
        captureParams.rcCapture.top = rects[current].top;

        // Start capturing
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
        this->mSlicing = true;

        ::Sleep(500);

        // End capturing
        SLICER_CAPTURE_RESULT captureResult = {};
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
        this->mSlicing = false;

        VERIFY_SUCCEEDED(captureResult.CaptureResult);

        Log::Comment(String().Format(L"Frames captured: %d", captureResult.cFramesCaptured));
    }
}


void SlicerBoundaryCases::CaptureInflatedFullPrimaryOutput(void)
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
        // Get the outputs that intersect this caputre rect
        auto outputMap = ::GetOutputsFromScreenRect(rects[current]);

        UINT textureWidth = rects[current].right - rects[current].left;
        UINT textureHeight = rects[current].bottom - rects[current].top;
        auto textures = ::CreateTexturesForOutputs(outputMap, textureWidth, textureHeight);

        // Set the textures
        auto sharedTextureSize = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures[textures.size()]);
        SLICER_SHARED_TEXTURES* sharedTextures = (SLICER_SHARED_TEXTURES*)::malloc(sharedTextureSize);

        // Fill out the texture information
        sharedTextures->Width = textureWidth;
        sharedTextures->Height = textureHeight;
        sharedTextures->PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        sharedTextures->cSharedTextures = textures.size();

        int i = 0;
        for (auto iter = textures.begin(); iter != textures.end(); ++iter)
        {
            sharedTextures->Textures[i++] = iter->second;
        }

        // Set the destination shared textures
        BOOL result = ::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize);

        ::free(sharedTextures);

        VERIFY_WIN32_BOOL_SUCCEEDED(result, L"::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, sharedTextures, sharedTextureSize)");

        // Start capturing
        SLICER_CAPTURE_PARAM captureParams;
        captureParams.cFramesToSkip = 0;
        captureParams.cMaxFrames = 1;
        captureParams.rcCapture.bottom = rects[current].bottom;
        captureParams.rcCapture.left = rects[current].left;
        captureParams.rcCapture.right = rects[current].right;
        captureParams.rcCapture.top = rects[current].top;

        // Start capturing
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
        this->mSlicing = true;

        ::Sleep(500);

        // End capturing
        SLICER_CAPTURE_RESULT captureResult = {};
        VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
        this->mSlicing = false;

        VERIFY_SUCCEEDED(captureResult.CaptureResult);

        Log::Comment(String().Format(L"Frames captured: %d", captureResult.cFramesCaptured));
    }
}