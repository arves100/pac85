#include "StdAfx.h"
#include "Net.h"
#include "Utils.h"
#include "resource.h"

#if 0
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
#endif

static VOID CALLBACK NetCallback(
	_In_ HINTERNET hInternet,
	_In_ DWORD_PTR dwContext,
	_In_ DWORD dwInternetStatus,
	_In_ LPVOID lpvStatusInformation,
	_In_ DWORD dwStatusInformationLength
)
{
	struct SNet* psNet = (struct SNet*)dwContext;

	UNREFERENCED_PARAMETER(hInternet);
	
	switch (dwInternetStatus)
	{
	case WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED:
		SendMessageW(psNet->sCtx.hNotification, WMNET_DISCONNECTED, 0, 0);
		break;
		
	case WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER:
		SendMessageW(psNet->sCtx.hNotification, WMNET_CONNECTED, (WPARAM)lpvStatusInformation, (LPARAM)dwStatusInformationLength);
		break;

	case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
		SendMessageW(psNet->sCtx.hNotification, WMNET_CANREAD, (WPARAM)lpvStatusInformation, 0);
		break;
		
	case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
		SendMessageW(psNet->sCtx.hNotification, WMNET_CANREADHEADERS, 0, 0);
		break;
		
	case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
		SendMessageW(psNet->sCtx.hNotification, WMNET_READED, (WPARAM)lpvStatusInformation, (LPARAM)dwStatusInformationLength);
		break;
		
	case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		SendMessageW(psNet->sCtx.hNotification, WMNET_ERROR, (WPARAM)lpvStatusInformation, 0);
		break;
		
	case WINHTTP_CALLBACK_STATUS_SECURE_FAILURE:
		SendMessageW(psNet->sCtx.hNotification, WMNET_SSLERROR, (WPARAM)lpvStatusInformation, 0);
		break;
		
	case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
		SendMessageW(psNet->sCtx.hNotification, WMNET_WRITTEN, (WPARAM)lpvStatusInformation, 0);
		break;

	default:
		break;
	}
}

HRESULT NetStart(_In_ struct SNetCtx sCtx, _Out_ struct SNet** psOut)
{
	struct SNet* psNet;

	if (FAILED(UtlMalloc(sizeof(struct SNet), (LPVOID*)&psNet)))
		return E_OUTOFMEMORY;
	
	psNet->hSession = WinHttpOpen(L"PAC85/" TEXT(__VERSION__) L" (Windows "
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
		, WINHTTP_NO_PROXY_NAME, 
		WINHTTP_NO_PROXY_BYPASS, 
		WINHTTP_FLAG_ASYNC);

	if (!psNet->hSession)
	{
		UtlFree(psNet);
		return E_FAIL;
	}

	psNet->sCtx = sCtx;

	if (!WinHttpSetOption(psNet->hSession, WINHTTP_OPTION_CONTEXT_VALUE, psNet, sizeof(struct SNet)))
	{
		WinHttpCloseHandle(psNet->hSession);
		UtlFree(psNet);
		return E_FAIL;
	}

	WinHttpSetStatusCallback(psNet->hSession, NetCallback,
		WINHTTP_CALLBACK_STATUS_CONNECTION_CLOSED |
		WINHTTP_CALLBACK_STATUS_CONNECTED_TO_SERVER |

		WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE |
		WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE |
		WINHTTP_CALLBACK_STATUS_READ_COMPLETE |

		WINHTTP_CALLBACK_STATUS_REQUEST_ERROR |
		WINHTTP_CALLBACK_STATUS_SECURE_FAILURE |
		
		WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE
	, 0);

	*psOut = psNet;
	
	return S_OK;
}

HRESULT NetFree(_In_ struct SNet* psNet)
{
	if (psNet)
	{
		if (psNet->hRequest)
		{
			WinHttpCloseHandle(psNet->hRequest);
			psNet->hRequest = NULL;
		}
		
		if (psNet->hConnection)
		{
			WinHttpCloseHandle(psNet->hConnection);
			psNet->hConnection = NULL;
		}
		
		if (psNet->hSession)
		{
			WinHttpCloseHandle(psNet->hSession);
			psNet->hSession = NULL;
		}
	}

	return S_OK;
}

HRESULT NetConnect(_In_ struct SNet* psNet, _In_ LPCWSTR szSrvName, _In_ BOOL bHttps)
{
	psNet->hConnection = WinHttpConnect(psNet->hSession, szSrvName, bHttps ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
	return psNet->hConnection ? S_OK : E_FAIL;
}

HRESULT NetOpenRequest(_In_ struct SNet* psNet, _In_ LPCWSTR szUrl, _In_ BOOL bPOST)
{
	psNet->hRequest = WinHttpOpenRequest(psNet->hConnection, bPOST ? L"POST" : L"GET", szUrl, NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

	return psNet->hRequest ? S_OK : E_FAIL;
}

HRESULT NetSendRequest(_In_ struct SNet* psNet)
{
	return WinHttpSendRequest(psNet->hRequest, L"Connection: Close\r\n", 20, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ? S_OK : E_FAIL;
}

HRESULT NetRecvResponse(_In_ struct SNet* psNet)
{
	return WinHttpReceiveResponse(psNet->hRequest, NULL) ? S_OK : E_FAIL;
}

HRESULT NetQueryDataAvailable(_In_ struct SNet* psNet)
{
	return WinHttpQueryDataAvailable(psNet->hRequest, NULL) ? S_OK : E_FAIL;
}

HRESULT NetReadData(_In_ struct SNet* psNet, _In_ LPVOID lpBuffer, _In_ DWORD dwBufferLen)
{
	return WinHttpReadData(psNet->hRequest, lpBuffer, dwBufferLen, NULL) ? S_OK : E_FAIL;
}
