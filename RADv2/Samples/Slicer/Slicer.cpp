#include "precomp.h"
#include "Slicer.h"
#include "DUIHelpers.h"

using namespace RADv2;
using namespace DirectUI;

HINSTANCE g_hInstance;

// TODO: Need to find a good permanent home for Slicer Viewer since windesign may be dissolved or cleaned in the future.
// Ideally Slicer Capture and a new version of Slicer Viewer will be added to the build in the Windows 9 time frame and
// added to http://toolbox
const wchar_t c_szSlicerViewer[] = L"\\\\windesign\\modern\\wicarr\\slicer\\SlicerViewer.exe";

IMPLEMENT_CLASSINFO(CSlicer);

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/)
{
    g_hInstance = hInstance;
    CDuiInitializer dui;

    HRESULT hr = dui.Initialize(COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr))
    {
        hr = CSlicer::Register();
    }

    if (SUCCEEDED(hr))
    {
        hr = RegisterPVLBehaviorFactory();
    }

    if (SUCCEEDED(hr))
    {
        hr = RegisterImmersiveBehaviors();
    }

    if (SUCCEEDED(hr))
    {
        CSlicer *pApp = nullptr;
        hr = CSlicer::Create(hInstance, &pApp);
        if (SUCCEEDED(hr))
        {            
            // Check command line
            int nCmdLineArgCount = 0;
            LPWSTR* pszCommandLineArgs = CommandLineToArgvW(lpCmdLine, &nCmdLineArgCount);

            if (nCmdLineArgCount == 1)
            {                
                PWSTR pszImagePath = pszCommandLineArgs[0];
                pApp->CommandLineLaunch(pszImagePath, nullptr, nullptr);
            }
            else if (nCmdLineArgCount == 3)
            {
                PWSTR pszImagePath = pszCommandLineArgs[0];
                PWSTR pszFrameWidth = pszCommandLineArgs[1];
                PWSTR pszFrameHeight = pszCommandLineArgs[2];

                UINT width = static_cast<UINT>(_wtoi(pszFrameWidth));
                UINT height = static_cast<UINT>(_wtoi(pszFrameHeight));

                // TODO: Should verify pszImagePath
                if ((width != 0) && (height != 0))
                {
                    pApp->CommandLineLaunch(pszImagePath, &width, &height);
                }
            }

            StartMessagePump();
        }
    }

    return hr;
}

/*---------------------------------------------------------------------------*/

CSlicer::CSlicer() :
_fValidCapture(false)
{
}

// Initialize application
HRESULT CSlicer::Initialize(_In_ HINSTANCE hInstance)
{
    return __super::Initialize(hInstance, L"Slicer", CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, WS_OVERLAPPEDWINDOW);
}

// Initialize UI
HRESULT CSlicer::CreateUI()
{
    HRESULT hr = S_OK;

    SetAccessible(true);
    SetActive(AE_Mouse|AE_Pointer|AE_Keyboard);

    hr = GetParser()->CreateElement(L"Main", nullptr, this, nullptr, &_peRoot);
    if (SUCCEEDED(hr))
    {
        hr = Add(_peRoot);
    }

    if (SUCCEEDED(hr))
    {
        hr = _InitializeSlicer();
    }

    if (SUCCEEDED(hr))
    {
        hr = _InitializeElementMembers();
    }

    if (SUCCEEDED(hr))
    {
        hr = _InitializeTimer();
    }

    if (SUCCEEDED(hr))
    {
        hr = _InitializeRegionSelect();
    }

    return hr;
}

HRESULT CSlicer::_InitializeSlicer()
{
    HRESULT hr = ::CreateSlicerFactory(&_slicerFactory, ComputeShaderModel::None);
    if (SUCCEEDED(hr))
    {
        hr = _slicerFactory->CreateDwmSlicer(&_dwmSlicer);
    }
    return hr;
}

HRESULT CSlicer::_InitializeElementMembers()
{
    _peCaptureButton = _peRoot->FindDescendent(StrToID(L"CaptureButton"));
    _peSelectRegionButton = _peRoot->FindDescendent(StrToID(L"SelectRegionButton"));
    _peLeftValue = _peRoot->FindDescendent(StrToID(L"RectLeft"));
    _peRightValue = _peRoot->FindDescendent(StrToID(L"RectRight"));
    _peTopValue = _peRoot->FindDescendent(StrToID(L"RectTop"));
    _peBottomValue = _peRoot->FindDescendent(StrToID(L"RectBottom"));
    _peCaptureDuration = _peRoot->FindDescendent(StrToID(L"CaptureDuration"));
    _peMaxFrames = _peRoot->FindDescendent(StrToID(L"MaxFrames"));
    _peLocation = _peRoot->FindDescendent(StrToID(L"SaveLocation"));
    _peCoverSheet = _peRoot->FindDescendent(StrToID(L"CoverSheet"));
    _peCaptureUI = _peRoot->FindDescendent(StrToID(L"Capture"));
    _peSlicerViewerCommandString = _peRoot->FindDescendent(StrToID(L"SlicerViewerCommandString"));
    _peViewUI = _peRoot->FindDescendent(StrToID(L"View"));
    _peSwitchButton = _peRoot->FindDescendent(StrToID(L"Switch"));
    _peViewContainer = _peRoot->FindDescendent(StrToID(L"ViewContainer"));
    _peTSV = element_cast<TouchScrollViewer>(_peRoot->FindDescendent(StrToID(L"ScrollViewer")));
    _peRoot->AddListener(this);
    _peRoot->SetVisible(true);

    return S_OK;
}

HRESULT CSlicer::_InitializeTimer()
{
    // Initialize our timer
    _pTimer = new SimpleTimer(10.0f, DTR_NO_REPEAT);
    HRESULT hr = (_pTimer != nullptr) ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        _pTimer->SetHandler([this] { _EndCapture(); });
    }
    return hr;
}

HRESULT CSlicer::_InitializeRegionSelect()
{
    _pRegionSelect = new CRegionSelect(g_hInstance, this);
    HRESULT hr = (_pRegionSelect != nullptr) ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        _pRegionSelect->Init();
    }
    return hr;
}

void CSlicer::CommandLineLaunch(_In_ PWSTR pszImageFile, _In_opt_ UINT *puWidth, _In_opt_ UINT *puHeight)
{
    HRESULT hr = _ProcessImage(pszImageFile, puWidth, puHeight);
    if (SUCCEEDED(hr))
    {        
        _ShowViewer();
        _peCaptureUI->SetVisible(false);
        _peCoverSheet->SetVisible(false);
    }
}

void CSlicer::OnListenedEvent(_In_ Element* peFrom, _In_ Event* pEvent)
{
    if (pEvent->nStage == GMF_BUBBLED)
    {
        if (pEvent->uidType == Button::Click || pEvent->uidType == TouchButton::Click)
        {
            if (pEvent->peTarget == _peCaptureButton)
            {
                // Capture button pressed - do a capture
                _StartCapture();
            }
            else if (pEvent->peTarget == _peSelectRegionButton)
            {
                // Show Region Select
                _pRegionSelect->SetVisible(true);
            }
            else if (pEvent->peTarget == _peSwitchButton)
            {
                if (_peViewUI->GetVisible())
                {
                    _HideViewer();
                    _peCaptureUI->SetVisible(true);
                    _peCoverSheet->SetVisible(false);
                }
                else
                {
                    _ShowViewer();
                    _peCaptureUI->SetVisible(false);
                    _peCoverSheet->SetVisible(false);
                }
            }
        }
    }
}

HRESULT CSlicer::_StartCapture()
{
   // Get user variables
    CValuePtr spLeft;
    PCWSTR left = _peLeftValue->GetContentString(&spLeft);

    CValuePtr spRight;
    PCWSTR right = _peRightValue->GetContentString(&spRight);

    CValuePtr spTop;
    PCWSTR top = _peTopValue->GetContentString(&spTop);

    CValuePtr spBottom;
    PCWSTR bottom = _peBottomValue->GetContentString(&spBottom);

    CValuePtr spDuration;
    PCWSTR duration = _peCaptureDuration->GetContentString(&spDuration);

    CValuePtr spFrames;
    PCWSTR maxframes = _peMaxFrames->GetContentString(&spFrames);

    int nLeft = _wtoi(left);
    int nRight = _wtoi(right);
    int nTop = _wtoi(top);
    int nBottom = _wtoi(bottom);
    UINT uDuration = _wtoi(duration);
    UINT uMaxFrames = _wtoi(maxframes);

    // Catch improper inputs here
    if (nLeft > nRight)
    {
        int nTemp = nLeft;
        nLeft = nRight;
        nRight = nTemp;
    }

    if (nTop > nBottom)
    {
        int nTemp = nTop;
        nTop = nBottom;
        nBottom = nTemp;
    }

    HRESULT hr = _dwmSlicer->StartCapture(RADv2::Rect(nLeft, nTop, nRight, nBottom), uMaxFrames, 0);
    if (SUCCEEDED(hr))
    {
        _pTimer->SetDuration(static_cast<float>(uDuration));
        _pTimer->Start();

        // Update UI to show the cover sheet since we are now in the middle of a capture
        _peCaptureUI->SetVisible(false);
        _peCoverSheet->SetVisible(true);

        _nCaptureWidth = nRight - nLeft;
        _nCaptureHeight = nBottom - nTop;
    }

    return hr;
}


void CSlicer::_EndCapture()
{
    UINT framesCaptured;
    HRESULT hrTemp = _dwmSlicer->EndCapture(&framesCaptured, &_captureResultLeftChannel);
    if (SUCCEEDED(hrTemp))
    {
        CValuePtr spLocation;
        PCWSTR pszSaveLocation = _peLocation->GetContentString(&spLocation);

        // TODO: If pszSaveLocation's directory does not exist we should CreateDirectory it here
        int nFrameWidth = static_cast<int>(_nCaptureWidth);
        int nFrameHeight = static_cast<int>(_nCaptureHeight);

        wchar_t szWidth[10];
        wchar_t szHeight[10];

        StringCchPrintf(szWidth, ARRAYSIZE(szWidth), L"%d", _nCaptureWidth);
        StringCchPrintf(szHeight, ARRAYSIZE(szHeight), L"%d", _nCaptureHeight);

        hrTemp = _slicerFactory->SaveImageToFile(pszSaveLocation, _captureResultLeftChannel, GUID_ContainerFormatPng, szWidth, szHeight);
        if (SUCCEEDED(hrTemp))
        {
            // Set the slicer viewer command string
            wchar_t szSlicerViewerLaunchCommand[1024];
            StringCchPrintf(szSlicerViewerLaunchCommand,
                            ARRAYSIZE(szSlicerViewerLaunchCommand),
                            L"%s %s %d %d",
                            c_szSlicerViewer,
                            pszSaveLocation,
                            _nCaptureWidth,
                            _nCaptureHeight);
            _peSlicerViewerCommandString->SetContentString(szSlicerViewerLaunchCommand);

            StringCchPrintf(_szLastImage, ARRAYSIZE(_szLastImage), L"%s", pszSaveLocation);
            _fValidCapture = true;
        }
    }

    // Update UI to show Controls since we are no longer capturing
    _peCaptureUI->SetVisible(true);
    _peCoverSheet->SetVisible(false);
}

void CSlicer::RectSelectEvent(_In_ RECT *pRect)
{
    // Update rect fields
    int const charCount = 6;

    wchar_t wszLeft[charCount];
    wchar_t wszTop[charCount];
    wchar_t wszRight[charCount];
    wchar_t wszBottom[charCount];

    StringCchPrintf(wszLeft, charCount, L"%d", pRect->left);
    StringCchPrintf(wszTop, charCount, L"%d", pRect->top);
    StringCchPrintf(wszRight, charCount, L"%d", pRect->right);
    StringCchPrintf(wszBottom, charCount, L"%d", pRect->bottom);

    _peLeftValue->SetContentString(wszLeft);
    _peTopValue->SetContentString(wszTop);
    _peRightValue->SetContentString(wszRight);
    _peBottomValue->SetContentString(wszBottom);

    _pRegionSelect->SetVisible(false);
}

HRESULT CSlicer::_ShowViewer()
{
    HRESULT hr = _peViewUI->SetVisible(true);

    if (SUCCEEDED(hr) && _fValidCapture)
    {
        // Process image file
        hr = _ProcessImage(_szLastImage, &_nCaptureWidth, &_nCaptureHeight);
    }

    return hr;
}

HRESULT CSlicer::_HideViewer()
{
    HRESULT hr = _peViewUI->SetVisible(false);

    // Destroy all children of the viewer
    _peViewContainer->DestroyAll();

    return hr;
}

HRESULT CSlicer::_ProcessImage(_In_ PWSTR pszImageFile, _In_opt_ UINT *puFrameWidth, _In_opt_ UINT *puFrameHeight)
{
    int uFrameWidth, uFrameHeight = 0;

    // Load Image
    IWICImagingFactory *pIWICFactory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,
                          NULL,
                          CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&pIWICFactory));

    if (SUCCEEDED(hr))
    {
        IWICBitmapDecoder *pIDecoder;
        hr = pIWICFactory->CreateDecoderFromFilename(pszImageFile,                   // Image to be decoded
                                                     NULL,                           // Do not prefer a particular vendor
                                                     GENERIC_READ,                   // Desired read access to the file
                                                     WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
                                                     &pIDecoder);                    // Pointer to the decoder
        if (SUCCEEDED(hr))
        {
            IWICBitmapFrameDecode *pIDecoderFrame;
            // Retrieve the first bitmap frame.
            hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
            if (SUCCEEDED(hr))
            {
                if ((puFrameWidth != nullptr) && (puFrameHeight != nullptr))
                {
                    uFrameWidth = *puFrameWidth;
                    uFrameHeight = *puFrameHeight;
                }
                else
                {
                    IWICMetadataQueryReader *pIQueryReader;
                    hr = pIDecoderFrame->GetMetadataQueryReader(&pIQueryReader);
                    if (SUCCEEDED(hr))
                    {
                        PROPVARIANT pvWidth;
                        PropVariantInit(&pvWidth);
                        hr = pIQueryReader->GetMetadataByName(L"/[*]tEXt/{str=FrameWidth}", &pvWidth);
                        if (SUCCEEDED(hr))
                        {
                            // Need to split this string since it is "%d %d" width and height respectively
                            PROPVARIANT pvHeight;
                            PropVariantInit(&pvHeight);
                            hr = pIQueryReader->GetMetadataByName(L"/[*]tEXt/{str=FrameHeight}", &pvHeight);
                            if (SUCCEEDED(hr))
                            {
                                uFrameWidth = static_cast<UINT>(atoi(pvWidth.pszVal));
                                uFrameHeight = static_cast<UINT>(atoi(pvHeight.pszVal));
                            }
                            PropVariantClear(&pvHeight);
                        }
                        PropVariantClear(&pvWidth);
                        pIQueryReader->Release();
                    }
                }

                hr = (uFrameWidth != 0) && (uFrameHeight != 0) ? hr : E_FAIL;

                if (SUCCEEDED(hr))
                {
                    IWICBitmap *pBitmapOriginal;
                    hr = pIWICFactory->CreateBitmapFromSource(pIDecoderFrame, WICBitmapCacheOnLoad, &pBitmapOriginal);
                    if (SUCCEEDED(hr))
                    {
                        UINT uImageWidth, uImageHeight;
                        hr = pBitmapOriginal->GetSize(&uImageWidth, &uImageHeight);
                        if (SUCCEEDED(hr))
                        {
                            UINT columns = uImageWidth / uFrameWidth;
                            UINT rows = uImageHeight / uFrameHeight;

                            AutoDefer ad(this);

                            for (UINT uCurrentRow = 0; uCurrentRow < rows; uCurrentRow++)
                            {
                                for (UINT uCurrentColumn = 0; uCurrentColumn < columns; uCurrentColumn++)
                                {
                                    IWICBitmapClipper *pIClipper;
                                    hr = pIWICFactory->CreateBitmapClipper(&pIClipper);
                                    if (SUCCEEDED(hr))
                                    { 
                                        UINT x = uCurrentColumn * uFrameWidth;
                                        UINT y = uCurrentRow * uFrameHeight;
                                        WICRect rcClip = {x, y, uFrameWidth, uFrameHeight};
                                        // Initialize the clipper with the given rectangle of the frame's image data.
                                        hr = pIClipper->Initialize(pBitmapOriginal, &rcClip);
                                        if (SUCCEEDED(hr))
                                        {
                                            IWICBitmap *pBitmap;
                                            hr = pIWICFactory->CreateBitmapFromSource(pIClipper, WICBitmapCacheOnLoad, &pBitmap);
                                            if (SUCCEEDED(hr))
                                            {
                                                HBITMAP hbmpResource;
                                                hr = ConvertIWICBitmapSourceTo32bppHBITMAP(pBitmap, pIWICFactory, &hbmpResource);
                                                if (SUCCEEDED(hr))
                                                {
                                                    // Create a new frame element
                                                    Element *peElement = nullptr;
                                                    hr = GetParser()->CreateElement(L"Frame", nullptr, _peViewContainer, nullptr, &peElement);
                                                    if (SUCCEEDED(hr))
                                                    {
                                                        Element *peImage = peElement->FindDescendent(StrToID(L"Image"));
                                                        Element *peNumber = peElement->FindDescendent(StrToID(L"Number"));
                                                        if ((peImage != nullptr) && (peNumber != nullptr))
                                                        {
                                                            hr = DUI_SetElementBitmap(peImage, hbmpResource, GRAPHIC_NoBlend, (UINT)-1, false);
                                                            if (SUCCEEDED(hr))
                                                            {
                                                                peElement->SetLayoutPos(0);
                                                                wchar_t szFrameNumber[10];
                                                                StringCchPrintf(szFrameNumber, ARRAYSIZE(szFrameNumber), L"%d", (uCurrentColumn + uCurrentRow * columns + 1));
                                                                peNumber->SetContentString(szFrameNumber);
                                                                hr = _peViewContainer->Add(peElement);
                                                            }
                                                        }
                                                    }
                                                }
                                                pBitmap->Release();
                                            }
                                        }
                                        pIClipper->Release();
                                    }
                                }
                            }
                        }
                        pBitmapOriginal->Release();
                    }
                }
                pIDecoderFrame->Release();
            }
            pIDecoder->Release();
        }
        pIWICFactory->Release();
    }

    if (SUCCEEDED(hr) && (_peTSV != nullptr))
    {
        Element *pe = nullptr;
        ScrollBar *peHScroll = nullptr;
        _peTSV->GetHScrollbar(&pe);
        peHScroll = element_cast<ScrollBar>(pe);
        if (peHScroll != nullptr)
        {
            // TODO: Query border width instead of assuming 1 pixel border
            peHScroll->SetLine(uFrameWidth + 2); // + 2 to account for border pixels
        }
    }

    return hr;
}