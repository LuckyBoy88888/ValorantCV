#include "logs.h"
#include "ValorantReader.h"

CValorantReader::CValorantReader()
	: m_pGameCapturer (nullptr)
	, m_pEngine (nullptr)
	, m_pEventBuilder (nullptr)
	, m_isRunning (true)
	, m_exitFuture(m_exitSignal.get_future())
{
}

CValorantReader::~CValorantReader()
{
	ExitInstance();
}

bool CValorantReader::InitInstance()
{
	m_pGameCapturer	= new CGameCapture();
	m_pEngine		= new CParserEngine();
	m_pEventBuilder = new CEventBuilder();

	Initialize();
	
	return true;
}

bool CValorantReader::Initialize()
{
	m_aryTimelineData.clear();

	return true;
}

bool CValorantReader::ExitInstance()
{
	Stop();

	SAFE_DELETE(m_pGameCapturer);
	SAFE_DELETE(m_pEngine);
	SAFE_DELETE(m_pEventBuilder);

	return true;
}

uint64_t g_time = 0;

void CValorantReader::Start()
{
	g_time = GetTickCount64();
	m_isRunning = true;
	// Create thread and run.
	m_thread = std::thread(&CValorantReader::Run, this);
}

void CValorantReader::Stop()
{
	m_isRunning = false;

	// Stop thread and wait for exit
	m_exitSignal.set_value();
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

bool CValorantReader::IsRunning() const
{
	return m_isRunning;
}

int CValorantReader::Run(CValorantReader* pThreadOwner)
{
	if (!pThreadOwner) {
		return 0;
	}

	while (!pThreadOwner->IsStopped())
	{
		pThreadOwner->Pipeline();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}

	return 0;
}

bool CValorantReader::IsStopped() const
{
	if (m_exitFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
		return false;
	return true;
}

int8_t CValorantReader::GetLastWeapon(std::list<CTimelineData>& aryTimeline)
{
	if (aryTimeline.size() == 0)
	{
		return 0;
	}

	CTimelineData &prevSlice = aryTimeline.back();
	if (prevSlice.m_playerState.m_weaponId == -1)
	{
		return 0;
	}

	return prevSlice.m_playerState.m_weaponId;
}

bool CValorantReader::Pipeline()
{
	int width = 0;
	int height = 0;
	uint8_t* buffer = nullptr;

	// Capture Game Screen
	if (!m_pGameCapturer->CaptureScreen(buffer, width, height))
	{
		GLogA("Game Screen Capture Failed.\n");
		return false;
	}

	// Recognize the Game Screen
	CTimelineData currSlice;
	int8_t lastWeapon = GetLastWeapon(m_aryTimelineData);
	if (!m_pEngine->Run(buffer, width, height, lastWeapon, currSlice))
	{
		GLogA("Parsing Game Screen Failed.\n");
		return false;
	}

	// Build Game Events
	m_currSlice = currSlice;
	m_aryTimelineData.push_back(m_currSlice);


	GLogA("Captured Image: %d, fps: %0.1f\n", m_aryTimelineData.size(), m_aryTimelineData.size()*1000 / (float)(GetTickCount64() - g_time));

	// Save Samples
	//SaveBitmap("Output.bmp", width, height, buffer);
	SAFE_DELETEA(buffer);

	return false;
}

bool CValorantReader::ExportSlice(CTimelineData& out)
{
	return false;
}

bool CValorantReader::ExportAll(std::list<CTimelineData>& outArray)
{
	return false;
}

void SaveBitmap(const char* fileName, int width, int height, uint8_t* pImg)
{
	BITMAPFILEHEADER    bitmapFileHeader;
	bitmapFileHeader.bfType = 0x4d42;   // 'BM' WINDOWS_BITMAP_SIGNATURE
	bitmapFileHeader.bfSize = ((3 * width + 3) & ~3) * height + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bitmapFileHeader.bfReserved1 = bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = (uint32_t)sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER bitmapInfoHeader;
	ZeroMemory(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;

	FILE* fp;
	if (!fopen_s(&fp, fileName, "wb"))
	{
		fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
		fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
		for (int i = height - 1; i >= 0; i--)
		{
			fwrite(pImg + i * 3 * width, 1, 3 * width, fp);
			for (int j = 0; j < ((3 * width + 3) & ~3) - 3 * width; j++)
			{
				char z = 0;
				fwrite(&z, 1, 1, fp);
			}
		}
		fclose(fp);
	}
}
