// DuiInit.h
#pragma once

// Simple class which handles DUI initialization in it's Ctor/Dtor
class CDuiInitializer
{
    public:
        CDuiInitializer() : _hrCoInit(E_FAIL) {}
        ~CDuiInitializer();
        _Success_(return >= 0) HRESULT Initialize(DWORD dwCoInit);

    private:
        _Interlocked_ static ULONG s_lProcInitCount;
        HRESULT _hrCoInit;
};

/*---------------------------------------------------------------------------*/

ULONG __declspec(selectany) CDuiInitializer::s_lProcInitCount = 0;

// Initialize Com/Dui
_Success_(return >= 0)
inline HRESULT CDuiInitializer::Initialize(DWORD dwCoInit)
{
    HRESULT hr = S_OK;
    hr = _hrCoInit = ::CoInitializeEx(nullptr, dwCoInit);
    if (1 == InterlockedIncrement(&s_lProcInitCount))
    {
        hr = DirectUI::InitProcess(DUI_VERSION);
    }
    if (SUCCEEDED(hr))
    {
        hr = DirectUI::InitThread();
    }
    return hr;
}

// Terminate Com/Dui
inline CDuiInitializer::~CDuiInitializer()
{
    DirectUI::UnInitThread();
    if (0 == InterlockedDecrement(&s_lProcInitCount))
    {
        DirectUI::UnInitProcess();
    }
    if (SUCCEEDED(_hrCoInit))
    {
        ::CoUninitialize();
    }
}
