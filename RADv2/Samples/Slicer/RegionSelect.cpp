#include "PreComp.h"
#include "RegionSelect.h"
#include "Slicer.h"

// the window class name for the capture form
const wchar_t CRegionSelect::s_szClassName[] = L"SlicerRegionSelectWindow";

//#pragma prefast (disable: __WARNING_UNUSED_SCALAR_ASSIGNMENT, "noisy error, bug filing TBD")

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

void PointsToRect(POINT pt1, POINT pt2, _Out_ RECT *pRect)
{
    if (pt1.x > pt2.x)
    {
        pRect->left = pt2.x;
        pRect->right = pt1.x;
    }
    else
    {
        pRect->left = pt1.x;
        pRect->right = pt2.x;
    }
    if (pt1.y > pt2.y)
    {
        pRect->top = pt2.y;
        pRect->bottom = pt1.y;
    }
    else
    {
        pRect->top = pt1.y;
        pRect->bottom = pt2.y;
    }
}

CRegionSelect::CRegionSelect(HINSTANCE hInstance, _In_ CSlicer *pParent)
{
    _hwnd = NULL;
    _hInstance = hInstance;
    _pParent = pParent;
    _ptLastMove.x = 0;
    _ptLastMove.y = 0;
    _pt.x = 0;
    _pt.y = 0;

    _nWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    _nHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    _nXOrigin = GetSystemMetrics(SM_XVIRTUALSCREEN);
    _nYOrigin = GetSystemMetrics(SM_YVIRTUALSCREEN);

    _fCapture = false;
}

CRegionSelect::~CRegionSelect()
{
    // cleanup done by WM_DESTROY
}

HRESULT CRegionSelect::SetVisible(BOOL fVisible)
{
    HRESULT hr = S_OK;

    BOOL fIsVisible = IsWindowVisible(_hwnd);
    if (fIsVisible != fVisible)
    {
        ShowWindow(_hwnd, fVisible ? SW_SHOW : SW_HIDE);
    }

    if (fVisible)
    {
        SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }
    else
    {
        // reset mouse points
        _pt.x = _pt.y = _ptLastMove.x = _ptLastMove.y = 0;
    }

    return hr;
}

ATOM CRegionSelect::Register()
{
    WNDCLASS wndclass;
    wndclass.style = 0;
    wndclass.lpfnWndProc = (WNDPROC)CRegionSelect::WndProcDispatch;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = (HINSTANCE)_hInstance;
    wndclass.hIcon = NULL;
    wndclass.hCursor = LoadCursor(NULL, IDC_CROSS);
    // null brush prevents flickering of the window
    wndclass.hbrBackground = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = s_szClassName;

    return RegisterClass(&wndclass);
}

HRESULT CRegionSelect::Init()
{
    HRESULT hr = S_OK;

    // Register the window class...
    ATOM atom = Register();
    if (atom == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
    }

 // Create the window...
    if (SUCCEEDED(hr))
    {
        _hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED, // prevent window from appearing in taskbar
                               s_szClassName,
                               NULL,
                               WS_OVERLAPPED | WS_POPUP,    // No title bar or border
                               _nXOrigin,
                               _nYOrigin,
                               _nWidth,                     // Full screen
                               _nHeight,
                               NULL,
                               NULL,
                               _hInstance,
                               (LPVOID)this);
    }

    // Make this window transparent
    if (SUCCEEDED(hr))
    {
        SetLayeredWindowAttributes(_hwnd, 0, (BYTE)150, LWA_ALPHA);
    }

    return hr;
}

LRESULT CALLBACK CRegionSelect::WndProcDispatch(HWND hwnd,
                                               UINT uMessage,
                                               WPARAM wParam,
                                               LPARAM lParam)
{
    CRegionSelect *pWnd = (CRegionSelect *)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (pWnd == NULL && uMessage == WM_CREATE)
    {
        LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
        pWnd = (CRegionSelect *)lpcs->lpCreateParams;
        pWnd->_hwnd = hwnd;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(ULONG_PTR)pWnd);
    }

    if (pWnd != NULL)
    {
        return pWnd->WndProc(uMessage, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, uMessage, wParam, lParam);
    }
}

LRESULT CRegionSelect::WndProc(UINT uMessage,
                              WPARAM wParam,
                              LPARAM lParam)
{
    HRESULT hr = S_OK;
    LRESULT lResult = -1;

    switch (uMessage)
    {
        case WM_CREATE:
            lResult = 0;
            break;

        case WM_ERASEBKGND:
            lResult = 1;
            break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT clientRect;
            HRGN bgRgn;
            HBRUSH hBrush;

            HDC hdc = BeginPaint(_hwnd, &ps);

            // Fill the client area with a gray brush
            GetClientRect(_hwnd, &clientRect);
            bgRgn = CreateRectRgnIndirect(&clientRect);
            // Create a gray brush
            hBrush = CreateSolidBrush(RGB(195, 195, 195));
            FillRgn(hdc, bgRgn, hBrush);

            if (_fCapture)
            {
                // Fill the selected region with a pink brush
                RECT rc;
                PointsToRect(_ptLastMove, _pt, &rc);
                HRGN selectedRgn = CreateRectRgnIndirect(&rc);
                // Create a pink brush
                hBrush = CreateSolidBrush(RGB(255, 40, 213));
                FillRgn(hdc, selectedRgn, hBrush);
            }

            // Clean up
            DeleteObject(hBrush);

            EndPaint(_hwnd, &ps);
            break;
        }

        case WM_DESTROY:
            Cleanup();
            lResult = 0;
            break;

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        // if we are in rectangular capture mode then remember the start point, and
        // set the draw rect flag to true
            _fCapture = true;

            _pt.x = GET_X_LPARAM(lParam);
            _pt.y = GET_Y_LPARAM(lParam);
            _ptLastMove = _pt;

            ::SetCapture(_hwnd);

            lResult = 0;
            break;

        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_LBUTTONUP:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            // if we are in rectangular capture mode then we have completed a snip
            // need to record the point and send an event to the parent
            // also unsets the draw rect flag
            RECT rc;
            PointsToRect(pt, _pt, &rc);
            
            // RC is currently in coordinates where 0,0 is the top right of the virtual screen.
            // Need to modify it so that 0,0 is the top right of the primary monitor.
            int xDelta = GetSystemMetrics(SM_XVIRTUALSCREEN);
            int yDelta = GetSystemMetrics(SM_YVIRTUALSCREEN);

            rc.left += xDelta;
            rc.right += xDelta;
            rc.top += yDelta;
            rc.bottom += yDelta;

            // send the event to the parent
            _pParent->RectSelectEvent(&rc);

            _fCapture = false;

            ReleaseCapture();
            lResult = 0;
            break;
        }

        case WM_MOUSEMOVE:
            {
                POINT pt;
                RECT rc1;
                RECT rc2;
                RECT rcUnion;

                ZeroMemory(&rc2, sizeof(RECT));

                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);

                PointsToRect(_pt, pt, &rc1);
                PointsToRect(_pt, _ptLastMove, &rc2);
                if (UnionRect(&rcUnion, &rc1, &rc2))
                {
                    InvalidateRect(_hwnd, &rcUnion, FALSE);
                }

                _ptLastMove = pt;

                lResult = 0;
                break;
            }

        case WM_ACTIVATE:
            // if we lose activation to anything other than the toolbar
            // window, then exit capture mode. This generally happens when
            // the user alt-tabs out of capture mode.
            if (LOWORD(wParam) == WA_INACTIVE)
            {
                HWND hwndNewActive = (HWND)lParam;
                if (hwndNewActive != _pParent->GetHWND() &&
                    _pParent->GetCaptureMode())
                {
                    _pParent->SetInCaptureMode(false); // logs
                }
            }
            lResult = DefWindowProc(_hwnd, uMessage, wParam, lParam);
            break;

        case WM_DISPLAYCHANGE:
            // kick out of capture mode
            _pParent->SetInCaptureMode(false);

            // update the virtual screen size (and make the capture form match this size)
            _nWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            _nHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
            _nXOrigin = GetSystemMetrics(SM_XVIRTUALSCREEN);
            _nYOrigin = GetSystemMetrics(SM_YVIRTUALSCREEN);

            SetWindowPos(_hwnd, HWND_TOPMOST,
                _nXOrigin,
                _nYOrigin,
                _nWidth,
                _nHeight,
                SWP_NOACTIVATE);
            lResult = 0;
            break;

        case WM_QUERYENDSESSION:
            if ((lParam & ENDSESSION_CLOSEAPP) != 0)
            {
                _pParent->SetInCaptureMode(false);
                lResult = 1;
            }
            break;

        default:
            lResult = DefWindowProc(_hwnd, uMessage, wParam, lParam);
            break;
    }

    return lResult;
}

void CRegionSelect::Cleanup()
{
    if (_hwnd)
    {
        // Make sure that if any additional messages come to our window,
        // no one trys to access the user data
        SetWindowLongPtr(_hwnd, GWLP_USERDATA, NULL);
    }
}
