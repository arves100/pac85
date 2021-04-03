#include "StdAfx.h"
#include "Utils.h"

HRESULT UtlMalloc(_In_ SIZE_T nTotalSize, _Out_ LPVOID* lpOut)
{
	if (!lpOut || !nTotalSize)
		return E_INVALIDARG;
	
	*lpOut = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nTotalSize);

	if (!*lpOut)
		return E_OUTOFMEMORY;

	return S_OK;
}

HRESULT UtlFree(_In_ LPVOID lpData)
{
	if (lpData)
		HeapFree(GetProcessHeap(), 0, lpData);

	return S_OK;
}
