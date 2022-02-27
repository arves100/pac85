#include "StdAfx.h"
#include "Utils.h"
#include "resource.h"

LPVOID UtAlloc(SIZE_T nTotalSize)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nTotalSize);
}

VOID UtFree(LPVOID lpData)
{
	if (lpData)
		HeapFree(GetProcessHeap(), 0, lpData);
}

LPWSTR UtLoadString(UINT nId)
{
	LPWSTR szSrc, szDst;
	CONST INT nSize = LoadStringW(GetModuleHandleW(NULL), nId, (LPWSTR)&szSrc, 0);
	
	if (!nSize)
		return NULL;

	szDst = (LPWSTR)UtAlloc(nSize + 2);

	if (!szDst)
		return NULL;

	if (FAILED(StringCchCopyNW(szDst, nSize + 2, szSrc, nSize)))
	{
		UtFree(szDst);
		return NULL;
	}

	return szDst;
}

void UtShowHResultError(HWND hWnd, LPCWSTR szFmt, HRESULT hResult, ...)
{
	LPWSTR lpSystemErrorBuffer = NULL;
	va_list vl;
	CONST DWORD dwSystemErrorBufferSize =
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE, NULL, 
			hResult, LANG_USER_DEFAULT, (LPWSTR)&lpSystemErrorBuffer, 1, NULL);
	size_t nSEBLen, nMBLen, nFMTLen;
	LPWSTR szMessageBuffer;
	
	if (szFmt && SUCCEEDED(StringCchLengthW(szFmt, 2048, &nFMTLen)))
	{
		if (dwSystemErrorBufferSize > 0)
		{
			lpSystemErrorBuffer[dwSystemErrorBufferSize - 1] = 0;
			lpSystemErrorBuffer[dwSystemErrorBufferSize - 2] = 0;
		}

		szMessageBuffer = (LPWSTR)UtAlloc(nFMTLen + 2048);
		if (szMessageBuffer)
		{
			va_start(vl, szFmt);
			StringCchVPrintfW(szMessageBuffer, nFMTLen + 2048, szFmt, vl);
			va_end(vl);

			if (SUCCEEDED(StringCchLengthW(lpSystemErrorBuffer, 1024, &nSEBLen)) && SUCCEEDED(StringCchLengthW(szMessageBuffer, 1024, &nMBLen)))
			{
				LPWSTR sz1 = UtLoadString(IDS_WINAPI_ERROR), sz2 = UtLoadString(IDS_ERROR);
				if (sz1 && sz2)
				{
					size_t nTotalSize;

					if (SUCCEEDED(StringCchLengthW(sz1, 1024, &nTotalSize)))
					{
						nTotalSize += nMBLen + nSEBLen;

						LPWSTR lpBuffer = (LPWSTR)UtAlloc(nTotalSize);

						if (lpBuffer)
						{
							StringCchPrintfW(lpBuffer, nTotalSize, sz1, szMessageBuffer, lpSystemErrorBuffer);
							MessageBoxW(hWnd, lpBuffer, sz2, MB_OK | MB_ICONERROR);
							UtFree(lpBuffer);
						}
					}

					if (sz1)
						UtFree(sz1);

					if (sz2)
						UtFree(sz2);
				}
			}

			UtFree(szMessageBuffer);
		}

		if (dwSystemErrorBufferSize > 0)
			LocalFree(lpSystemErrorBuffer);
	}
}
