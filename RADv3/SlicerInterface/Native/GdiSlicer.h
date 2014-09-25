//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      The GdiSlicer uses GDI methods to capture slices of the screen.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

#include "IGdiSlicer.h"
#include "SlicerFactory.h"

namespace RAD
{
    /// <summary>
    /// The GdiSlicer uses GDI methods to capture slices of the screen.
    /// </summary>
    class GdiSlicer : public IGdiSlicer
    {
    // Constructor
    private:
        GdiSlicer(void);

        HRESULT Initialize(
            std::shared_ptr<ID2D1Device> argD2DDevice,
            D3D_FEATURE_LEVEL argD2DFeatureLevel);


    // Destructor
    public:
        ~GdiSlicer(void);


    // IUnknown Methods
    public:
        ULONG STDMETHODCALLTYPE AddRef(void);

        ULONG STDMETHODCALLTYPE Release(void);

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID riid,
            LPVOID* ppvObj);


    // Public Methods
    public:
        bool STDMETHODCALLTYPE IsSlicing(void);

        HRESULT STDMETHODCALLTYPE CalculateMaxFramesThatCanBeCaptured(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            __out UINT* argNumberOfFramesThatCanBeCaptured);

        HRESULT STDMETHODCALLTYPE EndCapture(
            __out UINT* argFramesCaptured = nullptr,
            __out ID2D1Bitmap1** argLeftImage = nullptr,
            __out ID2D1Bitmap1** argRightImage = nullptr);

        HRESULT STDMETHODCALLTYPE StartCapture(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel = static_cast<Channel>(1),
            __out UINT* argNumberOfFramesThatCanBeCaptured = nullptr);


    // Private Methods
    private:
        HRESULT AllocateResources(
            Channel argChannel,
            UINT argFrameWidth,
            UINT argFrameHeight,
            UINT argRequestedFramesPerRow,
            UINT argRequestedRows,
            __out UINT* argActualFramesPerRow,
            __out UINT* argActualRows);

        void CleanupResources(void);

        HRESULT CreateTexturesForOutputs(
            Channel argChannel,
            UINT argTextureWidth,
            UINT argTextureHeight);

        HRESULT InternalStartCapture(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            __out UINT* argNumberOfFramesThatCanBeCaptured,
            bool argTest);


    // Private Data Members
    private:
        // Initialized once in constructor
        SRWLOCK                             mFunctionSyncLock;
        HANDLE                              mStopSlicingEvent;

        // Shared between threads
        RECT                                mAreaToCapture;
        SIZE                                mTextureSize;
        UINT                                mFramesCaptured;
        UINT                                mFramesToSkip;
        UINT                                mMaximumNumberOfFramesToCapture;
        std::unique_ptr<BYTE[]>             mSystemMemoryTexture;

        // Thread Info
        HANDLE                              mThreadHandle;
        DWORD                               mThreadId;

        // DirectX objects
        std::shared_ptr<ID2D1Bitmap1>       mGpuBitmapLeft;
        std::shared_ptr<ID2D1Bitmap1>       mGpuBitmapRight;
        std::shared_ptr<ID2D1Device>        mD2DDevice;
        std::shared_ptr<ID2D1DeviceContext> mD2DDeviceContext;
        D3D_FEATURE_LEVEL                   mD2DFeatureLevel;

        // Slicer state
        bool                                mSlicing;

        // IUnknown ref count
        LONG                                mRefCount;

        // Friends
        friend class SlicerFactory;


    // Private Static Methods
    private:
        static DWORD WINAPI ExecuteSlicing(LPVOID argParameter);

    };
}