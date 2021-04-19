#pragma once
#include "pch.h"

#define GLogA	FormatedOutputDebugStringA
#define GLogW	FormatedOutputDebugStringW

void FormatedOutputDebugStringA(const char* lpszFormat, ...)
{
	char				szBuffer[0x20000];
	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf_s(szBuffer, sizeof(szBuffer), lpszFormat, args);
	va_end(args);

	::OutputDebugStringA(szBuffer);
}

void FormatedOutputDebugStringW(const WCHAR* lEndPtrFormat, ...)
{
	wchar_t			wszMyOutputLog[0x20000];
	va_list args;
	va_start(args, lEndPtrFormat);
	_vsnwprintf_s(wszMyOutputLog, sizeof(wszMyOutputLog), lEndPtrFormat, args);
	va_end(args);
	::OutputDebugStringW(wszMyOutputLog);
}

