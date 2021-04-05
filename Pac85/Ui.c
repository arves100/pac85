#include "StdAfx.h"
#include "Ui.h"
#include "Utils.h"

/*!
 * Callback of Ui creation
 * @param psUi a pointer to the ui structure
 * @param psOut a pointer where the component structure will be held
 * @return S_OK for no error
 */
typedef HRESULT(CALLBACK* CbUiCreate)(_In_ struct SUi* psUi, _Out_ struct SBasicComponent** psOut);

/*!
 * Callback of Ui deletion
 * @param psUi a pointer to the ui structure
 * @param psComp a pointer to the corresponding component structure
 * @return S_OK for no error
 */
typedef HRESULT(CALLBACK* CbUiDelete)(_In_ struct SUi* psUi, _In_ struct SBasicComponent* psComp);

/*!
 * Defines the callbacks of a new Ui
 */
#define DEFINE_UI(name) extern HRESULT CALLBACK Ui_Create##name(_In_ struct SUi*, _Out_ struct SBasicComponent**); extern HRESULT CALLBACK Ui_Delete##name(_In_ struct SUi*, _In_ struct SBasicComponent*)

/*!
 * Adds a new Ui inside the memory manager
 */
#define ADD_UI(name) { WINDOWID_##name, Ui_Create##name, Ui_Delete##name }

/* UI definitions */

DEFINE_UI(Main);
DEFINE_UI(Welcome);
DEFINE_UI(DetectMirror);

/*!
 * This structure contains the callbacks of a specific ui component that will
 * get called during creation or destruction of the component
 */
struct SUiComponentCallbacks
{
	UINT nId; /*! id of the component */
	CbUiCreate Create; /*! creation callback */
	CbUiDelete Delete; /*! deletion callback */
};

/*!
 * An array that contains all the component callbacks
 */
static const struct SUiComponentCallbacks g_asMngr[] =
{
	ADD_UI(Main),
	ADD_UI(Welcome),
	ADD_UI(DetectMirror),
};

/*!
 * This function is internally used to free a component
 * @param psUi ui structure
 * @param psComp component structure that will be freed
 * @return S_OK for no error
 */
static HRESULT UiFreeComponent(_In_ struct SUi* psUi, struct SBasicComponent* psComp)
{
	UINT i = 0;
	HRESULT hr = S_OK;
	
	for (; i < ARRAYSIZE(g_asMngr); i++)
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

/*!
 * This function is internally used to create a component
 * @param psUi ui structure
 * @param nId ID of the component
 * @param psComp output where the specific component will be created
 * @return S_OK for no error, E_NOTIMPL if the id is invalid or other errors
 */
static HRESULT UiCreateComponent(_In_ struct SUi* psUi, _In_ UINT nId, _Out_ struct SBasicComponent** psComp)
{
	UINT i = 0;
	HRESULT hr = E_NOTIMPL;

	for (; i < ARRAYSIZE(g_asMngr); i++)
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

/*!
 * This function is internally used in the loop to skip handling of dialog window messages,
 * as explained in the MSDN, a window loop should not translate and free messages allocated by
 * dialog/dialog children
 * @param psUi ui component
 * @param pMsg the current message
 */
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
				/* it's a window message, handle and free it */
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	}

	return (INT)msg.wParam;
}
