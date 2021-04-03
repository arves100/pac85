#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"

/*!
 * Callback of Ui creation
 */
typedef HRESULT(CALLBACK* CbUiCreate)(_In_ struct SUi*, _Out_ struct SBasicComponent**);

/*!
 * Callback of Ui deletion
 */
typedef HRESULT(CALLBACK* CbUiDelete)(_In_ struct SUi*, _In_ struct SBasicComponent*);

/*!
 * Defines the callbacks of a new Ui
 */
#define DEFINE_UI(name) extern HRESULT CALLBACK Ui_Create##name(_In_ struct SUi*, _Out_ struct SBasicComponent**); extern HRESULT CALLBACK Ui_Delete##name(_In_ struct SUi*, _In_ struct SBasicComponent*)

/*!
 * Adds a new Ui inside the memory manager
 */
#define ADD_UI(name) { WINDOWID_##name, Ui_Create##name, Ui_Delete##name }

DEFINE_UI(Main);
DEFINE_UI(Welcome);
DEFINE_UI(DetectMirror);

struct SUiComponentMemoryManager
{
	UINT nId;
	CbUiCreate Create;
	CbUiDelete Delete;
};

static const UINT g_nMngrSize = 3;
static const struct SUiComponentMemoryManager g_asMngr[] =
{
	ADD_UI(Main),
	ADD_UI(Welcome),
	ADD_UI(DetectMirror),
};

static HRESULT UiFreeComponent(_In_ struct SUi* psUi, struct SBasicComponent* psComp)
{
	UINT i = 0;
	HRESULT hr = S_OK;
	
	for (; i < g_nMngrSize; i++)
	{
		if (g_asMngr[i].nId == psComp->nId)
		{
			hr = g_asMngr[i].Delete(psUi, psComp);
			break;
		}
	}

	if (FAILED(hr))
		return hr;

	UtlFree(psComp);
	return S_OK;
}

static HRESULT UiCreateComponent(_In_ struct SUi* psUi, _In_ UINT nId, _Out_ struct SBasicComponent** psComp)
{
	UINT i = 0;
	HRESULT hr = E_NOTIMPL;

	for (; i < g_nMngrSize; i++)
	{
		if (g_asMngr[i].nId == nId)
		{
			hr = g_asMngr[i].Create(psUi, psComp);
			break;
		}
	}

	return hr;
}

HRESULT UiCreate(_In_ HINSTANCE hInstance, _Out_ struct SUi** pOut)
{
	struct SUi* psUi;
	HRESULT hr = E_INVALIDARG;

	if (!pOut || !hInstance)
		return hr;

	hr = UtlMalloc(sizeof(struct SUi), (LPVOID*)&psUi);
	if (FAILED(hr))
		return hr;

	psUi->hInstance = hInstance;

	hr = UiCreateComponent(psUi, WINDOWID_Main, &psUi->psBackgroundWin);
	if (FAILED(hr))
	{
		UtlFree(psUi);
		return hr;
	}

	ShowWindow(psUi->psBackgroundWin->hWnd, SW_SHOW);
	UpdateWindow(psUi->psBackgroundWin->hWnd);

	*pOut = psUi;
	
	return hr;
}

HRESULT UiDelete(_In_ struct SUi* psUi)
{
	HRESULT hr = S_OK;
	
	if (psUi)
	{
		hr = UiDeleteActiveDialog(psUi);
		if (FAILED(hr))
			return hr;

		hr = UiFreeComponent(psUi, psUi->psBackgroundWin);
		if (FAILED(hr))
			return hr;

		psUi->hInstance = NULL;		
	}

	return hr;
}

HRESULT UiDeleteActiveDialog(_In_ struct SUi* psUi)
{
	HRESULT hr = E_INVALIDARG;
	
	if (!psUi)
		return hr;

	if (!psUi->psActiveDlg)
		return S_OK;
	
	if (!CloseWindow(psUi->psActiveDlg->hWnd))
		return E_FAIL;
	
	hr = UiFreeComponent(psUi, psUi->psActiveDlg);

	if (FAILED(hr))
		return hr;
	
	psUi->psActiveDlg = NULL;
	return S_OK;
}

HRESULT UiSwitchToDialog(_In_ struct SUi* psUi, _In_ UINT nId)
{
	HRESULT hr = UiDeleteActiveDialog(psUi);

	if (FAILED(hr))
		return hr;

	hr = UiCreateComponent(psUi, nId, &psUi->psActiveDlg);

	if (FAILED(hr))
		return hr;
	
	ShowWindow(psUi->psActiveDlg->hWnd, SW_SHOW);
	
	return hr;
}

static BOOL UiCanHandleMsg(_In_ struct SUi* psUi, _In_ MSG* pMsg)
{
	HWND hCld;
	
	if (!psUi->psActiveDlg)
		return TRUE;

	if (IsWindow(psUi->psActiveDlg->hWnd) && IsDialogMessageW(psUi->psActiveDlg->hWnd, pMsg))
		return FALSE;

	hCld = GetWindow(psUi->psActiveDlg->hWnd, GW_CHILD);

	if (!hCld)
		return TRUE;

	return !IsWindow(hCld) || !IsDialogMessageW(hCld, pMsg);
}

INT UiLoop(_In_ struct SUi* psUi)
{
	if (!psUi)
		return -1;
	
	MSG msg;
	msg.message = WM_CREATE;

	while (msg.message != WM_QUIT)
	{
		if (GetMessageW(&msg, NULL, 0, 0))
		{
			if (UiCanHandleMsg(psUi, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	return (INT)msg.wParam;
}
