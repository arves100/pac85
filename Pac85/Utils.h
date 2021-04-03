#ifndef PAC85_UTILS_H
#define PAC85_UTILS_H

extern HRESULT UtlShowErrorWithHResult(_In_ HWND hWnd, _In_ LPCWSTR szFmt, _In_ HRESULT hRes, ...);
extern HRESULT UtlShowTranslatedErrorWithHResult(_In_ HWND hWnd, _In_ UINT nId, _In_ HRESULT hRes, ...);

extern HRESULT UtlMalloc(_In_ SIZE_T nTotalSize, _Out_ LPVOID* lpOut);
extern HRESULT UtlFree(_In_ LPVOID lpData);


#endif /* PAC85_UTILS_H */
