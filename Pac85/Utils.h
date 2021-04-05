#ifndef PAC85_UTILS_H
#define PAC85_UTILS_H

/*!
 * Shows a messagebox that contains an HResult
 * @param hWnd Handle the the window where the box will be show upper
 * @param szFmt format string (c api)
 * @param hRes the HRESULT to get the error
 * @param ... Extra format parameters
 * @return S_OK for no error
 */
extern HRESULT UtlShowErrorWithHResult(_In_ HWND hWnd, _In_ LPCWSTR szFmt, _In_ HRESULT hRes, ...);

/*!
 * Shows a messagebox that contains an HResult, the format string will be retrieved from the resource translation
 * @param hWnd Handle the the window where the box will be show upper
 * @param nId id of the translation
 * @param hRes the HRESULT to get the error
 * @param ... Extra format parameters
 * @return S_OK for no error
 */
extern HRESULT UtlShowTranslatedErrorWithHResult(_In_ HWND hWnd, _In_ UINT nId, _In_ HRESULT hRes, ...);

/*!
 * Allocates new memory
 * Please note that the allocated memory content will be defaulted to all 0s already
 * @param nTotalSize size of the memory to be allocated
 * @param lpOut output of the memory
 * @return S_OK for no error
 */
extern HRESULT UtlMalloc(_In_ SIZE_T nTotalSize, _Out_ LPVOID* lpOut);

/*!
 * Free an allocated memory
 * @param lpData the data to get freed
 * @return S_OK for no error
 */
extern HRESULT UtlFree(_In_ LPVOID lpData);


#endif /* PAC85_UTILS_H */
