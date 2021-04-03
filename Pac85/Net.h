#pragma once

extern BOOL NetDownloadFile(HWND hWnd, LPCWSTR szUrl, LPCWSTR szRemoteFileUrl, LPCWSTR szSaveFileName, HINTERNET hSession);

struct SNet
{
	HINTERNET hSession, hConnection;
};

struct SNetCtx
{
	HWND hWindow;
};

extern HRESULT NetStart(_In_ struct SNetCtx* psCtx, _Out_ struct SNet** psOut);
extern HRESULT NetFree(_In_ struct SNet* psNet);
