#pragma once

enum EWindowIDs
{
	WINDOWID_MAIN,
};

#define DEFAULT_SUI_PARAMS() UINT nWindowId; ATOM aClass; HWND hWindow;

struct SUiHandle
{
	DEFAULT_SUI_PARAMS();
};

struct SUiMainWindow
{
	DEFAULT_SUI_PARAMS();
	HBRUSH hBrush;
};

extern BOOL UiMakeMainWindow(HINSTANCE hInstance, struct SUiHandle* lpsHandle);

extern VOID UiLoop();
extern VOID UiFreeHandle(struct SUiHandle* lpsHandle);
