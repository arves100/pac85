#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"
#include "resource.h"

static LRESULT WINAPI WPMain(HWND hWindow, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	default:
		return DefWindowProc(hWindow, nMsg, wParam, lParam);
	}
	
	return FALSE;
}

BOOL UiMakeMainWindow(HINSTANCE hInstance, struct SUiHandle** lpsOut)
{
	WNDCLASSEXW sWcex;
	INT nWidth = GetSystemMetrics(SM_CXFULLSCREEN), nHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	DWORD dwStyle;
	struct SUiMainWindow* lpsHandle = (struct SUiMainWindow*)UtAlloc(sizeof(struct SUiMainWindow));

	lpsHandle->hBrush = CreatePatternBrush(LoadBitmapW(hInstance, MAKEINTRESOURCEW(IDB_BACKGROUND)));
	lpsHandle->hWindow = NULL;
	lpsHandle->aClass = NULL;
	
	sWcex.cbSize = sizeof(sWcex);
	sWcex.hbrBackground = lpsHandle->hBrush;
	sWcex.cbClsExtra = 0;
	sWcex.cbWndExtra = 0;
	sWcex.hCursor = LoadCursorW(hInstance, IDC_ARROW);
	sWcex.hInstance = hInstance;
	sWcex.lpszClassName = L"PAC85_MAIN_CLASS";
	sWcex.lpfnWndProc = WPMain;
	sWcex.lpszMenuName = NULL;

	// TODO
	sWcex.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	sWcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);

	lpsHandle->aClass = RegisterClassExW(&sWcex);

	if (!lpsHandle->aClass)
	{
		UiFreeHandle(lpsHandle);
		return FALSE;
	}

	lpsHandle->hWindow = CreateWindowW(MAKEINTATOM(lpsHandle->aClass), L"Pac85", WS_VISIBLE | WS_MAXIMIZE, 0, 0, nWidth, nHeight, NULL, NULL, hInstance, NULL);

	if (!lpsHandle->hWindow)
	{
		UiFreeHandle(lpsHandle);
		return FALSE;
	}

	/* Remove border and captions */
	dwStyle = GetWindowLongPtr(lpsHandle->hWindow, GWL_STYLE);
	dwStyle = dwStyle & ~WS_BORDER & ~WS_CAPTION;
	SetWindowLongPtr(lpsHandle->hWindow, GWL_STYLE, dwStyle);

	UpdateWindow(lpsHandle->hWindow);
	
	return TRUE;
}
