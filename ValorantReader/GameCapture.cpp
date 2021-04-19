#include <tchar.h>
#include "Common.h"
#include "GameCapture.h"

CGameCapture::CGameCapture()
{
}

bool CGameCapture::CaptureScreen(uint8_t* &buffer, int &width, int &height)
{
	bool ret = false;
	
	HDC hDC = GetDC(GetDesktopWindow());
	HDC hMemDC = CreateCompatibleDC(hDC);
	HWND hGameWnd = FindWindow(nullptr, _T("VALORANT  "));
	//HWND hGameWnd = FindWindow(nullptr, _T("MainWindow"));
	if (hGameWnd)
	{
		RECT rt;
		GetWindowRect(hGameWnd, &rt);
		width = rt.right - rt.left;
		height = rt.bottom - rt.top;

		HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
		if (hBitmap)
		{
			HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
			BitBlt(hMemDC, 0, 0, width, height, hDC, rt.left, rt.top, SRCCOPY);
			ret = BitmapToBuffer(hMemDC, hBitmap, buffer);
			SelectObject(hMemDC, hOldBitmap);
			DeleteObject(hBitmap);
		}
	}

	DeleteDC(hMemDC);
	ReleaseDC(nullptr, hDC);

	return ret;
}

bool CGameCapture::BitmapToBuffer(HDC hDC, HBITMAP hBitmap, uint8_t* &buffer)
{
	BITMAPINFO bitInfo;
	ZeroMemory(&bitInfo, sizeof(BITMAPINFO));
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biBitCount = 0;

	if (!GetDIBits(hDC, hBitmap, 0, 0, nullptr, &bitInfo, DIB_RGB_COLORS))
	{
		buffer = nullptr;
		return false;
	}

	bitInfo.bmiHeader.biCompression = BI_RGB;
	uint8_t *pBits32 = new uint8_t[bitInfo.bmiHeader.biSizeImage + 5];
	if (!GetDIBits(hDC, hBitmap, 0, bitInfo.bmiHeader.biHeight, pBits32, &bitInfo, DIB_RGB_COLORS))
	{
		SAFE_DELETEA(pBits32);
		buffer = nullptr;

		return false;
	}

	// Convert 32bit BGRA to 24bit BGR.
	buffer = new uint8_t[bitInfo.bmiHeader.biWidth * bitInfo.bmiHeader.biHeight * 3];
	uint8_t *p = buffer, *pp = nullptr;
	for (int j = bitInfo.bmiHeader.biHeight - 1; j >= 0; j--)
	{
		pp = pBits32 + j * bitInfo.bmiHeader.biWidth * 4;
		for (int i = 0; i < bitInfo.bmiHeader.biWidth; i++)
		{
			*p++ = *pp++;	//Blue
			*p++ = *pp++;	//Green
			*p++ = *pp++;	//Red

			pp++;/*Alpha*/
		}
	}

	SAFE_DELETEA(pBits32);

	return true;
}
