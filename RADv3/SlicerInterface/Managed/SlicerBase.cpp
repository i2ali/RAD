//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//------------------------------------------------------------------------------

#include "Stdafx.h"
#include "DwmSlicer.h"

using namespace RAD::Managed;

//
// Constructor
//

SlicerBase::SlicerBase(
    RAD::ISlicer* argSlicer,
    RAD::ISlicerFactory* argSlicerFactory,
    HMODULE argSlicerInterfaceModule)
    : mSlicer(argSlicer),
      mSlicerFactory(argSlicerFactory),
      mSlicerInterfaceModule(argSlicerInterfaceModule)
{
    argSlicerFactory->AddRef();
}


//
// Destructor
//

SlicerBase::~SlicerBase(void)
{
    this->!SlicerBase();
}


//
// Finalizer
//

SlicerBase::!SlicerBase(void)
{
    if (nullptr != this->mSlicer)
    {
        this->mSlicer->Release();
        this->mSlicer = nullptr;
    }

    if (nullptr != this->mSlicerFactory)
    {
        this->mSlicerFactory->Release();
        this->mSlicerFactory = nullptr;
    }

    if (nullptr != this->mSlicerInterfaceModule)
    {
        ::FreeLibrary(this->mSlicerInterfaceModule);
        this->mSlicerInterfaceModule = nullptr;
    }
}


//
// Public Methods
//

bool SlicerBase::IsSlicing(void)
{
    return this->mSlicer->IsSlicing();
}


HRESULT SlicerBase::CalculateMaxFramesThatCanBeCaptured(
    LONG argLeft,
    LONG argTop,
    LONG argRight,
    LONG argBottom,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    UINT% argNumberOfFramesThatCanBeCaptured)
{
    RECT areaToCapture;
    areaToCapture.left = argLeft;
    areaToCapture.top = argTop;
    areaToCapture.right = argRight;
    areaToCapture.bottom = argBottom;

    RAD::Channel channel;

    switch (argChannel)
    {
        case Channel::Left:
            channel = RAD::Channel::Left;
            break;

        case Channel::Right:
            channel = RAD::Channel::Right;
            break;

        case Channel::LeftAndRight:
            channel = RAD::Channel::LeftAndRight;
            break;

    }

    UINT numberOfFramesThatCanBeCaptured = 0;

    HRESULT hResult = this->mSlicer->CalculateMaxFramesThatCanBeCaptured(
        areaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip,
        channel,
        &numberOfFramesThatCanBeCaptured);

    if (SUCCEEDED(hResult))
    {
        argNumberOfFramesThatCanBeCaptured = numberOfFramesThatCanBeCaptured;
    }
    else
    {
        argNumberOfFramesThatCanBeCaptured = 0;
    }

    return hResult;
}


HRESULT SlicerBase::EndCapture(void)
{
    return this->mSlicer->EndCapture();
}


HRESULT SlicerBase::EndCapture(
    UINT% argFramesCaptured)
{
    UINT framesCaptured = 0;
    HRESULT hResult = this->mSlicer->EndCapture(&framesCaptured);

    if (SUCCEEDED(hResult))
    {
        argFramesCaptured = framesCaptured;
    }
    else
    {
        argFramesCaptured = 0;
    }

    return hResult;
}


HRESULT SlicerBase::EndCapture(
    UINT% argFramesCaptured,
    [Out] SystemBitmap^% argLeftImage)
{
    argFramesCaptured = 0;
    argLeftImage = nullptr;

    HRESULT hResult = NOERROR;
    UINT framesCaptured = 0;
    ID2D1Bitmap1* leftImage = nullptr;
    ID2D1Bitmap1* leftMappableImage = nullptr;

    try
    {
        hResult = this->mSlicer->EndCapture(&framesCaptured, &leftImage);

        if (SUCCEEDED(hResult))
        {
            // We might have only been capturing right channel, so leftImage
            // could be null
            if (nullptr != leftImage)
            {
                D2D1_SIZE_F sizeF = leftImage->GetSize();
                D2D1_SIZE_U sizeU = D2D1::SizeU(static_cast<UINT32>(sizeF.width), static_cast<UINT32>(sizeF.height));

                // Create a CPU readable bitmap
                hResult = this->mSlicerFactory->GetD2DContextNoRef()->CreateFrame(
                    sizeU,
                    0,
                    leftImage,
                    &leftMappableImage,
                    D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW);

                if (SUCCEEDED(hResult))
                {
                    // Map the CPU readable bitmap into memory
                    D2D1_MAPPED_RECT mappedRect = {};
                    hResult = leftMappableImage->Map(D2D1_MAP_OPTIONS_READ, &mappedRect);

                    if (SUCCEEDED(hResult))
                    {
                        try
                        {
                            // Calculate the total number of bytes in the image
                            int elements = mappedRect.pitch * sizeU.height;

                            // Allocate a new array of bytes
                            array<System::Byte>^ leftSystemMemoryBitmap = gcnew array<System::Byte>(elements);

                            // Copy from the unmanaged byte array to the managed byte array
                            System::Runtime::InteropServices::Marshal::Copy(IntPtr(mappedRect.bits), leftSystemMemoryBitmap, 0, elements);

                            // Output a new SystemBitmap
                            argLeftImage = gcnew SystemBitmap(leftSystemMemoryBitmap, sizeU.width, sizeU.height, mappedRect.pitch);
                        }
                        finally
                        {
                            leftMappableImage->Unmap();
                        }
                    }
                }
            }

            if (SUCCEEDED(hResult))
            {
                argFramesCaptured = framesCaptured;
            }
        }
    }
    finally
    {
        if (nullptr != leftMappableImage)
        {
            leftMappableImage->Release();
        }

        if (nullptr != leftImage)
        {
            leftImage->Release();
        }
    }

    return hResult;
}


HRESULT SlicerBase::EndCapture(
    UINT% argFramesCaptured,
    [Out] SystemBitmap^% argLeftImage,
    [Out] SystemBitmap^% argRightImage)
{
    argFramesCaptured = 0;
    argLeftImage = nullptr;
    argRightImage = nullptr;

    HRESULT hResult = NOERROR;
    UINT framesCaptured = 0;
    ID2D1Bitmap1* leftImage = nullptr;
    ID2D1Bitmap1* rightImage = nullptr;
    ID2D1Bitmap1* leftMappableImage = nullptr;
    ID2D1Bitmap1* rightMappableImage = nullptr;

    try
    {
        hResult = this->mSlicer->EndCapture(&framesCaptured, &leftImage, &rightImage);

        if (SUCCEEDED(hResult))
        {
            // We might have only been capturing right channel, so leftImage
            // could be null
            if (nullptr != leftImage)
            {
                D2D1_SIZE_F sizeF = leftImage->GetSize();
                D2D1_SIZE_U sizeU = D2D1::SizeU(static_cast<UINT32>(sizeF.width), static_cast<UINT32>(sizeF.height));

                // Create a CPU readable bitmap
                hResult = this->mSlicerFactory->GetD2DContextNoRef()->CreateFrame(
                    sizeU,
                    0,
                    leftImage,
                    &leftMappableImage,
                    D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW);

                if (SUCCEEDED(hResult))
                {
                    // Map the CPU readable bitmap into memory
                    D2D1_MAPPED_RECT mappedRect = {};
                    hResult = leftMappableImage->Map(D2D1_MAP_OPTIONS_READ, &mappedRect);

                    if (SUCCEEDED(hResult))
                    {
                        // Create a BitmapSource from the system unmanaged memory
                        try
                        {
                            // Calculate the total number of bytes in the image
                            int elements = mappedRect.pitch * sizeU.height;

                            // Allocate a new array of bytes
                            array<System::Byte>^ leftSystemMemoryBitmap = gcnew array<System::Byte>(elements);

                            // Copy from the unmanaged byte array to the managed byte array
                            System::Runtime::InteropServices::Marshal::Copy(IntPtr(mappedRect.bits), leftSystemMemoryBitmap, 0, elements);

                            // Output a new SystemBitmap
                            argLeftImage = gcnew SystemBitmap(leftSystemMemoryBitmap, sizeU.width, sizeU.height, mappedRect.pitch);
                        }
                        finally
                        {
                            leftMappableImage->Unmap();
                        }
                    }
                }
            }

            // We might have only been capturing left channel, so rightImage
            // could be null
            if (nullptr != rightImage)
            {
                D2D1_SIZE_F sizeF = rightImage->GetSize();
                D2D1_SIZE_U sizeU = D2D1::SizeU(static_cast<UINT32>(sizeF.width), static_cast<UINT32>(sizeF.height));

                // Create a CPU readable bitmap
                hResult = this->mSlicerFactory->GetD2DContextNoRef()->CreateFrame(
                    sizeU,
                    0,
                    rightImage,
                    &rightMappableImage,
                    D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW);

                if (SUCCEEDED(hResult))
                {
                    // Map the CPU readable bitmap into memory
                    D2D1_MAPPED_RECT mappedRect = {};
                    hResult = rightMappableImage->Map(D2D1_MAP_OPTIONS_READ, &mappedRect);

                    if (SUCCEEDED(hResult))
                    {
                        try
                        {
                            // Calculate the total number of bytes in the image
                            int elements = mappedRect.pitch * sizeU.height;

                            // Allocate a new array of bytes
                            array<System::Byte>^ rightSystemMemoryBitmap = gcnew array<System::Byte>(elements);

                            // Copy from the unmanaged byte array to the managed byte array
                            System::Runtime::InteropServices::Marshal::Copy(IntPtr(mappedRect.bits), rightSystemMemoryBitmap, 0, elements);

                            // Output a new SystemBitmap
                            argRightImage = gcnew SystemBitmap(rightSystemMemoryBitmap, sizeU.width, sizeU.height, mappedRect.pitch);
                        }
                        finally
                        {
                            rightMappableImage->Unmap();
                        }
                    }
                }
            }

            if (SUCCEEDED(hResult))
            {
                argFramesCaptured = framesCaptured;
            }
        }
    }
    finally
    {
        if (nullptr != rightMappableImage)
        {
            rightMappableImage->Release();
        }

        if (nullptr != rightImage)
        {
            rightImage->Release();
        }

        if (nullptr != leftMappableImage)
        {
            leftMappableImage->Release();
        }

        if (nullptr != leftImage)
        {
            leftImage->Release();
        }
    }

    return hResult;
}


HRESULT SlicerBase::StartCapture(
    LONG argLeft,
    LONG argTop,
    LONG argRight,
    LONG argBottom,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip)
{
    RECT areaToCapture;
    areaToCapture.left = argLeft;
    areaToCapture.top = argTop;
    areaToCapture.right = argRight;
    areaToCapture.bottom = argBottom;

    HRESULT hResult = this->mSlicer->StartCapture(
        areaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip);

    return hResult;
}


HRESULT SlicerBase::StartCapture(
    LONG argLeft,
    LONG argTop,
    LONG argRight,
    LONG argBottom,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel)
{
    RECT areaToCapture;
    areaToCapture.left = argLeft;
    areaToCapture.top = argTop;
    areaToCapture.right = argRight;
    areaToCapture.bottom = argBottom;

    RAD::Channel channel;

    switch (argChannel)
    {
        case Channel::Left:
            channel = RAD::Channel::Left;
            break;

        case Channel::Right:
            channel = RAD::Channel::Right;
            break;

        case Channel::LeftAndRight:
            channel = RAD::Channel::LeftAndRight;
            break;

    }

    HRESULT hResult = this->mSlicer->StartCapture(
        areaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip,
        channel);

    return hResult;
}


HRESULT SlicerBase::StartCapture(
    LONG argLeft,
    LONG argTop,
    LONG argRight,
    LONG argBottom,
    UINT argMaximumNumberOfFramesToCapture,
    UINT argFramesToSkip,
    Channel argChannel,
    UINT% argNumberOfFramesThatCanBeCaptured)
{
    RECT areaToCapture;
    areaToCapture.left = argLeft;
    areaToCapture.top = argTop;
    areaToCapture.right = argRight;
    areaToCapture.bottom = argBottom;

    RAD::Channel channel;

    switch (argChannel)
    {
        case Channel::Left:
            channel = RAD::Channel::Left;
            break;

        case Channel::Right:
            channel = RAD::Channel::Right;
            break;

        case Channel::LeftAndRight:
            channel = RAD::Channel::LeftAndRight;
            break;

    }

    UINT numberOfFramesThatCanBeCaptured = 0;

    HRESULT hResult = this->mSlicer->StartCapture(
        areaToCapture,
        argMaximumNumberOfFramesToCapture,
        argFramesToSkip,
        channel,
        &numberOfFramesThatCanBeCaptured);

    if (SUCCEEDED(hResult))
    {
        argNumberOfFramesThatCanBeCaptured = numberOfFramesThatCanBeCaptured;
    }
    else
    {
        argNumberOfFramesThatCanBeCaptured = 0;
    }

    return hResult;
}