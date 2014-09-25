//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      Test app for the slicer interface and RAD effects.
//
//------------------------------------------------------------------------------

// D2D headers
#include <d2d1_1.h>

// Slicer headers
#include <SlicerInterface.h>

// Built in effects
#include <initguid.h>
#include <D2D1Effects.h>

// Custom effects
#include <RADEffects.h>

// STL
#include <iostream>
#include <string>

// WIC
#include <wincodec.h>

template<typename Interface> inline void SafeRelease(Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != nullptr)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = nullptr;
    }
}

#define IFC(expr) do { hResult = (expr); if (FAILED(hResult)) { goto Cleanup; } } while (0, 0)
#define IFCNULL(expr) do { if (nullptr == (expr)) { hResult = E_INVALIDARG; goto Cleanup; } } while (0, 0)

using namespace RAD;

int _cdecl wmain(int argc, __in wchar_t* argv[])
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    HRESULT hResult;

    // Slicer classes
    ISlicerFactory*     slicerFactory = nullptr;
    ISlicer*            slicer = nullptr;
    IDwmSlicer*         dwmSlicer = nullptr;
    IGdiSlicer*         gdiSlicer = nullptr;

    // DirectX object
    ID2D1Factory1*      factory = nullptr;
    ID2D1Device*        device = nullptr;
    ID2D1DeviceContext* deviceContext = nullptr;

    // Effects
    ID2D1Effect*        gaussianBlur = nullptr;
    ID2D1Effect*        saturation = nullptr;
    ID2D1Effect*        histogram = nullptr;
    ID2D1Effect*        tint = nullptr;
    ID2D1Effect*        invert = nullptr;
    ID2D1Effect*        rgbToLab0 = nullptr;
    ID2D1Effect*        rgbToLab1 = nullptr;
    ID2D1Effect*        labComparer = nullptr;
    ID2D1Effect*        colorDistanceToRgb = nullptr;
    ID2D1Effect*        checkColorDistance = nullptr;
    ID2D1Effect*        debugImageData = nullptr;

    // Images
    ID2D1Bitmap1*       captureResult = nullptr;
    ID2D1Bitmap1*       frame = nullptr;
    ID2D1Image*         outputOfGaussianBlur = nullptr;
    ID2D1Image*         outputOfSaturation = nullptr;
    ID2D1Image*         outputOfHistogram = nullptr;
    ID2D1Image*         outputOfTint = nullptr;
    ID2D1Image*         outputOfInvert = nullptr;
    ID2D1Bitmap1*       test1 = nullptr;
    ID2D1Bitmap1*       test2 = nullptr;
    ID2D1Image*         outputOfRgbToLab0 = nullptr;
    ID2D1Image*         outputOfRgbToLab1 = nullptr;
    ID2D1Image*         outputOfLabImageComparer = nullptr;
    ID2D1Image*         outputOfColorDistanceToRgb = nullptr;
    ID2D1Image*         outputOfCheckColorDistance = nullptr;
    ID2D1Image*         outputOfDebugImageData = nullptr;

    UINT framesCaptured = 0;

    bool useDwmSlicer;

    // Let the user decide the slicer type
    std::wcout << L"Choose a slicer type:" << std::endl
               << L"1 - DwmSlicer (default)" << std::endl
               << L"2 - GdiSlicer" << std::endl
               << L"Enter an option: ";

    std::wstring option;
    std::getline(std::wcin, option);

    if (option.empty() || '2' != option[0])
    {
        useDwmSlicer = true;
        std::wcout << L"DwmSlicer" << std::endl << std::endl;
    }
    else
    {
        useDwmSlicer = false;
        std::wcout << L"GdiSlicer" << std::endl << std::endl;
    }

    // Initialize COM
    IFC(::CoInitializeEx(nullptr, COINIT_MULTITHREADED));

    // Create a slicer factory
    std::wcout << L"CreateSlicerFactory()...";
    IFC(::CreateSlicerFactory(&slicerFactory, ComputeShaderModel::CS_4_0));
    std::wcout << L" SUCCEEDED" << std::endl;
    std::wcout << L"D2D Feature Level: 0x" << std::hex << slicerFactory->GetD2DContextNoRef()->GetD2DFeatureLevel() << std::dec << std::endl;

    // Get the D2D objects we need
    std::wcout << L"slicerFactory->GetD2DDevice()...";
    IFC(slicerFactory->GetD2DContextNoRef()->GetD2DDevice(&device));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"slicerFactory->GetD2DFactory()...";
    IFC(slicerFactory->GetD2DContextNoRef()->GetD2DFactory(&factory));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"device->CreateDeviceContext()...";
    IFC(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));
    std::wcout << L" SUCCEEDED" << std::endl;

    // Test double registration
    std::wcout << L"RegisterEffects()...";
    IFC(::RADRegisterEffects(factory));
    IFC(::RADRegisterEffects(factory));
    std::wcout << L" SUCCEEDED" << std::endl;

    if (useDwmSlicer)
    {
        std::wcout << L"CreateDwmSlicer()...";
        IFC(slicerFactory->CreateDwmSlicer(&dwmSlicer));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Test QueryInterface()...";
        IFC(dwmSlicer->QueryInterface<ISlicer>(&slicer));
        std::wcout << L" SUCCEEDED(" << slicer << L")" << std::endl;
    }
    else
    {
        std::wcout << L"CreateGdiSlicer()...";
        IFC(slicerFactory->CreateGdiSlicer(&gdiSlicer));
        std::wcout << L" SUCCEEDED" << std::endl;

        std::wcout << L"Test QueryInterface()...";
        IFC(gdiSlicer->QueryInterface<ISlicer>(&slicer));
        std::wcout << L" SUCCEEDED(" << slicer << L")" << std::endl;
    }

    UINT numberOfFramesThatCanBeCaptured = 0;

    std::wcout << L"StartCapture()...";
    IFC(slicer->StartCapture(D2D1::RectL(0, 0, 100, 100), 60, 1, Channel::Left, &numberOfFramesThatCanBeCaptured));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"Number of frames that can be captured: " << numberOfFramesThatCanBeCaptured <<std::endl;

    std::wcout << L"Sleeping...";
    ::Sleep(1500);
    std::wcout << L" Done" << std::endl;

    std::wcout << L"EndCapture()...";
    IFC(slicer->EndCapture(&framesCaptured, &captureResult));
    std::wcout << L" SUCCEEDED" << std::endl;
    std::wcout << L"FramesCaptured: " << framesCaptured << std::endl;

    std::wcout << L"SaveImageToFile()...";
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"captureLeft.bmp", captureResult, RAD::ImageContainerFormat::Bmp));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Get a single frame from the atlas
    IFC(slicerFactory->GetD2DContextNoRef()->CreateFrame(D2D1::SizeU(100, 100), 3, captureResult, &frame));
    std::wcout << L"SaveImageToFile()...";
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"frame.bmp", frame, RAD::ImageContainerFormat::Bmp));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Create the effects
    std::wcout << L"CreateEffect(CLSID_D2D1GaussianBlur)...";
    IFC(deviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &gaussianBlur));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_D2D1Saturation)...";
    IFC(deviceContext->CreateEffect(CLSID_D2D1Saturation, &saturation));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_D2D1Histogram)...";
    IFC(deviceContext->CreateEffect(CLSID_D2D1Histogram, &histogram));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADTint)...";
    IFC(deviceContext->CreateEffect(CLSID_RADTint, &tint));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADInvert)...";
    IFC(deviceContext->CreateEffect(CLSID_RADInvert, &invert));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADRgbToLab)...";
    IFC(deviceContext->CreateEffect(CLSID_RADRgbToLab, &rgbToLab0));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADRgbToLab)...";
    IFC(deviceContext->CreateEffect(CLSID_RADRgbToLab, &rgbToLab1));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADLabImageComparer)...";
    IFC(deviceContext->CreateEffect(CLSID_RADLabImageComparer, &labComparer));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADColorDistanceToRgb)...";
    IFC(deviceContext->CreateEffect(CLSID_RADColorDistanceToRgb, &colorDistanceToRgb));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADCheckColorDistance)...";
    IFC(deviceContext->CreateEffect(CLSID_RADCheckColorDistance, &checkColorDistance));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"CreateEffect(CLSID_RADDebugImageData)...";
    IFC(deviceContext->CreateEffect(CLSID_RADDebugImageData, &debugImageData));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Run the image we got through the Saturation effect
    std::wcout << L"saturation->SetValue()...";
    IFC(saturation->SetValue(D2D1_SATURATION_PROP_SATURATION, 0.5f));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"Getting saturation effect output...";
    saturation->SetInput(0, frame);
    saturation->GetOutput(&outputOfSaturation);
    std::wcout << L" Done" << std::endl;

    std::wcout << L"SaveImageToFile(saturation)...";
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"saturation.bmp", outputOfSaturation, RAD::ImageContainerFormat::Bmp));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Run the image we got through the Gaussian Blur effect
    std::wcout << L"gaussianBlur->SetValue()...";
    IFC(gaussianBlur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 2.0f));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"Getting gaussian blur effect output...";
    gaussianBlur->SetInput(0, outputOfSaturation);
    gaussianBlur->GetOutput(&outputOfGaussianBlur);
    std::wcout << L" Done" << std::endl;

    std::wcout << L"SaveImageToFile(gaussianBlur)...";
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"gaussianBlur.bmp", outputOfGaussianBlur, RAD::ImageContainerFormat::Bmp));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Run the image we got through a histogram analysis
    std::wcout << L"histogram->SetValue()...";
    IFC(histogram->SetValue(D2D1_HISTOGRAM_PROP_NUM_BINS, 10));
    IFC(histogram->SetValue(D2D1_HISTOGRAM_PROP_CHANNEL_SELECT, D2D1_CHANNEL_SELECTOR_R));
    std::wcout << L" SUCCEEDED" << std::endl;

    std::wcout << L"Getting histogram effect output...";
    histogram->SetInput(0, outputOfGaussianBlur);
    histogram->GetOutput(&outputOfHistogram);
    std::wcout << L" Done" << std::endl;

    // The output of histogram is data; saving it as a bitmap will fail.
    std::wcout << L"RenderOutput(histogram)...";
    IFC(slicerFactory->GetD2DContextNoRef()->RenderOutput(outputOfHistogram));
    std::wcout << L" SUCCEEDED" << std::endl;

    float bins[10];

    std::wcout << L"histogram->GetValue()...";
    IFC(histogram->GetValue(D2D1_HISTOGRAM_PROP_HISTOGRAM_OUTPUT, (BYTE*)bins, 10*sizeof(float)));
    std::wcout << L" SUCCEEDED" << std::endl;


    // Run the image we got through our custom Tint and Invert effect
    std::wcout << L"Testing Custom Effects...";
    IFC(tint->SetValue(RAD_TINT_PROP_RED,   1.0f));
    IFC(tint->SetValue(RAD_TINT_PROP_GREEN, 0.0f));
    IFC(tint->SetValue(RAD_TINT_PROP_BLUE,  0.0f));
    IFC(tint->SetValue(RAD_TINT_PROP_ALPHA, 0.5f));

    tint->SetInput(0, frame);
    tint->GetOutput(&outputOfTint);
    invert->SetInput(0, outputOfTint);
    invert->GetOutput(&outputOfInvert);

    // Save them in the opposite order just to test the rendering of the graph
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"invert.bmp", outputOfInvert, RAD::ImageContainerFormat::Bmp));
    IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"tint.bmp", outputOfTint, RAD::ImageContainerFormat::Bmp));
    std::wcout << L" SUCCEEDED" << std::endl;

    // Test loading images
    std::wcout << L"Loading Images...";
    IFC(slicerFactory->GetD2DContextNoRef()->LoadImageFromFile(L"test1.bmp", &test1));
    IFC(slicerFactory->GetD2DContextNoRef()->LoadImageFromFile(L"test2.bmp", &test2));
    std::wcout << L" SUCCEEDED" << std::endl;

    // Print out the histogram bins
    std::wcout << std::endl << L"Histogram Output:" << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        std::wcout << L"\t" << i << L":\t" << bins[i] << std::endl;
    }


    // Test the LAB related shaders
    {
        rgbToLab0->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
        rgbToLab0->SetInput(0, test1);
        rgbToLab0->GetOutput(&outputOfRgbToLab0);

        debugImageData->SetInput(0, outputOfRgbToLab0);
        debugImageData->GetOutput(&outputOfDebugImageData);

        std::wcout << L"RenderOutput outputOfDebugImageData...";
        IFC(slicerFactory->GetD2DContextNoRef()->RenderOutput(outputOfDebugImageData));

        FLOAT* imageData = 0;
        UINT32 imageDataLength = 0;

        std::wcout << L"GetValue RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA...";
        IFC(debugImageData->GetValue(RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA, &imageData));

        std::wcout << L"GetValue RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH...";
        IFC(debugImageData->GetValue(RAD_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH, &imageDataLength));

        rgbToLab1->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
        rgbToLab1->SetInput(0, test2);
        rgbToLab1->GetOutput(&outputOfRgbToLab1);

        labComparer->SetInput(0, outputOfRgbToLab0);
        labComparer->SetInput(1, outputOfRgbToLab1);
        labComparer->GetOutput(&outputOfLabImageComparer);

        colorDistanceToRgb->SetInput(0, outputOfLabImageComparer);
        colorDistanceToRgb->GetOutput(&outputOfColorDistanceToRgb);

        checkColorDistance->SetInput(0, outputOfLabImageComparer);
        checkColorDistance->GetOutput(&outputOfCheckColorDistance);

        std::wcout << L"SaveImageToFile(outputOfLabImageComparer)...";
        IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"labComparer0.bmp", outputOfLabImageComparer, RAD::ImageContainerFormat::Bmp));

        std::wcout << L"SaveImageToFile(outputOfColorDistanceToRgb)...";
        IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"colorDistanceToRgb0.bmp", outputOfColorDistanceToRgb, RAD::ImageContainerFormat::Bmp));

        // The output of check color distance is data; saving it as a bitmap will fail.
        std::wcout << L"RenderOutput(outputOfCheckColorDistance)...";
        IFC(slicerFactory->GetD2DContextNoRef()->RenderOutput(outputOfCheckColorDistance));

        UINT32 pixelMatchCount = 0;
        UINT32 pixelMatchFirstX = 0;
        UINT32 pixelMatchFirstY = 0;
        UINT32 pixelMismatchCount = 0;
        UINT32 pixelMismatchFirstX = 0;
        UINT32 pixelMismatchFirstY = 0;
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

        std::wcout << std::endl
                   << L"Check Color Distance (different images): " << std::endl
                   << L"\tPixel Match Count:      " << pixelMatchCount << std::endl
                   << L"\tPixel Match First X: "    << pixelMatchFirstX << std::endl
                   << L"\tPixel Match First Y: "    << pixelMatchFirstY << std::endl
                   << L"\tPixel Mismatch Count:   " << pixelMismatchCount << std::endl
                   << L"\tPixel Mismatch First X: " << pixelMismatchFirstX << std::endl
                   << L"\tPixel Mismatch First Y: " << pixelMismatchFirstY << std::endl;
    }

    SafeRelease(&outputOfRgbToLab0);
    SafeRelease(&outputOfRgbToLab1);
    SafeRelease(&outputOfLabImageComparer);
    SafeRelease(&outputOfColorDistanceToRgb);
    SafeRelease(&outputOfCheckColorDistance);

    {
        rgbToLab0->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
        rgbToLab0->SetInput(0, test1);
        rgbToLab0->GetOutput(&outputOfRgbToLab0);

        rgbToLab1->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
        rgbToLab1->SetInput(0, test1);
        rgbToLab1->GetOutput(&outputOfRgbToLab1);

        labComparer->SetInput(0, outputOfRgbToLab0);
        labComparer->SetInput(1, outputOfRgbToLab1);
        labComparer->GetOutput(&outputOfLabImageComparer);

        colorDistanceToRgb->SetInput(0, outputOfLabImageComparer);
        colorDistanceToRgb->GetOutput(&outputOfColorDistanceToRgb);

        checkColorDistance->SetInput(0, outputOfLabImageComparer);
        checkColorDistance->GetOutput(&outputOfCheckColorDistance);

        std::wcout << L"SaveImageToFile(outputOfLabImageComparer)...";
        IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"labComparer1.bmp", outputOfLabImageComparer, RAD::ImageContainerFormat::Bmp));

        std::wcout << L"SaveImageToFile(outputOfColorDistanceToRgb)...";
        IFC(slicerFactory->GetD2DContextNoRef()->SaveImageToFile(L"colorDistanceToRgb1.bmp", outputOfColorDistanceToRgb, RAD::ImageContainerFormat::Bmp));

        // The output of check color distance is data; saving it as a bitmap will fail.
        std::wcout << L"SaveImageToFile(outputOfCheckColorDistance)...";
        IFC(slicerFactory->GetD2DContextNoRef()->RenderOutput(outputOfCheckColorDistance));

        UINT32 pixelMatchCount = 0;
        UINT32 pixelMatchFirstX = 0;
        UINT32 pixelMatchFirstY = 0;
        UINT32 pixelMismatchCount = 0;
        UINT32 pixelMismatchFirstX = 0;
        UINT32 pixelMismatchFirstY = 0;
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
        IFC(checkColorDistance->GetValue(RAD_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

        std::wcout << std::endl
                   << L"Check Color Distance (same image): " << std::endl
                   << L"\tPixel Match Count:      " << pixelMatchCount << std::endl
                   << L"\tPixel Match First X: "    << pixelMatchFirstX << std::endl
                   << L"\tPixel Match First Y: "    << pixelMatchFirstY << std::endl
                   << L"\tPixel Mismatch Count:   " << pixelMismatchCount << std::endl
                   << L"\tPixel Mismatch First X: " << pixelMismatchFirstX << std::endl
                   << L"\tPixel Mismatch First Y: " << pixelMismatchFirstY << std::endl;
    }

    std::wcout << std::endl;


Cleanup:

    if (FAILED(hResult))
    {
        std::wcout << L" FAILED (0x" << std::hex << hResult << L")" << std::endl;
    }

    // Unregister the custom effects
    ::RADUnregisterEffects(factory);

    // Release the textures we got
    SafeRelease(&captureResult);
    SafeRelease(&frame);

    // Release the effects
    SafeRelease(&gaussianBlur);
    SafeRelease(&saturation);
    SafeRelease(&histogram);
    SafeRelease(&tint);
    SafeRelease(&invert);
    SafeRelease(&rgbToLab0);
    SafeRelease(&rgbToLab1);
    SafeRelease(&labComparer);
    SafeRelease(&colorDistanceToRgb);
    SafeRelease(&checkColorDistance);
    SafeRelease(&debugImageData);

    // Release the images
    SafeRelease(&outputOfGaussianBlur);
    SafeRelease(&outputOfSaturation);
    SafeRelease(&outputOfHistogram);
    SafeRelease(&outputOfTint);
    SafeRelease(&outputOfInvert);
    SafeRelease(&test1);
    SafeRelease(&test2);
    SafeRelease(&outputOfRgbToLab0);
    SafeRelease(&outputOfRgbToLab1);
    SafeRelease(&outputOfLabImageComparer);
    SafeRelease(&outputOfColorDistanceToRgb);
    SafeRelease(&outputOfCheckColorDistance);
    SafeRelease(&outputOfDebugImageData);

    // Release the devices
    SafeRelease(&deviceContext);
    SafeRelease(&device);
    SafeRelease(&factory);
    SafeRelease(&slicer);

    // Release the slicer objects
    SafeRelease(&dwmSlicer);
    SafeRelease(&gdiSlicer);
    SafeRelease(&slicer);
    SafeRelease(&slicerFactory);

    // Uninitialize COM
    ::CoUninitialize();

    std::wcout << L"Example completed.  Press enter to exit...";
    std::getline(std::wcin, option);

    return 0;
}
