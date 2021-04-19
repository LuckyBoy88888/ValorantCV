#pragma once
#include <stdint.h>
#include <windows.h>

class CGameCapture
{
public:
	CGameCapture();

	bool CaptureScreen(uint8_t* &buffer, int& width, int& height);

private:
	bool BitmapToBuffer(HDC hDC, HBITMAP hBitmap, uint8_t* &buffer);

};
