/*
	The Arch Linux Installation Program for Windows
	
	Official website: http://sigterm.cc/pac85/
	
	Bless PAC85 our true saviour!
	Completely made with Win32 API.
	Released under GPLv3.

	Compile it with:
		cl.exe /EHsc PAC85.C
*/

#ifndef _UNICODE
#define _UNICODE
#endif // !_UNICODE

#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winhttp.h>
#include <ShlObj.h>
#include <sdkddkver.h>

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _MSC_VER
#pragma comment(lib, "Winhttp.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#endif

#define DEFAULT_MIRROR L"mirrors.prometeus.net"

static void ShowError(LPCWSTR szFmt, ...)
{
	LPWSTR lpSystemErrorBuffer = NULL;
	va_list vl;
	WCHAR szMessageBuffer[1024];
	DWORD dwSystemErrorBufferSize = 0;
	ZeroMemory(szMessageBuffer, 1024);
	
	dwSystemErrorBufferSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), LANG_USER_DEFAULT, (LPWSTR)&lpSystemErrorBuffer, 1, NULL);

	if (dwSystemErrorBufferSize > 0)
	{
		lpSystemErrorBuffer[dwSystemErrorBufferSize - 1] = 0;
		lpSystemErrorBuffer[dwSystemErrorBufferSize - 2] = 0;
	}

	va_start(vl, szFmt);
	_vsnwprintf_s(szMessageBuffer, _countof(szMessageBuffer), 1024, szFmt, vl);
	va_end(vl);

	size_t dwTotalSize = wcslen(lpSystemErrorBuffer) + wcslen(szMessageBuffer) + 30;

	if (dwTotalSize > 30)
	{
		LPWSTR lpBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTotalSize);

		if (!lpBuffer)
		{
			MessageBoxW(NULL, L"Cannot allocate buffer to show error", L"Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			_snwprintf_s(lpBuffer, dwTotalSize, dwTotalSize, L"%s.\nWinAPI error: %s", szMessageBuffer, lpSystemErrorBuffer);
			MessageBoxW(NULL, lpBuffer, L"Error", MB_OK | MB_ICONERROR);
			HeapFree(GetProcessHeap(), 0, lpBuffer);
		}
	}

	LocalFree(lpSystemErrorBuffer);
}

static void ShowHError(LPCWSTR szFmt, HRESULT hResult, ...)
{
	LPWSTR lpSystemErrorBuffer = NULL;
	va_list vl;
	WCHAR szMessageBuffer[1024];
	DWORD dwSystemErrorBufferSize = 0;
	ZeroMemory(szMessageBuffer, 1024);

	dwSystemErrorBufferSize = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE, NULL, hResult, LANG_USER_DEFAULT, (LPWSTR)&lpSystemErrorBuffer, 1, NULL);

	if (dwSystemErrorBufferSize > 0)
	{
		lpSystemErrorBuffer[dwSystemErrorBufferSize - 1] = 0;
		lpSystemErrorBuffer[dwSystemErrorBufferSize - 2] = 0;
	}

	va_start(vl, szFmt);
	_vsnwprintf_s(szMessageBuffer, _countof(szMessageBuffer), 1024, szFmt, vl);
	va_end(vl);

	size_t dwTotalSize = wcslen(lpSystemErrorBuffer) + wcslen(szMessageBuffer) + 30;

	if (dwTotalSize > 30)
	{
		LPWSTR lpBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTotalSize);

		if (!lpBuffer)
		{
			MessageBoxW(NULL, L"Cannot allocate buffer to show error", L"Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			_snwprintf_s(lpBuffer, dwTotalSize, dwTotalSize, L"%s.\r\nWinAPI error: %s", szMessageBuffer, lpSystemErrorBuffer);
			MessageBoxW(NULL, lpBuffer, L"Error", MB_OK | MB_ICONERROR);
			HeapFree(GetProcessHeap(), 0, lpBuffer);
		}
	}

	LocalFree(lpSystemErrorBuffer);
}

static BOOL SaveFileData(LPCWSTR szFileName, DWORD dwSize, CONST LPBYTE lpbData)
{
	HANDLE hFile = CreateFileW(szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL bRet = FALSE;

	if (!hFile)
	{
		DWORD dwLastError = GetLastError();
		if (dwLastError == ERROR_FILE_EXISTS)
		{
			// File exists let's try to append to it
			hFile = CreateFileW(szFileName, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (!hFile)
				ShowError(L"Cannot open file %s", szFileName);
		}
		else
		{
			SetLastError(dwLastError);
			ShowError(L"Cannot create file %s", szFileName);
		}
	}

	if (hFile)
	{
		DWORD dwWrittenSize = 0;
		if (WriteFile(hFile, lpbData, dwSize, &dwWrittenSize, NULL))
		{
			if (dwWrittenSize != dwSize)
			{
				ShowError(L"Unable to write all data to file %s. Data written %lu/%lu", szFileName, dwWrittenSize, dwSize);
			}
			else
				bRet = TRUE;
		}
		else
			ShowError(L"Cannot write file %s", szFileName);

		CloseHandle(hFile);
	}

	return bRet;
}

static BOOL DownloadFile(LPCWSTR szUrl, LPCWSTR szRemoteFileUrl, LPCWSTR szSaveFileName, HINTERNET hSession)
{
	BOOL bRet = FALSE;
	HINTERNET hConnect = WinHttpConnect(hSession, szUrl, INTERNET_DEFAULT_HTTP_PORT, 0);

	if (hConnect)
	{
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", szRemoteFileUrl, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

		if (!hRequest)
			ShowError(L"Cannot open request to %s for file %s", szUrl, szRemoteFileUrl);
		else
		{
			if (!WinHttpSendRequest(hRequest, L"Connection: Close\r\n", 20, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
				ShowError(L"Cannot send request to %s for file %s", szUrl, szRemoteFileUrl);
			else
			{
				if (!WinHttpReceiveResponse(hRequest, NULL))
					ShowError(L"Cannot receive response from %s for file %s", szUrl, szRemoteFileUrl);
				else
				{
					DWORD dwSize, dwDownloaded = 0;
					LPBYTE lpbData = NULL;

					do
					{
						dwSize = 0;
						if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
							ShowError(L"Cannot query available data from %s for file %s", szUrl, szRemoteFileUrl);
						
						lpbData = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize + 1);

						if (!lpbData)
						{
							ShowError(L"Cannot allocate memory for %lu (Url %s file %s)", dwSize + 1, szUrl, szRemoteFileUrl);
							dwSize = 0;
						}
						else
						{
							if (!WinHttpReadData(hRequest, (LPVOID)lpbData, dwSize, &dwDownloaded))
							{
								ShowError(L"Cannot read data from %s for file %s", szUrl, szRemoteFileUrl);
								dwSize = 0;
							}
							else
							{
								if (!SaveFileData(szSaveFileName, dwDownloaded, lpbData))
									dwSize = 0;
							}

							HeapFree(GetProcessHeap(), 0, lpbData);
						}
					} while (dwSize > 0);
				}
			}

			WinHttpCloseHandle(hRequest);
		}

		WinHttpCloseHandle(hConnect);
	}
	else
		ShowError(L"Cannot connect to %s", szUrl);

	return bRet;
}

static HINTERNET MakeHttpSession(void)
{
	HINTERNET hSession = WinHttpOpen(L"PAC85/1.0 (Windows "
#ifdef _WIN64
		L"x86_64"
#else
		L"x86"
#endif
		L" )",
#if _WIN32_WINNT >= _WIN32_WINNT_WIN8
		WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY
#else
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
#endif
		, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	
	if (!hSession)
	{
		ShowError(L"Cannot create HTTP session");
	}

	return hSession;
}

#if _WIN32_WINNT >= _WIN32_WINNT_VISTA
static LPWSTR CreateSavePath(LPWSTR lpAdditionalPath)
{
	HRESULT hResult;
	LPWSTR lpAppDataPath = NULL, lpSavePath = NULL;
	KNOWNFOLDERID guidProgramData = FOLDERID_RoamingAppData;

	hResult = SHGetKnownFolderPath(&guidProgramData, 0, NULL, &lpAppDataPath);

	if (FAILED(hResult))
		ShowHError(L"Cannot get save path", hResult);
	else
	{
		size_t dwTotalSize = wcslen(lpAppDataPath) + wcslen(lpAdditionalPath) + 20;
		lpSavePath = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, dwTotalSize);

		if (!lpSavePath)
			ShowError(L"Cannot allocate memory for save path");
		else
		{
			_snwprintf_s(lpSavePath, dwTotalSize, dwTotalSize, L"%s\\.pac85\\%s", lpAppDataPath, lpAdditionalPath);
		}

		CoTaskMemFree(lpAppDataPath);
	}

	return lpSavePath;
}
#else
static LPWSTR CreateSavePath(LPWSTR lpAdditionalPath)
{
	HRESULT hResult;
	LPWSTR szAppDataPath[MAX_PATH+1], lpSavePath = NULL;
	ZeroMemory(szAppDataPath, MAX_PATH + 1);

	hResult = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, szAppDataPath);

	if (FAILED(hResult))
		ShowHError(L"Cannot get save path", hResult);
	else
	{
		DWORD dwTotalSize = wcslen(szAppDataPath) + wcslen(lpAdditionalPath) + 13;
		lpSavePath = (LPWSTR)HeapAlloc(GetProcessHeap(), 0, dwTotalSize);

		if (!lpSavePath)
			ShowError(L"Cannot allocate memory for save path");
		else
		{
			_snwprintf_s(lpSavePath, sizeof(WCHAR) * dwTotalSize, dwTotalSize, L"%s\\.pac85\\%s", szAppDataPath, lpAdditionalPath);
		}
	}

	return lpSavePath;
}
#endif

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ INT nCmdShow)
{
	HINTERNET hSession = MakeHttpSession();
	if (hSession)
	{
		LPWSTR lpPathBuffer = CreateSavePath(L"");

		if (lpPathBuffer)
		{
			CreateDirectoryW(lpPathBuffer, NULL);
			HeapFree(GetProcessHeap(), 0, lpPathBuffer);
		}

		lpPathBuffer = CreateSavePath(L"bash-5.0.018-1-x86_64.pkg.tar.zst.sig");

		if (lpPathBuffer)
		{
			DownloadFile(DEFAULT_MIRROR, L"/archlinux/core/os/x86_64/bash-5.0.018-1-x86_64.pkg.tar.zst.sig", lpPathBuffer, hSession);
			HeapFree(GetProcessHeap(), 0, lpPathBuffer);
		}

		WinHttpCloseHandle(hSession);
	}

	return 0;
}
