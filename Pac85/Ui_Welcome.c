#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"
#include "resource.h"

struct SWelcomeDlg
{
	struct SBasicComponent sBase;
};

static INT_PTR WINAPI WLCDMain(HWND hWindow, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG: /* create/set dialog child object */
	{
		return TRUE;
	}
	case WM_DESTROY: /* destroy dialog child objects */
	{
		break;
	}
	case WM_CLOSE:
		SendMessageW(GetParent(hWindow), WM_CLOSE, 0, 0);
		break;
		
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDOK:
			SendMessage(GetParent(hWindow), WM_SWITCH_DIALOG, (WPARAM)WINDOWID_DetectMirror, 0);
			break;
		case IDCANCEL:
			SendMessageW(GetParent(hWindow), WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
			
		break;
	}
	default:
		break;
	}

	return FALSE;
}

HRESULT CALLBACK Ui_DeleteWelcome(_In_ struct SUi* psUi, _In_ struct SBasicComponent* lpsComp)
{
	struct SWelcomeDlg* lpsDlg = (struct SWelcomeDlg*)lpsComp;

	UNREFERENCED_PARAMETER(psUi);

	if (lpsDlg->sBase.hWnd)
	{
		DestroyWindow(lpsDlg->sBase.hWnd);
		lpsDlg->sBase.hWnd = NULL;
	}

	return S_OK;
}

HRESULT CALLBACK Ui_CreateWelcome(_In_ struct SUi* psUi, _Out_ struct SBasicComponent** lpsOut)
{
	struct SWelcomeDlg* lpsHandle = NULL;
	INT nWidth = GetSystemMetrics(SM_CXFULLSCREEN), nHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	RECT sRct;
	
	if (FAILED(UtlMalloc(sizeof(struct SWelcomeDlg), (LPVOID*)&lpsHandle)))
		return E_OUTOFMEMORY;

	lpsHandle->sBase.nId = WINDOWID_Welcome;

	lpsHandle->sBase.hWnd = CreateDialogParamW(psUi->hInstance, MAKEINTRESOURCEW(IDD_WELCOME), psUi->psBackgroundWin->hWnd, (DLGPROC)WLCDMain, (LPARAM)lpsHandle);

	if (!lpsHandle->sBase.hWnd)
	{
		Ui_DeleteWelcome(psUi, (struct SBasicComponent*)lpsHandle);
		UtlFree(lpsHandle);
		return E_FAIL;
	}

	GetWindowRect(lpsHandle->sBase.hWnd, &sRct);
	SetWindowPos(lpsHandle->sBase.hWnd, NULL, (nWidth / 2) - ((sRct.right - sRct.left) / 2), (nHeight / 2) - ((sRct.bottom - sRct.top) / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	*lpsOut = (struct SBasicComponent*)lpsHandle;

	return S_OK;
}
