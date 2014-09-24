//------------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "Effects.h"

using namespace WEX::TestExecution;
using namespace WEX::Common;
using namespace WEX::Logging;
using namespace UnitTests;
using namespace WindowManager;


bool Effects::ClassSetup(void)
{
    VERIFY_SUCCEEDED(::CreateSlicerFactory(&this->mSlicerFactory, RADv2::ComputeShaderModel::CS_4_0));
    VERIFY_SUCCEEDED(::RADv2RegisterEffects(this->mSlicerFactory->GetD2DFactoryNoRef()));

    return true;
}


bool Effects::ClassCleanup(void)
{
    if (nullptr != this->mSlicerFactory)
    {
        ::RADv2UnregisterEffects(this->mSlicerFactory->GetD2DFactoryNoRef());
    }

    return true;
}


void Effects::LabConversion(void)
{
    RgbColor centerPixelRgbColor(  237.0/255.0,  28.0/255.0,  36.0/255.0);
    RgbColor surroundPixelRgbColor(197.0/255.0, 194.0/255.0, 193.0/255.0);

    LabColor centerPixelLabColor = centerPixelRgbColor;
    LabColor surroundPixelLabColor = surroundPixelRgbColor;

    LabColor colors[] =
    {
        surroundPixelLabColor, surroundPixelLabColor, surroundPixelLabColor,
        surroundPixelLabColor, centerPixelLabColor,   surroundPixelLabColor,
        surroundPixelLabColor, surroundPixelLabColor, surroundPixelLabColor,
    };

    // Define the rectangles that describe the test image
    D2D1_SIZE_U imageSize = D2D1::SizeU(3, 3);
    D2D1_RECT_F surroundRect = D2D1::RectF(0, 0, 3, 3);
    D2D1_RECT_F centerRect =   D2D1::RectF(1, 1, 2, 2);

    std::vector<ColorRect> rects;
    rects.push_back(ColorRect(surroundRect, surroundPixelRgbColor));
    rects.push_back(ColorRect(centerRect, centerPixelRgbColor));

    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    // Generate the test image
    CComPtr<ID2D1Bitmap1> image;
    this->RenderRects(deviceContext, imageSize, rects, &image);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabConversion_Image.bmp", image, GUID_ContainerFormatBmp));

    // Create the effects
    CComPtr<ID2D1Effect> rgbToLab;
    CComPtr<ID2D1Effect> debugImageData;

    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2DebugImageData, &debugImageData));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbToLab;
    CComPtr<ID2D1Image> outputOfDebugImageData;

    rgbToLab->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab->SetInput(0, image);
    rgbToLab->GetOutput(&outputOfRgbToLab);

    debugImageData->SetInput(0, outputOfRgbToLab);
    debugImageData->GetOutput(&outputOfDebugImageData);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfDebugImageData));

    // Verify
    struct LabColor_32bpc
    {
        FLOAT L;
        FLOAT a;
        FLOAT b;
        FLOAT W;
    };

    LabColor_32bpc* imageData = nullptr;
    UINT32 imageDataLength = 0;

    VERIFY_SUCCEEDED(debugImageData->GetValue(RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA, &imageData));
    VERIFY_SUCCEEDED(debugImageData->GetValue(RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH, &imageDataLength));

    Log::Comment(String().Format(L"Image Data Length: %d", imageDataLength));
    VERIFY_ARE_EQUAL(4 * 4 * 9, imageDataLength);

    for (UINT i = 0; i < ARRAYSIZE(colors); ++i)
    {
        LabColor sample(imageData[i].L, imageData[i].a, imageData[i].b);
        Log::Comment(String().Format(L"Pixel %d: { %.4f, %.4f, %.4f }", i, imageData[i].L, imageData[i].a, imageData[i].b));

        // The distance between the two LAB colors should be negligible
        double distance = LabColor::Distance(sample, colors[i]);
        VERIFY_IS_LESS_THAN(distance, 0.0001, String().Format(L"Distance: %f", distance));
    }
}


void Effects::LabDistance(void)
{
    RgbColor centerPixelRgbColor1(  237.0/255.0,  28.0/255.0,  36.0/255.0);
    RgbColor surroundPixelRgbColor1(197.0/255.0, 194.0/255.0, 193.0/255.0);

    LabColor centerPixelLabColor1 = centerPixelRgbColor1;
    LabColor surroundPixelLabColor1 = surroundPixelRgbColor1;

    RgbColor centerPixelRgbColor2(  255.0/255.0, 127.0/255.0,  39.0/255.0);
    RgbColor surroundPixelRgbColor2(195.0/255.0, 195.0/255.0, 195.0/255.0);

    LabColor centerPixelLabColor2 =   centerPixelRgbColor2;
    LabColor surroundPixelLabColor2 = surroundPixelRgbColor2;

    LabColor colors1[] =
    {
        surroundPixelLabColor1, surroundPixelLabColor1, surroundPixelLabColor1,
        surroundPixelLabColor1, centerPixelLabColor1,   surroundPixelLabColor1,
        surroundPixelLabColor1, surroundPixelLabColor1, surroundPixelLabColor1,
    };

    LabColor colors2[] =
    {
        surroundPixelLabColor2, surroundPixelLabColor2, surroundPixelLabColor2,
        surroundPixelLabColor2, centerPixelLabColor2,   surroundPixelLabColor2,
        surroundPixelLabColor2, surroundPixelLabColor2, surroundPixelLabColor2,
    };

    // Define the rectangles that describe the test image
    D2D1_SIZE_U imageSize = D2D1::SizeU(3, 3);
    D2D1_RECT_F surroundRect = D2D1::RectF(0, 0, 3, 3);
    D2D1_RECT_F centerRect =   D2D1::RectF(1, 1, 2, 2);

    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    // Generate the test images
    CComPtr<ID2D1Bitmap1> image1;
    CComPtr<ID2D1Bitmap1> image2;

    std::vector<ColorRect> rects1;
    rects1.push_back(ColorRect(surroundRect, surroundPixelRgbColor1));
    rects1.push_back(ColorRect(centerRect, centerPixelRgbColor1));

    this->RenderRects(deviceContext, imageSize, rects1, &image1);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabDistance_Image1.bmp", image1, GUID_ContainerFormatBmp));

    std::vector<ColorRect> rects2;
    rects2.push_back(ColorRect(surroundRect, surroundPixelRgbColor2));
    rects2.push_back(ColorRect(centerRect, centerPixelRgbColor2));

    this->RenderRects(deviceContext, imageSize, rects2, &image2);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabDistance_Image2.bmp", image2, GUID_ContainerFormatBmp));


    // Create the effects
    CComPtr<ID2D1Effect> rgbToLab1;
    CComPtr<ID2D1Effect> rgbToLab2;
    CComPtr<ID2D1Effect> labImageComparer;
    CComPtr<ID2D1Effect> debugImageData;

    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab1));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab2));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2LabImageComparer, &labImageComparer));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2DebugImageData, &debugImageData));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbToLab1;
    CComPtr<ID2D1Image> outputOfRgbToLab2;
    CComPtr<ID2D1Image> outputOfLabImageComparer;
    CComPtr<ID2D1Image> outputOfDebugImageData;

    rgbToLab1->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab1->SetInput(0, image1);
    rgbToLab1->GetOutput(&outputOfRgbToLab1);

    rgbToLab2->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab2->SetInput(0, image2);
    rgbToLab2->GetOutput(&outputOfRgbToLab2);

    labImageComparer->SetInput(0, outputOfRgbToLab1);
    labImageComparer->SetInput(1, outputOfRgbToLab2);
    labImageComparer->GetOutput(&outputOfLabImageComparer);

    debugImageData->SetInput(0, outputOfLabImageComparer);
    debugImageData->GetOutput(&outputOfDebugImageData);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfDebugImageData));

    // Verify
    struct LabDistance_32bpc
    {
        FLOAT Distance;
        FLOAT y;
        FLOAT z;
        FLOAT W;
    };

    LabDistance_32bpc* imageData = nullptr;
    UINT32 imageDataLength = 0;

    VERIFY_SUCCEEDED(debugImageData->GetValue(RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA, &imageData));
    VERIFY_SUCCEEDED(debugImageData->GetValue(RADV2_DEBUG_IMAGE_DATA_PROP_IMAGE_DATA_LENGTH, &imageDataLength));

    Log::Comment(String().Format(L"Image Data Length: %d", imageDataLength));
    VERIFY_ARE_EQUAL(4 * 4 * 9, imageDataLength);

    for (UINT i = 0; i < ARRAYSIZE(colors1); ++i)
    {
        Log::Comment(String().Format(L"Pixel %d: %.4f", i, imageData[i].Distance));

        // The difference between the two distances should be negligible
        double distance = LabColor::Distance(colors1[i], colors2[i]);
        double delta = ::abs(distance - static_cast<double>(imageData[i].Distance));

        VERIFY_IS_LESS_THAN(delta, 0.0001, String().Format(L"Delta: %f", delta));
    }

    // Verify check color distance shader on a small image
    CComPtr<ID2D1Effect> checkColorDistance;
    CComPtr<ID2D1Image> outputOfCheckColorDistance;
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2CheckColorDistance, &checkColorDistance));

    checkColorDistance->SetInput(0, outputOfLabImageComparer);
    checkColorDistance->GetOutput(&outputOfCheckColorDistance);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfCheckColorDistance));

    UINT32 pixelMatchCountExpected = 8;
    UINT32 pixelMatchFirstXExpected = 0;
    UINT32 pixelMatchFirstYExpected = 0;
    UINT32 pixelMismatchCountExpected = 1;
    UINT32 pixelMismatchFirstXExpected = 1;
    UINT32 pixelMismatchFirstYExpected = 1;

    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

    Log::Comment(String().Format(L"Pixel Match Count:      %d", pixelMatchCount));
    Log::Comment(String().Format(L"Pixel Match First X:    %d", pixelMatchFirstX));
    Log::Comment(String().Format(L"Pixel Match First Y:    %d", pixelMatchFirstY));
    Log::Comment(String().Format(L"Pixel Mismatch Count:   %d", pixelMismatchCount));
    Log::Comment(String().Format(L"Pixel Mismatch First X: %d", pixelMismatchFirstX));
    Log::Comment(String().Format(L"Pixel Mismatch First Y: %d", pixelMismatchFirstY));

    VERIFY_ARE_EQUAL(pixelMatchCountExpected, pixelMatchCount);
    VERIFY_ARE_EQUAL(pixelMatchFirstXExpected, pixelMatchFirstX);
    VERIFY_ARE_EQUAL(pixelMatchFirstYExpected, pixelMatchFirstY);
    VERIFY_ARE_EQUAL(pixelMismatchCountExpected, pixelMismatchCount);
    VERIFY_ARE_EQUAL(pixelMismatchFirstXExpected, pixelMismatchFirstX);
    VERIFY_ARE_EQUAL(pixelMismatchFirstYExpected, pixelMismatchFirstY);

}


void Effects::LabColorComparerGraph(void)
{
    RgbColor inputRgbColor(237.0/255.0, 28.0/255.0, 36.0/255.0);
    LabColor inputLabColor = inputRgbColor;

    RgbColor color1(181.0/255.0, 230.0/255.0,  29.0/255.0);
    RgbColor color2(237.0/255.0,  28.0/255.0,  36.0/255.0);
    RgbColor color3(  0.0/255.0, 162.0/255.0, 232.0/255.0);

    // Define the rectangles that describe the test image
    D2D1_SIZE_U imageSize = D2D1::SizeU(73, 67);
    D2D1_RECT_F rect1 = D2D1::RectF(0, 0, 73, 67);
    D2D1_RECT_F rect2 = D2D1::RectF(10, 13, 22, 27);
    D2D1_RECT_F rect3 = D2D1::RectF(45, 35, 63, 50);

    std::vector<ColorRect> rects;
    rects.push_back(ColorRect(rect1, color1));
    rects.push_back(ColorRect(rect2, color2));
    rects.push_back(ColorRect(rect3, color3));

    const UINT32 PixelMatchCountExpected = static_cast<UINT32>((rect2.right - rect2.left) * (rect2.bottom - rect2.top));
    const UINT32 PixelMatchFirstXExpected = static_cast<UINT32>(rect2.left);
    const UINT32 PixelMatchFirstYExpected = static_cast<UINT32>(rect2.top);
    const UINT32 PixelMismatchCountExpected = (imageSize.width * imageSize.height) - PixelMatchCountExpected;
    const UINT32 PixelMismatchFirstXExpected = 0;
    const UINT32 PixelMismatchFirstYExpected = 0;

    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    // Generate the test image
    CComPtr<ID2D1Bitmap1> image;
    this->RenderRects(deviceContext, imageSize, rects, &image);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabColorComparerGraph_Image.bmp", image, GUID_ContainerFormatBmp));

    // Create the effects
    CComPtr<ID2D1Effect> rgbToLab;
    CComPtr<ID2D1Effect> labColorComparer;
    CComPtr<ID2D1Effect> colorDistanceToRgb;
    CComPtr<ID2D1Effect> checkColorDistance;

    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2LabColorComparer, &labColorComparer));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2ColorDistanceToRgb, &colorDistanceToRgb));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2CheckColorDistance, &checkColorDistance));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbToLab;
    CComPtr<ID2D1Image> outputOfLabColorComparer;
    CComPtr<ID2D1Image> outputOfColorDistanceToRgb;
    CComPtr<ID2D1Image> outputOfCheckColorDistance;

    rgbToLab->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab->SetInput(0, image);
    rgbToLab->GetOutput(&outputOfRgbToLab);

    VERIFY_SUCCEEDED(labColorComparer->SetValue(RADV2_LAB_COLOR_COMPARER_PROP_L, static_cast<FLOAT>(inputLabColor.GetL())));
    VERIFY_SUCCEEDED(labColorComparer->SetValue(RADV2_LAB_COLOR_COMPARER_PROP_A, static_cast<FLOAT>(inputLabColor.GetA())));
    VERIFY_SUCCEEDED(labColorComparer->SetValue(RADV2_LAB_COLOR_COMPARER_PROP_B, static_cast<FLOAT>(inputLabColor.GetB())));

    labColorComparer->SetInput(0, outputOfRgbToLab);
    labColorComparer->GetOutput(&outputOfLabColorComparer);

    colorDistanceToRgb->SetInput(0, outputOfLabColorComparer);
    colorDistanceToRgb->GetOutput(&outputOfColorDistanceToRgb);

    checkColorDistance->SetInput(0, outputOfLabColorComparer);
    checkColorDistance->GetOutput(&outputOfCheckColorDistance);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfColorDistanceToRgb));
    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfCheckColorDistance));

    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabColorComparerGraph_outputOfRgbToLab.bmp", outputOfRgbToLab, GUID_ContainerFormatBmp));
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabColorComparerGraph_outputOfLabColorComparer.bmp", outputOfLabColorComparer, GUID_ContainerFormatBmp));
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabColorComparerGraph_outputOfColorDistanceToRgb.bmp", outputOfColorDistanceToRgb, GUID_ContainerFormatBmp));

    // Verify
    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

    Log::Comment(String().Format(L"Pixel Match Count:      %d", pixelMatchCount));
    Log::Comment(String().Format(L"Pixel Match First X:    %d", pixelMatchFirstX));
    Log::Comment(String().Format(L"Pixel Match First Y:    %d", pixelMatchFirstY));
    Log::Comment(String().Format(L"Pixel Mismatch Count:   %d", pixelMismatchCount));
    Log::Comment(String().Format(L"Pixel Mismatch First X: %d", pixelMismatchFirstX));
    Log::Comment(String().Format(L"Pixel Mismatch First Y: %d", pixelMismatchFirstY));

    VERIFY_ARE_EQUAL(PixelMatchCountExpected, pixelMatchCount);
    VERIFY_ARE_EQUAL(PixelMatchFirstXExpected, pixelMatchFirstX);
    VERIFY_ARE_EQUAL(PixelMatchFirstYExpected, pixelMatchFirstY);
    VERIFY_ARE_EQUAL(PixelMismatchCountExpected, pixelMismatchCount);
    VERIFY_ARE_EQUAL(PixelMismatchFirstXExpected, pixelMismatchFirstX);
    VERIFY_ARE_EQUAL(PixelMismatchFirstYExpected, pixelMismatchFirstY);
}


void Effects::LabImageComparerGraph(void)
{
    RgbColor image1RgbColor1(181.0/255.0, 230.0/255.0,  29.0/255.0);
    RgbColor image1RgbColor2(237.0/255.0,  28.0/255.0,  36.0/255.0);
    RgbColor image1RgbColor3(  0.0/255.0, 162.0/255.0, 232.0/255.0);

    RgbColor image2RgbColor1(181.0/255.0, 230.0/255.0,  29.0/255.0);
    RgbColor image2RgbColor2(240.0/255.0,  20.0/255.0,  66.0/255.0);
    RgbColor image2RgbColor3(  0.0/255.0, 162.0/255.0, 200.0/255.0);

    // Define the rectangles that describe the test image
    D2D1_SIZE_U imageSize = D2D1::SizeU(73, 67);
    D2D1_RECT_F rect1 = D2D1::RectF(0, 0, 73, 67);
    D2D1_RECT_F rect2 = D2D1::RectF(10, 13, 22, 27);
    D2D1_RECT_F rect3 = D2D1::RectF(45, 35, 63, 50);

    const UINT32 PixelMismatchCountExpected = static_cast<UINT32>(((rect2.right - rect2.left) * (rect2.bottom - rect2.top)) + ((rect3.right - rect3.left) * (rect3.bottom - rect3.top)));
    const UINT32 PixelMismatchFirstXExpected = static_cast<UINT32>(rect2.left);
    const UINT32 PixelMismatchFirstYExpected = static_cast<UINT32>(rect2.top);
    const UINT32 PixelMatchCountExpected = (imageSize.width * imageSize.height) - PixelMismatchCountExpected;
    const UINT32 PixelMatchFirstXExpected = 0;
    const UINT32 PixelMatchFirstYExpected = 0;

    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    // Generate the test images
    CComPtr<ID2D1Bitmap1> image1;
    CComPtr<ID2D1Bitmap1> image2;

    std::vector<ColorRect> rects1;
    rects1.push_back(ColorRect(rect1, image1RgbColor1));
    rects1.push_back(ColorRect(rect2, image1RgbColor2));
    rects1.push_back(ColorRect(rect3, image1RgbColor3));

    this->RenderRects(deviceContext, imageSize, rects1, &image1);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_Image1.bmp", image1, GUID_ContainerFormatBmp));

    std::vector<ColorRect> rects2;
    rects2.push_back(ColorRect(rect1, image2RgbColor1));
    rects2.push_back(ColorRect(rect2, image2RgbColor2));
    rects2.push_back(ColorRect(rect3, image2RgbColor3));

    this->RenderRects(deviceContext, imageSize, rects2, &image2);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_Image2.bmp", image2, GUID_ContainerFormatBmp));


    // Create the effects
    CComPtr<ID2D1Effect> rgbToLab1;
    CComPtr<ID2D1Effect> rgbToLab2;
    CComPtr<ID2D1Effect> labImageComparer;
    CComPtr<ID2D1Effect> colorDistanceToRgb;
    CComPtr<ID2D1Effect> checkColorDistance;

    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab1));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbToLab, &rgbToLab2));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2LabImageComparer, &labImageComparer));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2ColorDistanceToRgb, &colorDistanceToRgb));
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2CheckColorDistance, &checkColorDistance));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbToLab1;
    CComPtr<ID2D1Image> outputOfRgbToLab2;
    CComPtr<ID2D1Image> outputOfLabImageComparer;
    CComPtr<ID2D1Image> outputOfColorDistanceToRgb;
    CComPtr<ID2D1Image> outputOfCheckColorDistance;


    rgbToLab1->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab1->SetInput(0, image1);
    rgbToLab1->GetOutput(&outputOfRgbToLab1);

    rgbToLab2->SetValue(D2D1_PROPERTY_PRECISION, D2D1_BUFFER_PRECISION_32BPC_FLOAT);
    rgbToLab2->SetInput(0, image2);
    rgbToLab2->GetOutput(&outputOfRgbToLab2);

    labImageComparer->SetInput(0, outputOfRgbToLab1);
    labImageComparer->SetInput(1, outputOfRgbToLab2);
    labImageComparer->GetOutput(&outputOfLabImageComparer);

    colorDistanceToRgb->SetInput(0, outputOfLabImageComparer);
    colorDistanceToRgb->GetOutput(&outputOfColorDistanceToRgb);

    checkColorDistance->SetInput(0, outputOfLabImageComparer);
    checkColorDistance->GetOutput(&outputOfCheckColorDistance);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfColorDistanceToRgb));
    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfCheckColorDistance));

    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_outputOfRgbToLab1.bmp", outputOfRgbToLab1, GUID_ContainerFormatBmp));
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_outputOfRgbToLab2.bmp", outputOfRgbToLab2, GUID_ContainerFormatBmp));
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_outputOfLabImageComparer.bmp", outputOfLabImageComparer, GUID_ContainerFormatBmp));
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"LabImageComparerGraph_outputOfColorDistanceToRgb.bmp", outputOfColorDistanceToRgb, GUID_ContainerFormatBmp));

    // Verify
    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
    VERIFY_SUCCEEDED(checkColorDistance->GetValue(RADV2_CHECK_COLOR_DISTANCE_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

    Log::Comment(String().Format(L"Pixel Match Count:      %d", pixelMatchCount));
    Log::Comment(String().Format(L"Pixel Match First X:    %d", pixelMatchFirstX));
    Log::Comment(String().Format(L"Pixel Match First Y:    %d", pixelMatchFirstY));
    Log::Comment(String().Format(L"Pixel Mismatch Count:   %d", pixelMismatchCount));
    Log::Comment(String().Format(L"Pixel Mismatch First X: %d", pixelMismatchFirstX));
    Log::Comment(String().Format(L"Pixel Mismatch First Y: %d", pixelMismatchFirstY));

    VERIFY_ARE_EQUAL(PixelMatchCountExpected, pixelMatchCount);
    VERIFY_ARE_EQUAL(PixelMatchFirstXExpected, pixelMatchFirstX);
    VERIFY_ARE_EQUAL(PixelMatchFirstYExpected, pixelMatchFirstY);
    VERIFY_ARE_EQUAL(PixelMismatchCountExpected, pixelMismatchCount);
    VERIFY_ARE_EQUAL(PixelMismatchFirstXExpected, pixelMismatchFirstX);
    VERIFY_ARE_EQUAL(PixelMismatchFirstYExpected, pixelMismatchFirstY);
}


void Effects::RgbColorComparerGraph(void)
{
    RgbColor inputColor(237.0/255.0, 28.0/255.0, 36.0/255.0);

    RgbColor color1(195.0/255.0, 195.0/255.0, 195.0/255.0);
    RgbColor color2(255.0/255.0, 127.0/255.0, 39.0/255.0);
    RgbColor color3(237.0/255.0, 28.0/255.0,  36.0/255.0);

    D2D1_SIZE_U size = D2D1::SizeU(53, 71);
    D2D1_RECT_F rect1 = D2D1::RectF(0, 0, 53, 71);
    D2D1_RECT_F rect2 = D2D1::RectF(10, 10, 43, 61);
    D2D1_RECT_F rect3 = D2D1::RectF(20, 20, 33, 51);

    std::vector<ColorRect> rects;
    rects.push_back(ColorRect(rect1, color1));
    rects.push_back(ColorRect(rect2, color2));
    rects.push_back(ColorRect(rect3, color3));

    const UINT32 PixelMatchCountExpected = static_cast<UINT32>((rect3.right - rect3.left) * (rect3.bottom - rect3.top));
    const UINT32 PixelMatchFirstXExpected = static_cast<UINT32>(rect3.left);
    const UINT32 PixelMatchFirstYExpected = static_cast<UINT32>(rect3.top);
    const UINT32 PixelMismatchCountExpected = (size.width * size.height) - PixelMatchCountExpected;
    const UINT32 PixelMismatchFirstXExpected = 0;
    const UINT32 PixelMismatchFirstYExpected = 0;


    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    CComPtr<ID2D1Bitmap1> image;
    this->RenderRects(deviceContext, size, rects, &image);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"RgbColorComparerGraph_Image.bmp", image, GUID_ContainerFormatBmp));

    // Create the effects
    CComPtr<ID2D1Effect> rgbColorComparer;
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbColorComparer, &rgbColorComparer));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbColorComparer;

    rgbColorComparer->SetValue(RADV2_RGB_COLOR_COMPARER_PROP_RED,   static_cast<float>(inputColor.GetRed()));
    rgbColorComparer->SetValue(RADV2_RGB_COLOR_COMPARER_PROP_GREEN, static_cast<float>(inputColor.GetGreen()));
    rgbColorComparer->SetValue(RADV2_RGB_COLOR_COMPARER_PROP_BLUE,  static_cast<float>(inputColor.GetBlue()));

    rgbColorComparer->SetInput(0, image);
    rgbColorComparer->GetOutput(&outputOfRgbColorComparer);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfRgbColorComparer));

    // Verify
    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
    VERIFY_SUCCEEDED(rgbColorComparer->GetValue(RADV2_RGB_COLOR_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

    Log::Comment(String().Format(L"Pixel Match Count:      %d", pixelMatchCount));
    Log::Comment(String().Format(L"Pixel Match First X:    %d", pixelMatchFirstX));
    Log::Comment(String().Format(L"Pixel Match First Y:    %d", pixelMatchFirstY));
    Log::Comment(String().Format(L"Pixel Mismatch Count:   %d", pixelMismatchCount));
    Log::Comment(String().Format(L"Pixel Mismatch First X: %d", pixelMismatchFirstX));
    Log::Comment(String().Format(L"Pixel Mismatch First Y: %d", pixelMismatchFirstY));

    VERIFY_ARE_EQUAL(PixelMatchCountExpected, pixelMatchCount);
    VERIFY_ARE_EQUAL(PixelMatchFirstXExpected, pixelMatchFirstX);
    VERIFY_ARE_EQUAL(PixelMatchFirstYExpected, pixelMatchFirstY);
    VERIFY_ARE_EQUAL(PixelMismatchCountExpected, pixelMismatchCount);
    VERIFY_ARE_EQUAL(PixelMismatchFirstXExpected, pixelMismatchFirstX);
    VERIFY_ARE_EQUAL(PixelMismatchFirstYExpected, pixelMismatchFirstY);
}


void Effects::RgbImageComparerGraph(void)
{
    D2D1_SIZE_U size = D2D1::SizeU(53, 71);

    D2D1_RECT_F rect1 = D2D1::RectF(0, 0, 53, 71);
    D2D1_RECT_F rect2 = D2D1::RectF(10, 10, 43, 61);
    D2D1_RECT_F rect3 = D2D1::RectF(20, 20, 33, 51);

    const UINT32 PixelMismatchCountExpected = static_cast<UINT32>((rect3.right - rect3.left) * (rect3.bottom - rect3.top));
    const UINT32 PixelMismatchFirstXExpected = static_cast<UINT32>(rect3.left);
    const UINT32 PixelMismatchFirstYExpected = static_cast<UINT32>(rect3.top);
    const UINT32 PixelMatchCountExpected = (size.width * size.height) - PixelMismatchCountExpected;
    const UINT32 PixelMatchFirstXExpected = 0;
    const UINT32 PixelMatchFirstYExpected = 0;

    // Create a device context
    CComPtr<ID2D1DeviceContext> deviceContext;
    VERIFY_SUCCEEDED(this->mSlicerFactory->GetD2DDeviceNoRef()->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &deviceContext));

    RgbColor image1Color1(195.0/255.0, 195.0/255.0, 195.0/255.0);
    RgbColor image1Color2(255.0/255.0, 127.0/255.0,  39.0/255.0);
    RgbColor image1Color3(237.0/255.0,  28.0/255.0,  36.0/255.0);

    std::vector<ColorRect> rects1;
    rects1.push_back(ColorRect(rect1, image1Color1));
    rects1.push_back(ColorRect(rect2, image1Color2));
    rects1.push_back(ColorRect(rect3, image1Color3));

    CComPtr<ID2D1Bitmap1> image1;
    this->RenderRects(deviceContext, size, rects1, &image1);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"RgbImageComparerGraph_Image1.bmp", image1, GUID_ContainerFormatBmp));

    RgbColor image2Color1(197.0/255.0, 194.0/255.0, 193.0/255.0);
    RgbColor image2Color2(255.0/255.0, 127.0/255.0,  39.0/255.0);
    RgbColor image2Color3(  0.0/255.0,  28.0/255.0,  36.0/255.0);

    std::vector<ColorRect> rects2;
    rects2.push_back(ColorRect(rect1, image2Color1));
    rects2.push_back(ColorRect(rect2, image2Color2));
    rects2.push_back(ColorRect(rect3, image2Color3));

    CComPtr<ID2D1Bitmap1> image2;
    this->RenderRects(deviceContext, size, rects2, &image2);
    VERIFY_SUCCEEDED(this->mSlicerFactory->SaveImageToFile(L"RgbImageComparerGraph_Image2.bmp", image2, GUID_ContainerFormatBmp));


    // Create the effects
    CComPtr<ID2D1Effect> rgbImageComparer;
    VERIFY_SUCCEEDED(deviceContext->CreateEffect(CLSID_RADV2RgbImageComparer, &rgbImageComparer));

    // Create the effect graph
    CComPtr<ID2D1Image> outputOfRgbImageComparer;
    rgbImageComparer->SetInput(0, image1);
    rgbImageComparer->SetInput(1, image2);
    rgbImageComparer->GetOutput(&outputOfRgbImageComparer);

    VERIFY_SUCCEEDED(this->mSlicerFactory->RenderOutput(outputOfRgbImageComparer));

    // Verify
    UINT32 pixelMatchCount = 0;
    UINT32 pixelMatchFirstX = 0;
    UINT32 pixelMatchFirstY = 0;
    UINT32 pixelMismatchCount = 0;
    UINT32 pixelMismatchFirstX = 0;
    UINT32 pixelMismatchFirstY = 0;

    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_COUNT, &pixelMatchCount));
    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_X, &pixelMatchFirstX));
    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MATCH_FIRST_Y, &pixelMatchFirstY));
    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_COUNT, &pixelMismatchCount));
    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_X, &pixelMismatchFirstX));
    VERIFY_SUCCEEDED(rgbImageComparer->GetValue(RADV2_RGB_IMAGE_COMPARER_PROP_PIXEL_MISMATCH_FIRST_Y, &pixelMismatchFirstY));

    Log::Comment(String().Format(L"Pixel Match Count:      %d", pixelMatchCount));
    Log::Comment(String().Format(L"Pixel Match First X:    %d", pixelMatchFirstX));
    Log::Comment(String().Format(L"Pixel Match First Y:    %d", pixelMatchFirstY));
    Log::Comment(String().Format(L"Pixel Mismatch Count:   %d", pixelMismatchCount));
    Log::Comment(String().Format(L"Pixel Mismatch First X: %d", pixelMismatchFirstX));
    Log::Comment(String().Format(L"Pixel Mismatch First Y: %d", pixelMismatchFirstY));

    VERIFY_ARE_EQUAL(PixelMatchCountExpected, pixelMatchCount);
    VERIFY_ARE_EQUAL(PixelMatchFirstXExpected, pixelMatchFirstX);
    VERIFY_ARE_EQUAL(PixelMatchFirstYExpected, pixelMatchFirstY);
    VERIFY_ARE_EQUAL(PixelMismatchCountExpected, pixelMismatchCount);
    VERIFY_ARE_EQUAL(PixelMismatchFirstXExpected, pixelMismatchFirstX);
    VERIFY_ARE_EQUAL(PixelMismatchFirstYExpected, pixelMismatchFirstY);
}


void Effects::RenderRects(ID2D1DeviceContext* argDeviceContext, D2D1_SIZE_U argSize, const std::vector<ColorRect>& argColorRects, ID2D1Bitmap1** argBitmap)
{
    D2D1_BITMAP_PROPERTIES1 renderTargetProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                D2D1_ALPHA_MODE_IGNORE));

    VERIFY_SUCCEEDED(argDeviceContext->CreateBitmap(
        argSize,
        nullptr,
        0,
        &renderTargetProperties,
        argBitmap));

    argDeviceContext->SetTarget(*argBitmap);

    argDeviceContext->BeginDraw();

    for (UINT i = 0; i < argColorRects.size(); ++i)
    {
        CComPtr<ID2D1SolidColorBrush> brush;
        VERIFY_SUCCEEDED(argDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(
                static_cast<FLOAT>(argColorRects[i].color.GetRed()),
                static_cast<FLOAT>(argColorRects[i].color.GetGreen()),
                static_cast<FLOAT>(argColorRects[i].color.GetBlue())),
            &brush));

        argDeviceContext->FillRectangle(argColorRects[i].rect, brush);
    }

    VERIFY_SUCCEEDED(argDeviceContext->EndDraw());
}
