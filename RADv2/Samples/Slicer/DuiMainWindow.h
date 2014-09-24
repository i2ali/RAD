// DuiMainWindow.h
//
// This contains a helper class for creating a MainWindow DUI application.
//
#pragma once
#include "shellapi.h"
#include "directui.h"
#include <time.h>

template <class TWindow, typename TBase = DirectUI::TouchHWNDElement>
class CDuiMainWindow : public TBase
{
    public:
        CDuiMainWindow();
        ~CDuiMainWindow();
        virtual HRESULT Initialize(_In_ HINSTANCE hInstance) { return CDuiMainWindow::Initialize(hInstance, nullptr, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, WS_OVERLAPPEDWINDOW); }
        static HRESULT Register();
        static HRESULT Create(_In_ HINSTANCE hInstance, _COM_Outptr_ TWindow **ppElement);

    protected:
        virtual HRESULT CreateUI() { return S_OK; }
        inline DirectUI::DUIXmlParser *GetParser() { return _pParser; }
        inline DirectUI::NativeHWNDHost *GetHost() { return _pNativeHost; }
        inline HINSTANCE GetHInstance() { return _hInstance; }
        HRESULT Initialize(_In_ HINSTANCE hInstance, _In_opt_ PCWSTR wzCaption, int x, int y, int cx, int cy, DWORD dwExStyles, DWORD dwStyles);
        double GetParseTime() const { return _parseTime; }

    private:
        HRESULT CreateParser();
        HRESULT CreateLayout();
        static bool IsOkToIgnoreAttribute(_In_ PCWSTR wzAttrName, _In_opt_ void *pContext);

    private:
        HINSTANCE _hInstance;
        DirectUI::DUIXmlParser *_pParser;
        DirectUI::NativeHWNDHost *_pNativeHost;
        double _parseTime;
};

/*---------------------------------------------------------------------------*/

template <class TWindow, typename TBase>
CDuiMainWindow<TWindow, TBase>::CDuiMainWindow() :
    _hInstance(nullptr),
    _pNativeHost(nullptr),
    _pParser(nullptr)
{
}

template <class TWindow, typename TBase>
CDuiMainWindow<TWindow, TBase>::~CDuiMainWindow()
{
    _pNativeHost = nullptr;
    if (_pParser != nullptr)
    {
        _pParser->Destroy();
        _pParser = nullptr;
    }
}

// Initialize our application
// You can use "-noalpha" on the command line to disable the alpha channel
template <class TWindow, typename TBase>
HRESULT CDuiMainWindow<TWindow, TBase>::Initialize(_In_ HINSTANCE hInstance, _In_opt_ PCWSTR wzCaption, int x, int y, int cx, int cy, DWORD dwExStyles, DWORD dwStyles)
{
    HRESULT hr = S_OK;
    DWORD dwDeferCookie = 0;
    DWORD dwEc = EC_PreserveAlphaChannel;

    PCWSTR pszCmdLine = GetCommandLine();
    if ((pszCmdLine != nullptr) && (wcslen(pszCmdLine) != 0))
    {
        int cArgs;
        PWSTR *rgArgs = CommandLineToArgvW(pszCmdLine, &cArgs);
        for (int iArg = 0; iArg < cArgs; iArg++)
        {
            if ((*rgArgs[iArg] == L'-') || (*rgArgs[iArg] == L'/'))
            {
                PCWSTR pszArg = rgArgs[iArg] + 1;

                if (CSTR_EQUAL == CompareStringOrdinal(pszArg, -1, L"noalpha", -1, true))
                {
                   dwEc &= ~EC_PreserveAlphaChannel;
                }
                else if (CSTR_EQUAL == CompareStringOrdinal(pszArg, -1, L"notouch", -1, true))
                {
                    dwEc &= ~EC_MultiTouch;
                }
            }
        }

        LocalFree(rgArgs);
    }


    _hInstance = hInstance;
    hr = DirectUI::NativeHWNDHost::Create(wzCaption, nullptr, LoadIcon(nullptr, MAKEINTRESOURCE(IDI_APPLICATION)), x, y, cx, cy, dwExStyles, dwStyles, 0, &_pNativeHost);
    if (SUCCEEDED(hr))
    {
        hr = CreateParser();
    }
    if (SUCCEEDED(hr))
    {
        hr = TBase::Initialize(_pNativeHost->GetHWND(), true, dwEc, nullptr, &dwDeferCookie);
    }
    if (SUCCEEDED(hr))
    {
        SetVisible(true);
        SetBackgroundColor(ARGB(0, 0, 0, 0));
        hr = CreateLayout();
    }
    if (SUCCEEDED(hr))
    {
        hr = CreateUI();
    }
    if (SUCCEEDED(hr))
    {
        _pNativeHost->Host(this);
        EndDefer(dwDeferCookie);
        _pNativeHost->ShowWindow();
        SetKeyFocus();
    }
    return hr;
}

// Determine if we can ignore the attribute or not. We ignore all "loc." attributes
template <class TWindow, typename TBase>
bool CDuiMainWindow<TWindow, TBase>::IsOkToIgnoreAttribute(_In_ PCWSTR wzAttrName, _In_opt_ void* /*pContext*/)
{
    int cchAttrName = wcslen(wzAttrName);
    if ((cchAttrName > 4) && (CSTR_EQUAL == CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, wzAttrName, 4, L"loc.", 4)))
    {
        return true;
    }
    return false;
}

// Create our parser
template <class TWindow, typename TBase>
HRESULT CDuiMainWindow<TWindow, TBase>::CreateParser()
{
    HRESULT hr = S_OK;

    LARGE_INTEGER liStart;
    QueryPerformanceCounter(&liStart);

    hr = DirectUI::DUIXmlParser::Create(&_pParser);
    if (SUCCEEDED(hr))
    {
        _pParser->EnableDesignMode();
        _pParser->SetUnavailableIcon(LoadIcon(nullptr, MAKEINTRESOURCE(IDI_WARNING)));
        _pParser->SetUnknownAttrCallback(&IsOkToIgnoreAttribute, nullptr);
        __if_exists(TWindow::IDRMARKUP)
        {
            hr = _pParser->SetXMLFromResource(TWindow::IDRMARKUP, _hInstance);
        }
    }

    LARGE_INTEGER liEnd, liFreq;
    QueryPerformanceCounter(&liEnd);
    QueryPerformanceFrequency(&liFreq);
    liStart.QuadPart = liEnd.QuadPart - liStart.QuadPart;
    liFreq.QuadPart = liFreq.QuadPart / 1000;
    _parseTime = (1.0 * liStart.QuadPart) / liFreq.QuadPart;

    return hr;
}

// Create our base layout
template <class TWindow, typename TBase>
HRESULT CDuiMainWindow<TWindow, TBase>::CreateLayout()
{
    Layout *pLayout = nullptr;
    HRESULT hr = DirectUI::FillLayout::Create(&pLayout);
    if (SUCCEEDED(hr))
    {
        hr = SetLayout(pLayout);
    }
    if (FAILED(hr) && (pLayout != nullptr))
    {
        pLayout->Destroy();
    }

    return hr;
}

// Register our DUI Class
template <class TWindow, typename TBase>
HRESULT CDuiMainWindow<TWindow, TBase>::Register()
{
    return ClassInfo<TWindow, TBase, DirectUI::EmptyCreator<TWindow>>::Register(L"CDuiMainWindow", nullptr, 0);
}

// Create a new instance of the application
template <class TWindow, typename TBase>
HRESULT CDuiMainWindow<TWindow, TBase>::Create(_In_ HINSTANCE hInstance,  _COM_Outptr_ TWindow **ppElement)
{
    HRESULT hr = E_OUTOFMEMORY;

    // Param validation
    if (ppElement == nullptr)
    {
        return E_INVALIDARG;
    }
    *ppElement = nullptr;

    TWindow *pThis = DirectUI::HNew<TWindow>();
    if (pThis != nullptr)
    {
        *ppElement = pThis;
        hr = pThis->Initialize(hInstance);
        if (FAILED(hr))
        {
            *ppElement = nullptr;
            pThis->Destroy();
        }
    }

    return hr;
}
