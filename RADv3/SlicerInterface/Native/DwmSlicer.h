//+-----------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Description:
//      The DwmSlicer uses the SlicerControl method to capture slices of the
//      screen.
//
//------------------------------------------------------------------------------

#pragma once

#include "Stdafx.h"

#include "IDwmSlicer.h"
#include "SlicerFactory.h"

namespace RAD
{
    /// <summary>
    /// The DwmSlicer uses the SlicerControl method to capture slices of the
    /// screen.
    /// </summary>
    class DwmSlicer : public IDwmSlicer
    {
    // Typedef
    private:
        typedef std::map<std::shared_ptr<IDXGIAdapter>, std::shared_ptr<ID3D11Device>>                 AdapterToDeviceMap;
        typedef std::map<std::shared_ptr<IDXGIAdapter>, std::vector<std::shared_ptr<IDXGIOutput>>>     AdapterToOutputsMap;
        typedef std::map<std::shared_ptr<ID3D11Device>, std::shared_ptr<ID3D11Texture2D>>              DeviceToTextureMap;
        typedef std::map<std::shared_ptr<ID3D11Device>, std::vector<std::shared_ptr<ID3D11Texture2D>>> DeviceToTexturesMap;


    // Constructor
    private:
        DwmSlicer(void);

        HRESULT Initialize(
            const std::vector<std::shared_ptr<IDXGIAdapter>>& argAdapters,
            const std::vector<std::shared_ptr<ID3D11Device>>& argDevices,
            const std::shared_ptr<ID2D1Device>& argD2DDevice);


    // Destructor
    public:
        virtual ~DwmSlicer(void);


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
            const AdapterToOutputsMap& argAdapterToOutputsMap,
            Channel argChannel,
            UINT argFrameWidth,
            UINT argFrameHeight,
            UINT argRequestedFramesPerRow,
            UINT argRequestedRows,
            __out UINT* argActualFramesPerRow,
            __out UINT* argActualRows,
            __out std::vector<SLICER_SHARED_TEXTURE>* argSharedTextures);

        void CleanupResources(void);

        HRESULT CombineTextures(
            const DeviceToTexturesMap& argTextureMap,
            ID2D1Bitmap1* argGpuBitmap,
            __out ID2D1Bitmap1** argTextureOut);

        HRESULT CopyFromSystemMemory(
            HANDLE argSystemMemoryHandle,
            ULONG argLength,
            __out ID2D1Bitmap1** argBitmapOut);

        HRESULT CreateTexturesForOutputs(
            const AdapterToOutputsMap& argAdapterToOutputsMap,
            Channel argChannel,
            UINT argTextureWidth,
            UINT argTextureHeight,
            __out std::vector<SLICER_SHARED_TEXTURE>* argSharedTextures);

        HRESULT GetLowestFeatureLevelFromAdapters(
            const AdapterToOutputsMap& argAdapterToOutputsMap,
            __out D3D_FEATURE_LEVEL* argLowestFeatureLevel);

        UINT GetMaxTextureSizeFromFeatureLevel(
            D3D_FEATURE_LEVEL argFeatureLevel);

        HRESULT GetOutputsFromScreenRect(
            const RECT& argScreenRect,
            __out AdapterToOutputsMap* argAdapterToOutputsMap);

        HRESULT InternalStartCapture(
            RECT argAreaToCapture,
            UINT argMaximumNumberOfFramesToCapture,
            UINT argFramesToSkip,
            Channel argChannel,
            __out UINT* argNumberOfFramesThatCanBeCaptured,
            bool argTest);


    // Private Typedef
    private:
        typedef BOOL (*SlicerControlApi)(
            _In_opt_ HWND hWndContext,
            _In_ SLICER_COMMAND Command,
            _In_reads_bytes_(ulInformationLength) PVOID pInformationBuffer,
            _In_ ULONG ulInformationLength);


    // Private Data Members
    private:
        // Initialized once in the contrustor
        SRWLOCK                             mFunctionSyncLock;
        HMODULE                             mModuleHandle;

        // Set during Initialize
        AdapterToDeviceMap                  mAdapterToDeviceMap;
        std::shared_ptr<ID2D1Device>        mD2DDevice;
        std::shared_ptr<ID2D1DeviceContext> mD2DDeviceContext;
        SlicerControlApi                    mSlicerControl;
        HMODULE                             mUser32;
        ATOM                                mWindowClass;
        HWND                                mWindowHandle;

        // Shared textures passed to the slicer in BeginCapture
        DeviceToTexturesMap                 mSharedTexturesLeft;
        DeviceToTexturesMap                 mSharedTexturesRight;

        // Staging textures for copying the shared textures from GPU to CPU
        DeviceToTextureMap                  mStagingTextures;

        // CPU staging bitmap for combining the textures on the CPU
        std::unique_ptr<BYTE[]>             mCpuBitmap;

        // GPU bitmaps that are returned in EndCapture
        std::shared_ptr<ID2D1Bitmap1>       mGpuBitmapLeft;
        std::shared_ptr<ID2D1Bitmap1>       mGpuBitmapRight;

        // Slicer state
        bool                                mSlicing;

        // IUnknown ref count
        LONG                                mRefCount;

        // Friends
        friend class SlicerFactory;

        #ifdef DBG
        void Save(const std::wstring& argFileName, ID3D11Texture2D* argTexture);
        #endif

    };
}