/*
 * The Arch Linux installer of Windows
 *
 * Bless PAC85 our true saviour!
 * Completely made with Win32 API.
 *
 * Copyright © 2020-2021.
 *
 * Distribution, ownership, reproduction, and usage without explicit
 * authorization is strictly prohibited.
 */
/*!
 * File: StdAfx.h
 * Description: General inclusions for precompiled headers.
 */
#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif // !WIN32_LEAN_AND_MEAN

#ifndef _UNICODE
#define _UNICODE 1
#endif // !_UNICODE

#ifndef UNICODE
#define UNICODE 1
#endif // !UNICODE

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC 1
#endif

#ifdef _MSC_VER
#pragma comment(lib, "Winhttp.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#endif

#include <stdlib.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <Windows.h>
#include <winhttp.h>
#include <ShlObj.h>
#include <sdkddkver.h>
#include <strsafe.h>
#include <stdarg.h>
