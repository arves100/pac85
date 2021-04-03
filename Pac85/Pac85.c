#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"

/* -.- */
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' ""version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	HRESULT hr;
	struct SUi* psUi = NULL;
	INITCOMMONCONTROLSEX sIcse;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdShow);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	sIcse.dwSize = sizeof(sIcse);
	sIcse.dwICC = ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS | ICC_LINK_CLASS;

	if (!InitCommonControlsEx(&sIcse))
	{
		DWORD xd = GetLastError();
		
		//_CrtDbgBreak();
		// todo
	}

	hr = UiCreate(hInstance, &psUi);
	if (FAILED(hr))
	{
		//_CrtDbgBreak();
		// todo
	}

	hr = UiSwitchToDialog(psUi, WINDOWID_Welcome);
	if (FAILED(hr))
	{
		//_CrtDbgBreak();
		// todo
	}
	
	UiLoop(psUi);

	UiDelete(psUi);
	
	return 0;
}
