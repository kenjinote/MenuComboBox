#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>

#define DEFAULT_DPI 96
#define SCALEX(X) MulDiv(X, uDpiX, DEFAULT_DPI)
#define SCALEY(Y) MulDiv(Y, uDpiY, DEFAULT_DPI)
#define POINT2PIXEL(PT) MulDiv(PT, uDpiY, 72)

TCHAR szClassName[] = TEXT("Window");
WNDPROC defStaticWndProc;

LRESULT CALLBACK StaticProc1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static LPWSTR lpszCommand1[] = { L"繰り返さない", L"毎日", L"毎週〇曜日", L"毎月第x◯曜日", L"毎年◯◯月◯◯日" };
	static LPWSTR lpszCommand2[] = { L"この予定", L"これ以降のすべての予定", L"すべての予定" };
	switch (msg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			RECT rect;
			GetClientRect(hWnd, &rect);
			DWORD dwTextSize = GetWindowTextLength(hWnd);
			LPWSTR lpszText = (LPWSTR)GlobalAlloc(0, (dwTextSize + 1) * sizeof(WCHAR));
			GetWindowText(hWnd, lpszText, dwTextSize + 1);
			COLORREF clrPrev = SetBkColor(hdc, RGB(255, 255, 255));
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
			rect.left += 4;
			DrawText(hdc, lpszText, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			DrawText(hdc, L"▼", -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
			SetBkColor(hdc, clrPrev);
			GlobalFree(lpszText);
			SelectObject(hdc, hOldFont);
			EndPaint(hWnd, &ps);
		}
		return 0;
	case WM_LBUTTONDOWN:
		{
			int wID = 0;
			HMENU hMenu = CreatePopupMenu();
			MENUITEMINFO mi2 = {};
			mi2.cbSize = sizeof(MENUITEMINFO);
			mi2.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_DATA;
			mi2.fType = MFT_OWNERDRAW;
			for (auto i : lpszCommand1)
			{
				HMENU hSubMenu1 = CreatePopupMenu();
				MENUITEMINFO mi1 = {};
				mi1.cbSize = sizeof(MENUITEMINFO);
				mi1.fMask = MIIM_TYPE | MIIM_ID | MIIM_DATA;
				mi1.fType = MFT_OWNERDRAW;
				int nPos = 0;
				for (auto j : lpszCommand2)
				{
					mi1.dwItemData = (ULONG_PTR)j;
					mi1.wID = 1000 + wID;
					wID++;
					InsertMenuItem(hSubMenu1, nPos + 1, TRUE, &mi1);
					nPos++;
				}
				mi2.dwItemData = (ULONG_PTR)i;
				mi2.hSubMenu = hSubMenu1;
				InsertMenuItem(hMenu, nPos + 1, TRUE, &mi2);
				DestroyMenu(hSubMenu1);
			}
			RECT rect;
			GetWindowRect(hWnd, &rect);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_TOPALIGN, rect.left, rect.bottom, 0, hWnd, NULL);
			DestroyMenu(hMenu);
		}
		break;
	case WM_COMMAND:
		if (1000 <= LOWORD(wParam) && LOWORD(wParam) < 1000 + _countof(lpszCommand1) * _countof(lpszCommand2))
		{
			int wID = LOWORD(wParam) - 1000;
			if (0 <= wID && wID < _countof(lpszCommand1) * _countof(lpszCommand2))
			{
				WCHAR szText[1024];
				wsprintf(szText, L"%s, %s", lpszCommand1[wID / _countof(lpszCommand2)], lpszCommand2[wID % _countof(lpszCommand2)]);
				SetWindowText(hWnd, szText);
				InvalidateRect(hWnd, 0, 0);
			}
		}
		break;
	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpMI = (LPMEASUREITEMSTRUCT)lParam;
			HDC hdc = GetDC(hWnd);
			HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			LPWSTR lpszText = (LPWSTR)lpMI->itemData;
			SIZE sz = {};
			GetTextExtentPoint32(hdc,
				lpszText,
				lstrlen(lpszText),
				&sz);
			lpMI->itemWidth = sz.cx + 8;
			lpMI->itemHeight = sz.cy + 8;
			SelectObject(hdc, hOldFont);
			ReleaseDC(hWnd, hdc);
		}
		return TRUE;
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDI = (LPDRAWITEMSTRUCT)lParam;
			RECT rect = lpDI->rcItem;
			HDC hdc = lpDI->hDC;
			HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
			COLORREF clrPrev = SetBkColor(hdc, lpDI->itemState & ODS_SELECTED ? RGB(228, 242, 251) : RGB(255, 255, 255));
			ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
			rect.left += 4;
			DrawText(hdc, (LPCWSTR)lpDI->itemData, -1, &rect, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			SetBkColor(hdc, clrPrev);
			SelectObject(hdc, hOldFont);
		}
		return TRUE;
	default:
		break;
	}
	return CallWindowProc(defStaticWndProc, hWnd, msg, wParam, lParam);
}

BOOL GetScaling(HWND hWnd, UINT* pnX, UINT* pnY)
{
	BOOL bSetScaling = FALSE;
	const HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	if (hMonitor)
	{
		HMODULE hShcore = LoadLibrary(TEXT("SHCORE"));
		if (hShcore)
		{
			typedef HRESULT __stdcall GetDpiForMonitor(HMONITOR, int, UINT*, UINT*);
			GetDpiForMonitor* fnGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor*>(GetProcAddress(hShcore, "GetDpiForMonitor"));
			if (fnGetDpiForMonitor)
			{
				UINT uDpiX, uDpiY;
				if (SUCCEEDED(fnGetDpiForMonitor(hMonitor, 0, &uDpiX, &uDpiY)) && uDpiX > 0 && uDpiY > 0)
				{
					*pnX = uDpiX;
					*pnY = uDpiY;
					bSetScaling = TRUE;
				}
			}
			FreeLibrary(hShcore);
		}
	}
	if (!bSetScaling)
	{
		HDC hdc = GetDC(NULL);
		if (hdc)
		{
			*pnX = GetDeviceCaps(hdc, LOGPIXELSX);
			*pnY = GetDeviceCaps(hdc, LOGPIXELSY);
			ReleaseDC(NULL, hdc);
			bSetScaling = TRUE;
		}
	}
	if (!bSetScaling)
	{
		*pnX = DEFAULT_DPI;
		*pnY = DEFAULT_DPI;
		bSetScaling = TRUE;
	}
	return bSetScaling;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStatic;
	static HFONT hFont;
	static UINT uDpiX = DEFAULT_DPI, uDpiY = DEFAULT_DPI;
	switch (msg)
	{
	case WM_CREATE:
		hStatic = CreateWindow(TEXT("STATIC"), TEXT("a"), WS_VISIBLE | WS_CHILD | WS_BORDER | SS_NOTIFY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		defStaticWndProc = (WNDPROC)SetWindowLongPtr(hStatic, GWLP_WNDPROC, (LONG_PTR)StaticProc1);
		SendMessage(hWnd, WM_APP, 0, 0);
		break;
	case WM_APP:
		GetScaling(hWnd, &uDpiX, &uDpiY);
		DeleteObject(hFont);
		hFont = CreateFontW(-POINT2PIXEL(16), 0, 0, 0, FW_NORMAL, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0, 0, L"MS Shell Dlg");
		SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, 0);
		break;
	case WM_SIZE:
		MoveWindow(hStatic, POINT2PIXEL(10), POINT2PIXEL(10), POINT2PIXEL(256), POINT2PIXEL(32), TRUE);
		break;
	case WM_NCCREATE:
		{
			const HMODULE hModUser32 = GetModuleHandle(TEXT("user32.dll"));
			if (hModUser32)
			{
				typedef BOOL(WINAPI*fnTypeEnableNCScaling)(HWND);
				const fnTypeEnableNCScaling fnEnableNCScaling = (fnTypeEnableNCScaling)GetProcAddress(hModUser32, "EnableNonClientDpiScaling");
				if (fnEnableNCScaling)
				{
					fnEnableNCScaling(hWnd);
				}
			}
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	case WM_DPICHANGED:
		SendMessage(hWnd, WM_APP, 0, 0);
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
