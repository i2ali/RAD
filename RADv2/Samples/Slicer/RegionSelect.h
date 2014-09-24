//-----------------------------------------------------------------------
// Borrowed from the Snipping Tool
//-----------------------------------------------------------------------

#pragma once


class CRegionSelect
{

friend class CSlicer; // make CSlicer a friend so it has access to private members

public:
    CRegionSelect(HINSTANCE hInstance, _In_ CSlicer *_pParent);
    ~CRegionSelect();
    HRESULT Init();

    HRESULT SetVisible(BOOL fVisible);

    static const wchar_t s_szClassName[];

private:
    int _nWidth;                   // Width of full-screen
    int _nHeight;                  // Height of full-screen
    int _nXOrigin;                 // left of the virtual screen
    int _nYOrigin;                 // top of the virtual screen

    ATOM Register();

    // static WndProc that dispatches messages to the WndProc for an instance of CRegionSelect
    static LRESULT CALLBACK WndProcDispatch(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

    // actual WndProc for an instance of this object
    LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    void Cleanup();

    HWND _hwnd;

    // The owning CToolbar object
    CSlicer *_pParent;

    POINT _pt;                             // Point used for rectangular and window selection mode
    POINT _ptLastMove;                     // The last point we were hovering over (used to determine repaint area)

    HINSTANCE _hInstance;

    // declare these as private so defaults are not generated
    CRegionSelect();
    CRegionSelect(const CRegionSelect&);

    bool _fCapture;
};
