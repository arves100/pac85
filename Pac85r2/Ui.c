#include "StdAfx.h"
#include "Ui.h"

VOID UiFreeHandle(struct SUiHandle* lpsHandle)
{
	if (lpsHandle)
	{
		if (lpsHandle->hWindow)
		{
			DestroyWindow(lpsHandle->hWindow);
			lpsHandle->hWindow = NULL;
		}

		if (lpsHandle->aClass)
		{
			UnregisterClass(MAKEINTATOM(lpsHandle->aClass), GetModuleHandleW(NULL));
			lpsHandle->aClass = NULL;
		}

		switch (lpsHandle->nWindowId)
		{
		case WINDOWID_MAIN:
		{
			struct SUiMainWindow* lpsMw = (struct SUiMainWindow*)lpsHandle;

			if (lpsMw->hBrush)
			{
				DeleteObject(lpsMw->hBrush);
				
			}
			break;
		}
		default:
			break;
		}
	}
}