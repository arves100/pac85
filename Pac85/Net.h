#ifndef PAC85_NET_H
#define PAC85_NET_H

/*!
 * Context of the network data, this will be used by the callbacks
 */
struct SNetCtx
{
	HWND hNotification; /* Handle to the window that will receive the notification messages */
};

/*!
 * Structure that holds all the networking data
 */
struct SNet
{
	HINTERNET hSession; /* handle the the http(s) session */
	HINTERNET hConnection; /* handle to the connection to the server */
	HINTERNET hRequest; /* handle to the network request */
	struct SNetCtx sCtx; /* context */
};

/*!
 * Opens a new network session
 * @param sCtx context structure that will be copied to psOut
 * @param psOut output the structure where networking data is being held
 * @return S_OK for no error
 */
extern HRESULT NetStart(_In_ struct SNetCtx sCtx, _Out_ struct SNet** psOut);

/*!
 * Free an existing network structure
 * @param psNet the structure to free
 * @return S_OK for no error
 */
extern HRESULT NetFree(_In_ struct SNet* psNet);

/*!
 * Opens a new connection with a server
 * @param psNet network structure
 * @param szSrvName domain/ip of the server
 * @param bHttps if this is set to TRUE, the program will connect to the server by using HTTPS (and not HTTP)
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetConnect(_In_ struct SNet* psNet, _In_ LPCWSTR szSrvName, _In_ BOOL bHttps);

/*!
 * Creates a new request that will be sent to the server
 * @param psNet network structure
 * @param szUrl the url of the data to request
 * @param bPOST if this is set to TRUE, the program will sent a POST request (NOTE: custom data not supported yet)
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetOpenRequest(_In_ struct SNet* psNet, _In_ LPCWSTR szUrl, _In_ BOOL bPOST); // TODO: support custom post data

/*!
 * Sends the request created with "NetOpenRequest" to the server
 * @param psNet network structure
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetSendRequest(_In_ struct SNet* psNet);

/*!
 * Starts receiving a response from the server
 * NOTE: you need to send this command at the right time after receiving the correct message in the window
 * @param psNet network structure
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetRecvResponse(_In_ struct SNet* psNet);

/*!
 * Query if the network data is available from the server
 * @param psNet network structure
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetQueryDataAvailable(_In_ struct SNet* psNet);

/*!
 * Reads the response data from the server
 * @param psNet network structure
 * @param lpBuffer pointer to a valid buffer where the data will be held
 * @param dwBufferLen length of the buffer/data to store
 * @return S_OK for no error, E_FAIL for an error, use GetLastError() to get more information about the error
 */
extern HRESULT NetReadData(_In_ struct SNet* psNet, _In_ LPVOID lpBuffer, _In_ DWORD dwBufferLen);

#endif /* PAC85_NET_H */
