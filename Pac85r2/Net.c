#include "StdAfx.h"
#include "Net.h"
#include "Utils.h"
#include "File.h"
#include "resource.h"

BOOL NetDownloadFile(HWND hWnd, LPCWSTR szUrl, LPCWSTR szRemoteFileUrl, LPCWSTR szSaveFileName, HINTERNET hSession)
{
	BOOL bRet = FALSE;
	HINTERNET hConnect = WinHttpConnect(hSession, szUrl, INTERNET_DEFAULT_HTTP_PORT, 0);

	if (hConnect)
	{
		HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", szRemoteFileUrl, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

		if (!hRequest)
		{
			UtShowError2(hWnd, IDS_CANNOT_OPEN_REQ, szUrl, szRemoteFileUrl);
		}
		else
		{
			if (!WinHttpSendRequest(hRequest, L"Connection: Close\r\n", 20, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
			{
				UtShowError2(hWnd, IDS_CANNOT_SEND_REQ, szUrl, szRemoteFileUrl);
			}
			else
			{
				if (!WinHttpReceiveResponse(hRequest, NULL))
				{
					UtShowError2(hWnd, IDS_CANNOT_RECEIVE_RESP, szUrl, szRemoteFileUrl);
				}
				else
				{
					DWORD dwSize, dwDownloaded = 0;
					LPBYTE lpbData;

					do
					{
						dwSize = 0;
						if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
						{
							UtShowError2(hWnd, IDS_CANNOT_QUERY_DATA, szUrl, szRemoteFileUrl);
							break;
						}

						lpbData = (LPBYTE)UtAlloc(dwSize + 1);
						
						if (lpbData)
						{
							if (!WinHttpReadData(hRequest, (LPVOID)lpbData, dwSize, &dwDownloaded))
							{
								UtShowError2(hWnd, IDS_CANNOT_READ_DATA, szUrl, szRemoteFileUrl);
								dwSize = 0;
							}
							else
							{
								if (!FsSave(hWnd, szSaveFileName, dwDownloaded, lpbData))
									dwSize = 0;
							}

							UtFree(lpbData);
						}
						
					} while (dwSize > 0);
				}
			}

			WinHttpCloseHandle(hRequest);
		}

		WinHttpCloseHandle(hConnect);
	}
	else
	{
		UtShowError2(hWnd, IDS_CANNOT_CONNECT, szUrl);
	}
	
	return bRet;
}

HINTERNET NetMakeHttpSession(HWND hWnd)
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
		UtShowError2(hWnd, IDS_CANNOT_CREATE_SESSION);
	}

	return hSession;
}
