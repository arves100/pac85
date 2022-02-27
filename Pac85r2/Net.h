#pragma once

extern BOOL NetDownloadFile(HWND hWnd, LPCWSTR szUrl, LPCWSTR szRemoteFileUrl, LPCWSTR szSaveFileName, HINTERNET hSession);
extern HINTERNET NetMakeHttpSession(HWND hWnd);
