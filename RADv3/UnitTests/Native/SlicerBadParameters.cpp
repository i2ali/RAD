//-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//-----------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"
#include "SlicerBadParameters.h"

using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;


bool SlicerBadParameters::ClassSetup(void)
{
    HRESULT hResult;

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

    this->mWindowHandleMessageOnly = ::CreateWindowExW(
        0,
        MAKEINTATOM(this->mWindowClass),
        L"DwmSlicerControlWindowMessageOnly",
        0,
        0,
        0,
        64,
        64,
        HWND_MESSAGE,
        nullptr,
        this->mModuleHandle,
        nullptr);

    VERIFY_IS_NOT_NULL(this->mWindowHandleMessageOnly);

    UINT frameWidth = 100;
    UINT frameHeight = 100;
    this->mLargeTextureWidth = 1000;
    this->mLargeTextureHeight = 1000;
    this->mSmallTextureWidth = 50;
    this->mSmallTextureHeight = 50;

    auto primaryOutput = ::GetPrimaryOutput();
    DXGI_OUTPUT_DESC outputDesc = {};
    primaryOutput->GetDesc(&outputDesc);

    // The capture rect
    this->mCaptureRect.top = outputDesc.DesktopCoordinates.top;
    this->mCaptureRect.left = outputDesc.DesktopCoordinates.left;
    this->mCaptureRect.bottom = outputDesc.DesktopCoordinates.top + frameHeight;
    this->mCaptureRect.right = outputDesc.DesktopCoordinates.left + frameWidth;

    auto outputs = ::GetOutputsFromScreenRect(this->mCaptureRect);
    this->mLargeTexture = ::CreateTexturesForOutputs(outputs, this->mLargeTextureWidth, this->mLargeTextureHeight);
    this->mSmallTexture = ::CreateTexturesForOutputs(outputs, this->mSmallTextureWidth, this->mSmallTextureHeight);

    return true;
}


bool SlicerBadParameters::ClassCleanup(void)
{
    if (nullptr != this->mWindowHandleMessageOnly)
    {
        VERIFY_WIN32_BOOL_SUCCEEDED(::DestroyWindow(this->mWindowHandleMessageOnly));
        this->mWindowHandleMessageOnly = nullptr;
    }

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


///
/// Pre and post test
///

bool SlicerBadParameters::TestSetup(void)
{
    return true;
}


bool SlicerBadParameters::TestCleanup(void)
{
    // Deinitialize always
    ::SlicerControl(this->mWindowHandleMessageOnly, SlicerContextDeinitialize, NULL, 0);
    ::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, NULL, 0);

    return true;
}


///
/// Bad SLICER_COMMAND
///

void SlicerBadParameters::SlicerContext_BadCommand(void)
{
    SLICER_COMMAND badSlicerCommand = static_cast<SLICER_COMMAND>(0x87654321);
    HWND badWindowHandle = reinterpret_cast<HWND>(0x87654321);

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(badWindowHandle, badSlicerCommand, nullptr, 0));
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, badSlicerCommand, nullptr, 0));
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandleMessageOnly, badSlicerCommand, nullptr, 0));
}


///
/// Plain initialize and deinitialize
///

void SlicerBadParameters::SlicerContext_InitializeAndDeinitialize(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


///
/// SlicerContextInitialize Commands
///

void SlicerBadParameters::SlicerContextInitialize_BadWindowHandle(void)
{
    HWND badWindowHandle = reinterpret_cast<HWND>(0x87654321);
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(badWindowHandle, SlicerContextInitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextInitialize_BadVariableArguments(void)
{
    UINT badParameters = 1234567890;
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, &badParameters, sizeof(badParameters)));
}


void SlicerBadParameters::SlicerContextInitialize_MessageOnlyWindow(void)
{
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandleMessageOnly, SlicerContextInitialize, nullptr, 0));
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandleMessageOnly, SlicerContextDeinitialize, nullptr, 0));
}


///
/// SlicerContextDeinitialize Commands
///

void SlicerBadParameters::SlicerContextDeinitialize_BadWindowHandle(void)
{
    HWND badWindowHandle = reinterpret_cast<HWND>(0x87654321);
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(badWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextDeinitialize_BadVariableArguments(void)
{
    UINT badParameters = 1234567890;

    // Test it before an initialize
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, &badParameters, sizeof(badParameters)));

    // Initialize normally and then test deinitialize with bad arguments
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, &badParameters, sizeof(badParameters)));
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextDeinitialize_BeforeInitialize(void)
{
    // A good deinitialize command before an initialize
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


///
/// SlicerContextSetDestinationTextures Commands
///

void SlicerBadParameters::SlicerContextSetDestinationTextures_BeforeInitialize(void)
{
    SLICER_SHARED_TEXTURES sharedTextures = {};
    UINT sharedTextureSize = sizeof(sharedTextures);
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureSize));
}


void SlicerBadParameters::SlicerContextSetDestinationTextures_BadWindowHandle(void)
{
    HWND badWindowHandle = reinterpret_cast<HWND>(0x87654321);
    SLICER_SHARED_TEXTURES sharedTextures = {};
    UINT sharedTextureSize = sizeof(sharedTextures);
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(badWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureSize));
}


void SlicerBadParameters::SlicerContextSetDestinationTextures_BadVariableArguments(void)
{
    const UINT sharedTextureSize = sizeof(SLICER_SHARED_TEXTURES);
    const UINT sharedTextureFieldOffset = FIELD_OFFSET(SLICER_SHARED_TEXTURES, Textures);

    SLICER_SHARED_TEXTURES sharedTextures = {};
    sharedTextures.Width = 151;
    sharedTextures.Height = 743;
    sharedTextures.PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextures.cSharedTextures = MAG_MAX_ADAPTER + 1;
    sharedTextures.Textures[0].AdapterLuid.HighPart = 0xFFEEEEDD;
    sharedTextures.Textures[0].AdapterLuid.LowPart = 0x12345678;
    sharedTextures.Textures[0].hmonAssoc = reinterpret_cast<HMONITOR>(0x11223344);
    sharedTextures.Textures[0].hDxSharedTexture = reinterpret_cast<HANDLE>(0x55667788);

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, (void*)(0x21321321), sharedTextureSize));

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureFieldOffset - 1));

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureSize));

    sharedTextures.cSharedTextures = MAG_MAX_ADAPTER - 1;

    // Windows 8 Bugs #684677
    // Won't fix for Windows 8; fix needed for Windows 9 MQ
    // TEST - Uncomment this code when the bug is fixed
    //VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureSize));
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sharedTextureSize));

    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = this->mCaptureRect.bottom;
    captureParams.rcCapture.left = this->mCaptureRect.left;
    captureParams.rcCapture.right = this->mCaptureRect.right;
    captureParams.rcCapture.top = this->mCaptureRect.top;

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
    Log::Comment(String().Format(L"GLE: 0n%d", ::GetLastError()));

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


///
/// SlicerContextBeginCapture Commands
///

void SlicerBadParameters::SlicerContextBeginCapture_BeforeInitialize(void)
{
    // Set the capture parameters
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = 100;
    captureParams.rcCapture.left = 0;
    captureParams.rcCapture.right = 100;
    captureParams.rcCapture.top = 0;

    // Start capturing
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
}


void SlicerBadParameters::SlicerContextBeginCapture_BeforeSetDestinationTextures(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    // Set the capture parameters
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = 100;
    captureParams.rcCapture.left = 0;
    captureParams.rcCapture.right = 100;
    captureParams.rcCapture.top = 0;

    // Start capturing
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextBeginCapture_BadWindowHandle(void)
{
    // Set the capture parameters
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = 100;
    captureParams.rcCapture.left = 0;
    captureParams.rcCapture.right = 100;
    captureParams.rcCapture.top = 0;

    // Start capturing
    HWND badWindowHandle = reinterpret_cast<HWND>(0x87654321);
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(badWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));
}


void SlicerBadParameters::SlicerContextBeginCapture_BadVariableArguments(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    // Set textures
    SLICER_SHARED_TEXTURES sharedTextures = {};
    sharedTextures.Width = this->mLargeTextureWidth;
    sharedTextures.Height = this->mLargeTextureHeight;
    sharedTextures.PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextures.cSharedTextures = 1;
    sharedTextures.Textures[0] = this->mLargeTexture.begin()->second;

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sizeof(sharedTextures)));

    // Set the capture parameters
    // Pass in a negative capture rect
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = this->mCaptureRect.top;
    captureParams.rcCapture.left = this->mCaptureRect.right;
    captureParams.rcCapture.right = this->mCaptureRect.left;
    captureParams.rcCapture.top = this->mCaptureRect.bottom;

    // Start capturing
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, reinterpret_cast<void*>(0xAABBCCDD), sizeof(captureParams)));

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


///
/// SlicerContextEndCapture Commands
///

void SlicerBadParameters::SlicerContextEndCapture_BeforeInitialize(void)
{
    SLICER_CAPTURE_RESULT captureResult = {};
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
}


void SlicerBadParameters::SlicerContextEndCapture_BeforeBeginCapture(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    // Set textures
    SLICER_SHARED_TEXTURES sharedTextures = {};
    sharedTextures.Width = this->mLargeTextureWidth;
    sharedTextures.Height = this->mLargeTextureHeight;
    sharedTextures.PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextures.cSharedTextures = 1;
    sharedTextures.Textures[0] = this->mLargeTexture.begin()->second;

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sizeof(sharedTextures)));

    // Windows 8 Bugs #42145 - By Design
    // Calling SlicerControl(SlicerContextEndCapture) before
    // SlicerControl(SlicerContextBeginCapture) will succeed
    SLICER_CAPTURE_RESULT captureResult = {};
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
    VERIFY_ARE_EQUAL(0, captureResult.cFramesCaptured);

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextEndCapture_BadVariableArguments(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    // Set textures
    SLICER_SHARED_TEXTURES sharedTextures = {};
    sharedTextures.Width = this->mLargeTextureWidth;
    sharedTextures.Height = this->mLargeTextureHeight;
    sharedTextures.PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextures.cSharedTextures = 1;
    sharedTextures.Textures[0] = this->mLargeTexture.begin()->second;

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sizeof(sharedTextures)));

    // Set the capture parameters
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 1;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = this->mCaptureRect.bottom;
    captureParams.rcCapture.left = this->mCaptureRect.left;
    captureParams.rcCapture.right = this->mCaptureRect.right;
    captureParams.rcCapture.top = this->mCaptureRect.top;

    // Start capturing
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));

    SLICER_CAPTURE_RESULT captureResult = {};
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, reinterpret_cast<void*>(0xEFEFEFEF), sizeof(captureResult)));

    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult) - 10));

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
}


void SlicerBadParameters::SlicerContextEndCapture_BadTextureSize(void)
{
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextInitialize, nullptr, 0));

    // Set textures
    SLICER_SHARED_TEXTURES sharedTextures = {};
    sharedTextures.Width = this->mLargeTextureWidth;
    sharedTextures.Height = this->mLargeTextureHeight;
    sharedTextures.PixelFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextures.cSharedTextures = 1;
    sharedTextures.Textures[0] = this->mSmallTexture.begin()->second;

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextSetDestinationTextures, &sharedTextures, sizeof(sharedTextures)));

    // Set the capture parameters
    SLICER_CAPTURE_PARAM captureParams;
    captureParams.cFramesToSkip = 0;
    captureParams.cMaxFrames = 60;
    captureParams.rcCapture.bottom = this->mCaptureRect.bottom;
    captureParams.rcCapture.left = this->mCaptureRect.left;
    captureParams.rcCapture.right = this->mCaptureRect.right;
    captureParams.rcCapture.top = this->mCaptureRect.top;

    // Start capturing
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, reinterpret_cast<void*>(0xAABBCCDD), sizeof(captureParams)));

    // Windows 8 Bugs #680052
    // Won't fix for Windows 8; fix needed for Windows 9 MQ
    // TEST - Uncomment this code when the bug is fixed
    /*
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextBeginCapture, &captureParams, sizeof(captureParams)));

    ::Sleep(1000);

    SLICER_CAPTURE_RESULT captureResult = {};
    VERIFY_WIN32_BOOL_FAILED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));
    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextEndCapture, &captureResult, sizeof(captureResult)));

    Log::Comment(String().Format(L"Frames Captured: %d", captureResult.cFramesCaptured));

    VERIFY_WIN32_BOOL_SUCCEEDED(::SlicerControl(this->mWindowHandle, SlicerContextDeinitialize, nullptr, 0));
    */
}