#pragma once

using namespace DirectUI;

class CSlicer : public CDuiMainWindow<CSlicer>,
                public IElementListener
{
public:
    DEFINE_CLASSINFO();
    static const int IDRMARKUP = 1000;

    CSlicer();
    HRESULT Initialize(_In_ HINSTANCE hInstance);

    // IElementListener
    virtual void OnListenerAttach(_In_ Element *peFrom) {}
    virtual void OnListenerDetach(_In_ Element *peFrom) {}
    virtual bool OnListenedPropertyChanging(_In_ Element *peFrom, _In_ PropertyInfo const *ppi, _In_ int iIndex, _In_ Value *pvOld, _In_ Value *pvNew) { return true; }
    virtual void OnListenedPropertyChanged(_In_ Element *peFrom, _In_ PropertyInfo const *ppi, _In_ int iIndex, _In_ Value *pvOld, _In_ Value *pvNew) {};
    virtual void OnListenedInput(_In_ Element *peFrom, _In_ InputEvent *pInput) {};
    virtual void OnListenedEvent(_In_ Element *peFrom, _In_ Event *pEvent);

    void CommandLineLaunch(_In_ PWSTR pszImageFile, _In_opt_ UINT *pnWidth, _In_opt_ UINT *pnHeight);

    // Callback from Region Select
    void RectSelectEvent(_In_ RECT *pRect);
    void SetInCaptureMode(bool fCapture) { _fCapture = fCapture; }
    bool GetCaptureMode() { return _fCapture; }

protected:
    HRESULT CreateUI();

private: // Member functions
    HRESULT _InitializeSlicer();
    HRESULT _InitializeElementMembers();
    HRESULT _InitializeTimer();
    HRESULT _InitializeRegionSelect();
    HRESULT _StartCapture();
    void    _EndCapture();

    HRESULT _ShowViewer();
    HRESULT _HideViewer();
    HRESULT _ProcessImage(_In_ PWSTR pszImageFile, _In_opt_ UINT *puFrameWidth, _In_opt_ UINT *puFrameHeight);

private: // Member variables

    // DUI Elements
    Element *_peRoot;
    Element *_peCaptureButton;
    Element *_peSelectRegionButton;
    Element *_peLeftValue;
    Element *_peRightValue;
    Element *_peTopValue;
    Element *_peBottomValue;
    Element *_peCaptureDuration;
    Element *_peMaxFrames;
    Element *_peLocation;
    Element *_peCaptureUI;
    Element *_peCoverSheet;
    Element *_peSlicerViewerCommandString;
    Element *_peSwitchButton;
    Element *_peViewUI;
    Element *_peViewContainer;
    TouchScrollViewer *_peTSV;

    // Timer
    SimpleTimer *_pTimer;

    // Settings
    UINT _nCaptureHeight;
    UINT _nCaptureWidth;

    // Slicer Factory
    CComPtr<RADv2::ISlicerFactory> _slicerFactory;
    CComPtr<RADv2::IDwmSlicer>     _dwmSlicer;
    CComPtr<ID2D1Bitmap1>          _captureResultLeftChannel;
    CComPtr<ID2D1Bitmap1>          _captureResultRightChannel;
    CComPtr<ID2D1Bitmap1>          _frame;

    // Region Select
    CRegionSelect *_pRegionSelect;
    bool _fCapture;
    bool _fValidCapture;
    wchar_t _szLastImage[MAX_PATH];
};