#pragma once
#include <stdio.h>
#include <string>
#include <stdarg.h>

#define GLogA	FormatedOutputDebugString
/*
inline void FormatedOutputDebugStringA(const char* lpszFormat, ...)
{
	char				szBuffer[0x20000];
	va_list args;
	va_start(args, lpszFormat);
	_vsnprintf_s(szBuffer, sizeof(szBuffer), lpszFormat, args);
	va_end(args);

	::OutputDebugStringA(szBuffer);
}

inline void FormatedOutputDebugStringW(const WCHAR* lEndPtrFormat, ...)
{
	wchar_t			wszMyOutputLog[0x20000];
	va_list args;
	va_start(args, lEndPtrFormat);
	_vsnwprintf_s(wszMyOutputLog, sizeof(wszMyOutputLog), lEndPtrFormat, args);
	va_end(args);
	::OutputDebugStringW(wszMyOutputLog);
}
*/
inline std::string FormatedOutputDebugString(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	char szBuffer[1024] = "";
	_vsnprintf_s(szBuffer, sizeof(szBuffer), format, args);
	printf_s("%s", szBuffer);
	va_end(args);

	return szBuffer;
}


