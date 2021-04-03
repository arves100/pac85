#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"
#include "resource.h"

struct SDetectMirrorDlg
{
	struct SBasicComponent sBase; /* required */
};

static INT_PTR WINAPI DCTMProc(HWND hWindow, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
	case WM_INITDIALOG: /* create/set dialog child object */
	{
		HWND hPrg = GetDlgItem(hWindow, IDC_PROGRESS1);
		DWORD dwStyle;
			
		if (!hPrg)
			break;

		dwStyle = GetWindowLongPtr(hPrg, GWL_STYLE);
		dwStyle |= PBS_MARQUEE;
		SetWindowLongPtr(hPrg, GWL_STYLE, dwStyle);
		SendMessage(hPrg, PBM_SETMARQUEE, TRUE, 0);
			
		return TRUE;
	}
	case WM_DESTROY: /* destroy dialog child objects */
	{
		break;
	}
	case WM_CLOSE:
		SendMessageW(GetParent(hWindow), WM_CLOSE, 0, 0);
		break;

	default:
		break;
	}

	return FALSE;
}

HRESULT CALLBACK Ui_DeleteDetectMirror(_In_ struct SUi* psUi, _In_ struct SBasicComponent* lpsComp)
{
	struct SDetectMirrorDlg* lpsDlg = (struct SDetectMirrorDlg*)lpsComp;

	UNREFERENCED_PARAMETER(psUi);

	if (lpsDlg->sBase.hWnd)
	{
		DestroyWindow(lpsDlg->sBase.hWnd);
		lpsDlg->sBase.hWnd = NULL;
	}

	return S_OK;
}

HRESULT CALLBACK Ui_CreateDetectMirror(_In_ struct SUi* psUi, _Out_ struct SBasicComponent** lpsOut)
{
	struct SDetectMirrorDlg* lpsHandle;
	RECT sRct;
	INT nWidth = GetSystemMetrics(SM_CXFULLSCREEN), nHeight = GetSystemMetrics(SM_CYFULLSCREEN);

	if (FAILED(UtlMalloc(sizeof(struct SDetectMirrorDlg), (LPVOID*)&lpsHandle)))
		return E_OUTOFMEMORY;

	lpsHandle->sBase.nId = WINDOWID_DetectMirror;

	lpsHandle->sBase.hWnd = CreateDialogParamW(psUi->hInstance, MAKEINTRESOURCEW(IDD_DETECTMIRROR), psUi->psBackgroundWin->hWnd, (DLGPROC)DCTMProc, (LPARAM)lpsHandle);

	if (!lpsHandle->sBase.hWnd)
	{
		Ui_DeleteDetectMirror(psUi, (struct SBasicComponent*)lpsHandle);
		UtlFree(lpsHandle);
		return E_FAIL;
	}

	GetWindowRect(lpsHandle->sBase.hWnd, &sRct);
	SetWindowPos(lpsHandle->sBase.hWnd, NULL, (nWidth / 2) - ((sRct.right - sRct.left) / 2), (nHeight / 2) - ((sRct.bottom - sRct.top) / 2), 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	*lpsOut = (struct SBasicComponent*)lpsHandle;
	
	return S_OK;
}
