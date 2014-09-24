//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Test app for the slicer interface.
//
//------------------------------------------------------------------------------

// Slicer headers
#include <SlicerInterface.h>

// Built in effects
#include <initguid.h>
#include <D2D1Effects.h>

// Custom effects
#include <RADv2Effects.h>

// STL
#include <iostream>
#include <string>

// ATL
#include <atlbase.h>

// WIC
#include <wincodec.h>

using namespace RADv2;

#define IFC(expr) do { hResult = (expr); if (FAILED(hResult)) { goto Cleanup; } } while (0, 0)
#define IFCNULL(expr) do { if (nullptr == (expr)) { hResult = E_INVALIDARG; goto Cleanup; } } while (0, 0)

int _cdecl wmain(int argc, __in wchar_t* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    HRESULT hResult;

    CComPtr<ISlicerFactory> slicerFactory;
    CComPtr<IDwmSlicer>     dwmSlicer;
    CComPtr<ID2D1Bitmap1>   captureResultLeftChannel;
    CComPtr<ID2D1Bitmap1>   captureResultRightChannel;
    CComPtr<ID2D1Bitmap1>   frame;

    UINT framesCaptured = 0;

    // Initialize COM
    IFC(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    // Create a slicer factory
    std::wcout << L"CreateSlicerFactory()...";
    IFC(::CreateSlicerFactory(&slicerFactory, ComputeShaderModel::None));
    std::wcout << L" SUCCEEDED" << std::endl;
    std::wcout << L"D2D Feature Level: 0x" << std::hex << slicerFactory->GetD2DFeatureLevel() << std::dec << std::endl;

    std::wcout << L"CreateDwmSlicer()...";
    IFC(slicerFactory->CreateDwmSlicer(&dwmSlicer));
    std::wcout << L" SUCCEEDED" << std::endl;

    UINT numberOfFramesThatCanBeCaptured = 0;

    {
        std::wcout << L"StartCapture()...";
        IFC(dwmSlicer->StartCapture(RADv2::Rect(0, 0, 100, 100), 60, 1));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

        std::wcout << L"Sleeping...";
        ::Sleep(1500);
        std::wcout << L" Done" << std::endl;

        std::wcout << L"EndCapture()...";
        IFC(dwmSlicer->EndCapture(&framesCaptured, &captureResultLeftChannel));
        std::wcout << L" SUCCEEDED" << std::endl;
        std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

        std::wcout << L"SaveImageToFile()...";
        IFC(slicerFactory->SaveImageToFile(L"captureResultLeftChannel-1.png", captureResultLeftChannel, GUID_ContainerFormatPng));
        std::wcout << L" SUCCEEDED" << std::endl;
    }

    {
        std::wcout << L"StartCapture()...";
        IFC(dwmSlicer->StartCapture(RADv2::Rect(100, 100, 200, 200), 67834, 1, Channel::Left, &numberOfFramesThatCanBeCaptured));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

        std::wcout << L"Sleeping...";
        ::Sleep(1500);
        std::wcout << L" Done" << std::endl;

        std::wcout << L"EndCapture()...";
        IFC(dwmSlicer->EndCapture(&framesCaptured, &captureResultLeftChannel));
        std::wcout << L" SUCCEEDED" << std::endl;
        std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

        std::wcout << L"SaveImageToFile()...";
        IFC(slicerFactory->SaveImageToFile(L"captureResultLeftChannel-2.png", captureResultLeftChannel, GUID_ContainerFormatPng));
        std::wcout << L" SUCCEEDED" << std::endl;
    }

    {
        std::wcout << L"StartCapture()...";
        IFC(dwmSlicer->StartCapture(RADv2::Rect(900, 0, 1200, 100), 300, 0, Channel::Left, &numberOfFramesThatCanBeCaptured));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

        std::wcout << L"Sleeping...";
        ::Sleep(1500);
        std::wcout << L" Done" << std::endl;

        std::wcout << L"EndCapture()...";
        IFC(dwmSlicer->EndCapture(&framesCaptured, &captureResultLeftChannel));
        std::wcout << L" SUCCEEDED" << std::endl;
        std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

        std::wcout << L"SaveImageToFile()...";
        IFC(slicerFactory->SaveImageToFile(L"captureResultLeftChannel-3.png", captureResultLeftChannel, GUID_ContainerFormatPng));
        std::wcout << L" SUCCEEDED" << std::endl;
    }

    {
        std::wcout << L"StartCapture()...";
        IFC(dwmSlicer->StartCapture(RADv2::Rect(1600, 0, 1800, 100), 60, 0, Channel::Right, &numberOfFramesThatCanBeCaptured));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

        std::wcout << L"Sleeping...";
        ::Sleep(1500);
        std::wcout << L" Done" << std::endl;

        std::wcout << L"EndCapture()...";
        IFC(dwmSlicer->EndCapture(&framesCaptured, &captureResultLeftChannel));
        std::wcout << L" SUCCEEDED" << std::endl;
        std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

        std::wcout << L"SaveImageToFile()...";
        IFC(slicerFactory->SaveImageToFile(L"captureResultLeftChannel-4.png", captureResultLeftChannel, GUID_ContainerFormatPng));
        std::wcout << L" SUCCEEDED" << std::endl;
    }

    {
        std::wcout << L"StartCapture()...";
        IFC(dwmSlicer->StartCapture(RADv2::Rect(200, 200, 300, 300), 120, 1, Channel::LeftAndRight, &numberOfFramesThatCanBeCaptured));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

        std::wcout << L"Sleeping...";
        ::Sleep(1500);
        std::wcout << L" Done" << std::endl;

        std::wcout << L"EndCapture()...";
        IFC(dwmSlicer->EndCapture(&framesCaptured, &captureResultLeftChannel, &captureResultRightChannel));
        std::wcout << L" SUCCEEDED" << std::endl;
        std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

        std::wcout << L"SaveImageToFile()...";
        IFC(slicerFactory->SaveImageToFile(L"captureResultLeftChannel-5.png", captureResultLeftChannel, GUID_ContainerFormatPng));
        std::wcout << L" SUCCEEDED" << std::endl;
    }


Cleanup:

    if (FAILED(hResult))
    {
        std::wcout << L" FAILED (0x" << std::hex << hResult << L")" << std::endl;
    }

    // Uninitialize COM
    ::CoUninitialize();

    std::wcout << L"Example completed.  Press enter to exit...";
    std::wstring option;
    std::getline(std::wcin, option);

    return 0;
}
