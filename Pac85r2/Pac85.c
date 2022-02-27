/*
	The Arch Linux Installation Program for Windows

	Official website: http://sigterm.cc/pac85/

	Bless PAC85 our true saviour!
	Completely made with Win32 API.
	Released under GPLv3.

	Compile it with:
		cl.exe /EHsc PAC85.C
*/
#include "StdAfx.h"
#include "Ui.h"
#include "resource.h"

#define DEFAULT_MIRROR L"mirrors.prometeus.net"

static BOOL CALLBACK DownloadDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
static BOOL CALLBACK WelcomeDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

struct SWindowData
{
	HWND hBitmap;
};

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	MSG msg;
	struct SUiHandle sMainWindow;

	if (!UiMakeMainWindow(hInstance, &sMainWindow))
		return -1;

	while (GetMessageW(&msg, NULL,  0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}

static BOOL CALLBACK WelcomeDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hDlgBitmap = GetDlgItem(hWnd, IDC_BITMAP);
		HBITMAP hPacWaifu = LoadBitmapW((HINSTANCE)GetWindowLongW(hWnd, GWL_HINSTANCE), MAKEINTRESOURCEW(IDB_WAIFU));
		SendDlgItemMessageW(hWnd, IDC_BITMAP, STM_SETIMAGE, (WPARAM)IDB_WAIFU, (LPARAM)hPacWaifu);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		default:
			break;
		}
		break;
		
	default:
		break;
	}
	
	return FALSE;
}

static BOOL CALLBACK DownloadDlgProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDABORT:
			MessageBoxW(hWnd, L"TODO", L"TODO", MB_OK);
			EndDialog(hWnd, wParam);
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return FALSE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_CREATE:
		break;
	case WM_PAINT:
	{
		//HDC hdc;
		RECT rPacPos;
		DRAWTEXTPARAMS sDTP;
			
		rPacPos.top = 10;
		rPacPos.left = 10;
		rPacPos.right = 400;
		rPacPos.bottom = 400;
		sDTP.cbSize = 100;
		sDTP.iTabLength = 4;
		sDTP.iLeftMargin = 0;
		sDTP.iRightMargin = 0;
		sDTP.uiLengthDrawn = 6;
			
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		SetTextColor(hdc, RGB(0xFF, 0x00, 0x00));

		DrawText(hdc, L"Hello World!", -1, &rPacPos, DT_SINGLELINE | DT_NOCLIP);
		EndPaint(hWnd, &ps);
			
		break;
	}
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
		
	default:
		return DefWindowProc(hWnd, nMsg, wParam, lParam);
	}

	return FALSE;
}
