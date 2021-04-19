#pragma once
#include <thread>
#include <chrono>
#include <future>
#include <list>

#include "TimelineData.h"
#include "GameCapture.h"
#include "ParserEngine.h"
#include "EventBuilder.h"

class CValorantReader
{
public:
	CValorantReader();
	virtual ~CValorantReader();

	bool			InitInstance();
	bool			ExitInstance();

	void 			Start();					// Create Instances using as new or malloc
	void 			Stop();

	bool 			Initialize();				// Reset values such as EventType
	bool			IsRunning() const;
	bool 			IsStopped() const;
	bool 			Pipeline();					// A single function loop to capture a frame,
												// to recognize the frame and to output it.

	int8_t			GetLastWeapon(std::list<CTimelineData>& aryTimeline);
	bool 			ExportSlice(CTimelineData& out);
	bool 			ExportAll(std::list<CTimelineData>& outArray);

	static int		Run(CValorantReader* pThreadOwner);	// A threaded function that loops pipeline inside

public:
	std::list<CTimelineData>	m_aryTimelineData;

private:
	bool						m_isRunning;

	CGameCapture*				m_pGameCapturer;
	CParserEngine*				m_pEngine;
	CEventBuilder*				m_pEventBuilder;

	std::thread					m_thread;
	std::promise<void>			m_exitSignal;
	std::future<void>			m_exitFuture;
};
