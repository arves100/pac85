#ifndef PAC85_STDAFX_H
#define PAC85_STDAFX_H

/* Exclude useless stuff from W32 inclusions (also Ws1) */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif /* !WIN32_LEAN_AND_MEAN */

/* Unicode/UCS2 support */
#ifndef _UNICODE
#error "This application must be built in Unicode"
#endif /* !_UNICODE */

/* Memory leak detection */
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 1
#endif /* _DEBUG */

/* STRICT windows conversion types */
#ifndef STRICT
#define STRICT 1
#endif /* !STRICT */

/* For having debug crtdbg */
#include <stdlib.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

/* Windows SDK inclusions */
#include <Windows.h>
#include <CommCtrl.h>
#include <winhttp.h>

/* Custom messages, see DOC.TXT for more information */

#define WM_SWITCH_DIALOG			(WM_USER + 0x01)
#define WMNET_DISCONNECTED			(WM_USER + 0x02)
#define WMNET_CONNECTED				(WM_USER + 0x04)
#define WMNET_CANREAD				(WM_USER + 0x05)
#define WMNET_CANREADHEADERS		(WM_USER + 0x06)
#define WMNET_READED				(WM_USER + 0x07)
#define WMNET_ERROR					(WM_USER + 0x08)
#define WMNET_SSLERROR				(WM_USER + 0x09)
#define WMNET_WRITTEN				(WM_USER + 0x0A)


#endif /* PAC85_STDAFX_H */
