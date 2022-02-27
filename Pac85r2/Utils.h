#pragma once

#define UtShowError(hWnd, szFmt, ...) UtShowHResultError(hWnd, szFmt, GetLastError(), ##__VA_ARGS__)
#define UtShowError2(hWnd, nId, ...) { LPWSTR szLang = UtLoadString(nId); if (szLang) { UtShowError(hWnd, szLang, ##__VA_ARGS__); UtFree(szLang); }}

extern void UtShowHResultError(HWND hWnd, LPCWSTR szFmt, HRESULT hResult, ...);
#define UtShowHResultError2(hWnd, nId, hResult, ...) { LPWSTR szLang = UtLoadString(nId); if (szLang) { UtShowHResultError(hWnd, szLang, hResult, ##__VA_ARGS__); UtFree(szLang); }}

extern LPWSTR UtLoadString(UINT nId);

extern LPVOID UtAlloc(SIZE_T nTotalSize);
extern VOID UtFree(LPVOID lpData);
