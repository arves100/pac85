#pragma once

extern BOOL FsSave(HWND hWnd, LPCWSTR szFileName, DWORD dwSize, LPBYTE lpbData);
extern LPWSTR FsCreateSavePath(HWND hWnd, LPWSTR lpAdditionalPath);
