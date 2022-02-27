#include "StdAfx.h"
#include "File.h"
#include "Utils.h"
#include "resource.h"

BOOL FsSave(HWND hWnd, LPCWSTR szFileName, DWORD dwSize, LPBYTE lpbData)
{
	HANDLE hFile = CreateFileW(szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL bRet = FALSE;

	if (!hFile)
	{
		CONST DWORD dwLastError = GetLastError();
		if (dwLastError == ERROR_FILE_EXISTS)
		{
			// File exists let's try to append to it
			hFile = CreateFileW(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (!hFile)
				UtShowError2(hWnd, IDS_CANNOT_OPEN_FILE, szFileName);
		}
		else
		{
			SetLastError(dwLastError);
			UtShowError2(hWnd, IDS_CANNOT_OPEN_FILE, szFileName);
		}
	}

	if (hFile)
	{
		DWORD dwWrittenSize = 0;
		if (WriteFile(hFile, lpbData, dwSize, &dwWrittenSize, NULL))
		{
			if (dwWrittenSize != dwSize)
			{
				UtShowError2(hWnd, IDS_CANNOT_WRITE_ALL_DATA_TO_FILE, szFileName, dwWrittenSize, dwSize);
			}
			else
				bRet = TRUE;
		}
		else
			UtShowError2(hWnd, IDS_CANNOT_OPEN_FILE, szFileName);

		CloseHandle(hFile);
	}

	return bRet;
}


LPWSTR FsCreateSavePath(HWND hWnd, LPWSTR lpAdditionalPath)
{
	HRESULT hResult;
	LPWSTR lpSavePath = NULL;

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
	LPWSTR lpAppDataPath;
	KNOWNFOLDERID guidProgramData = FOLDERID_RoamingAppData;

	hResult = SHGetKnownFolderPath(&guidProgramData, 0, NULL, &lpAppDataPath);
#else
	LPWSTR lpAppDataPath[MAX_PATH + 1];
	ZeroMemory(lpAppDataPath, MAX_PATH + 1);
	hResult = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, lpAppDataPath);
#endif
	
	if (FAILED(hResult))
	{
		UtShowHResultError2(hWnd, IDS_CANNOT_GET_SAVE_PATH, hResult);
	}
	else
	{
		size_t nTotalSize;
		if (SUCCEEDED(StringCchLengthW(lpAppDataPath, MAX_PATH, &nTotalSize)))
		{
			size_t nAddSize;
			if (SUCCEEDED(StringCchLengthW(lpAdditionalPath, MAX_PATH, &nAddSize)))
			{
				nTotalSize += nAddSize + 20;
				lpSavePath = (LPWSTR)UtAlloc(nTotalSize);
				StringCchPrintfW(lpSavePath, nTotalSize, L"%s\\.pac85\\%s", lpAppDataPath, lpAdditionalPath);
			}
		}

		CoTaskMemFree(lpAppDataPath);
	}

	return lpSavePath;
}
