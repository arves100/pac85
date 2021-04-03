#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"
#include "resource.h"

struct SMainWindow
{
	struct SBasicComponent sBase; /* required */
	struct SUi* psUi; /* for switching */
	ATOM aCls; /* registerclassex */
	HBRUSH bBg; /* createsolidbrush */
	HWND hWText;
	HFONT hFont;
};

static LRESULT WINAPI CTLColorProc(HWND hWindow, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_PAINT: /* Custom paint routine to draw the Pac85 text like the old installshield applications */
	{
		RECT rc;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWindow, &ps);
		struct SMainWindow* psw = (struct SMainWindow*)GetWindowLongPtr(hWindow, GWLP_USERDATA);
		HFONT hOldFont = SelectObject(hdc, psw->hFont);
			
		GetClientRect(hWindow, &rc);
		SetBkMode(hdc, TRANSPARENT);

		SetTextColor(hdc, RGB(64, 64, 64));
		rc.top += 5;
		rc.left += 5;
		rc.bottom += 5;
		rc.right += 5;
		DrawTextW(hdc, L"PAC85", -1, &rc, DT_CENTER | DT_VCENTER); /* shadow text */

		rc.top -= 5;
		rc.left -= 5;
		rc.bottom -= 5;
		rc.right -= 5;
		SetTextColor(hdc, RGB(255, 255, 255));
		DrawTextW(hdc, L"PAC85", -1, &rc, DT_CENTER | DT_VCENTER); /* normal text */
		EndPaint(hWindow, &ps);

		SelectObject(hdc, hOldFont);
		return FALSE;
	}
	default:
		break;
	}

	return DefWindowProc(hWindow, nMsg, wParam, lParam);
}

static LRESULT WINAPI WPMain(HWND hWindow, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CREATE: /* Create child objects */
	{
		CREATESTRUCTW* sCs = (CREATESTRUCTW*)lParam;
		struct SMainWindow* sMW = (struct SMainWindow*)sCs->lpCreateParams;

		/* create the pac85 installshield like font */
		sMW->hFont = CreateFontW(90, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Times New Roman");

		if (!sMW->hFont)
			return -1;

		/* static control that will get the custom paint for the text */
		sMW->hWText = CreateWindowW(L"STATIC", NULL, WS_VISIBLE | WS_CHILD, 10, 20, 320, 180, hWindow, NULL, sCs->hInstance, NULL);

		if (!sMW->hWText)
			return -1;

		/* Set user data and window proc for the new static text */
		SetWindowLongPtr(sMW->hWText, GWLP_USERDATA, (LONG)sMW);
		SetWindowLongPtr(sMW->hWText, GWLP_WNDPROC, (LONG)CTLColorProc);

		/* Set user data for ourself (used in WM_DESTROY) */
		SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG)sMW);
		break;
	}
	case WM_DESTROY: /* Destroy child objects by the window */
	{
		struct SMainWindow* lpsWin = (struct SMainWindow*)GetWindowLongPtr(hWindow, GWLP_USERDATA);

		if (lpsWin->hWText)
		{
			DestroyWindow(lpsWin->hWText);
			lpsWin->hWText = NULL;
		}

		if (lpsWin->hFont)
		{
			DeleteObject(lpsWin->hFont);
			lpsWin->hFont = NULL;
		}

		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE: /* Sent by any message */
	{
		HWND hCurDlg = GetWindow(hWindow, GW_CHILD);
		if (MessageBoxW(hCurDlg, L"The installation is not finished, are you sure you want to exit?", L"Pac85", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			DestroyWindow(hWindow);
		}
		
		break;
	}
	case WM_SWITCH_DIALOG: /* Custom msg to switch dialogs (WPARAM = Dialog ID) */
	{
		struct SMainWindow* lpsWin = (struct SMainWindow*)GetWindowLongPtr(hWindow, GWLP_USERDATA);
		UiSwitchToDialog(lpsWin->psUi, (UINT)wParam);
		break;
	}
	default:
		return DefWindowProc(hWindow, nMsg, wParam, lParam);
	}

	return FALSE;
}

HRESULT CALLBACK Ui_DeleteMain(_In_ struct SUi* psMain, _In_ struct SBasicComponent* lpsComp)
{
	struct SMainWindow* lpsWin = (struct SMainWindow*)lpsComp;
	
	if (lpsWin->sBase.hWnd)
	{
		DestroyWindow(lpsWin->sBase.hWnd);
		lpsWin->sBase.hWnd = NULL;
	}

	if (lpsWin->aCls)
	{
		UnregisterClassW(MAKEINTATOM(lpsWin->aCls), psMain->hInstance);
		lpsWin->aCls = 0;
	}

	if (lpsWin->bBg)
	{
		DeleteObject(lpsWin->bBg);
		lpsWin->bBg = NULL;
	}

	return S_OK;
}

HRESULT CALLBACK Ui_CreateMain(_In_ struct SUi* psMain, _Out_ struct SBasicComponent** lpsOut)
{
	WNDCLASSEXW sWcex;
	INT nWidth = GetSystemMetrics(SM_CXFULLSCREEN), nHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	DWORD dwStyle;
	struct SMainWindow* lpsHandle = NULL;

	/* setup basic structure fields */
	if (FAILED(UtlMalloc(sizeof(struct SMainWindow), (LPVOID*)&lpsHandle)))
		return E_OUTOFMEMORY;

	lpsHandle->sBase.nId = WINDOWID_Main;
	lpsHandle->bBg = CreatePatternBrush(LoadBitmapW(psMain->hInstance, MAKEINTRESOURCEW(IDB_BACKGROUND)));
	lpsHandle->psUi = psMain; /* TODO: any better way? sigh */

	/* register window class */
	sWcex.cbSize = sizeof(sWcex);
	sWcex.hbrBackground = lpsHandle->bBg;
	sWcex.cbClsExtra = 0;
	sWcex.cbWndExtra = 0;
	sWcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	sWcex.hInstance = psMain->hInstance;
	sWcex.lpszClassName = L"PAC85_MAIN_CLASS";
	sWcex.lpfnWndProc = WPMain;
	sWcex.lpszMenuName = NULL;
	sWcex.style = CS_VREDRAW | CS_HREDRAW;
	sWcex.hIcon = LoadIconW(psMain->hInstance, MAKEINTRESOURCEW(IDI_PAC));
	sWcex.hIconSm = LoadIconW(psMain->hInstance, MAKEINTRESOURCEW(IDI_PAC));

	lpsHandle->aCls = RegisterClassExW(&sWcex);

	if (!lpsHandle->aCls)
	{
		Ui_DeleteMain(psMain, (struct SBasicComponent*)lpsHandle);
		UtlFree(lpsHandle);
		return E_FAIL;
	}

	/* create main background window */
	lpsHandle->sBase.hWnd = CreateWindowW(MAKEINTATOM(lpsHandle->aCls), L"Pac85", WS_MAXIMIZE, 0, 0, nWidth, nHeight, NULL, NULL, psMain->hInstance, lpsHandle);

	if (!lpsHandle->sBase.hWnd)
	{
		Ui_DeleteMain(psMain, (struct SBasicComponent*)lpsHandle);
		UtlFree(lpsHandle);
		return E_FAIL;
	}

	/* Remove border and captions */
	dwStyle = GetWindowLongPtr(lpsHandle->sBase.hWnd, GWL_STYLE);
	dwStyle = dwStyle & ~WS_BORDER & ~WS_CAPTION;
	SetWindowLongPtr(lpsHandle->sBase.hWnd, GWL_STYLE, dwStyle);
	
	*lpsOut = (struct SBasicComponent*)lpsHandle;
	
	return S_OK;
}
