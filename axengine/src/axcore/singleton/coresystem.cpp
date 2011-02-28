/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

AX_BEGIN_NAMESPACE

CoreSystem::CoreSystem()
	: m_initialized(0)
	, m_logBufPos(0)
	, m_lastId(0)
{
	m_linePos = 0;
	m_canExit = false;
}

CoreSystem::~CoreSystem()
{
}


void CoreSystem::initialize()
{
	if (m_initialized)
		return;

	memset(m_logBuf, 0, sizeof(m_logBuf));

	Printf(S_COLOR_RED "Axon Engine Version ");
	Printf(AX_VERSION "\n");

	Printf(_("Initializing System...\n"));

	// setup cpuinfo
	DetectCpuInfo(m_cpuInfo);
	printCpuInfo();

	// init engine time and random seed
	OsUtil::seconds();

	::setlocale(LC_ALL, ".ACP");
	Printf(_("..set locale to system default\n"));


#if 0
	TypeZeroArray(m_events);
	m_eventReadPos = 0;
	m_eventWritePos = 0;
#endif

	m_initialized = true;

	Printf(_("Initialized BaseSystem\n"));
}

void CoreSystem::finalize()
{
	Printf(_("Finalizing BaseSystem...\n"));
	Printf(_("Finalized BaseSystem\n"));
}

// register output system, core system can output info to multi target at
// the same time
void CoreSystem::registerLog(ILogHandler *log)
{
	size_t i;

	for (i=0; i<m_logSeq.size(); i++) {
		if (m_logSeq[i] == log)
			break;
	}

	if (i != m_logSeq.size()) {
		return;
	}

	m_logSeq.push_back(log);

	// print logbuf to new logsystem
	if (m_logBufPos < LOGBUFSIZE) {		// direct print if not rewind
		log->print(m_logBuf);
	} else {							// rewinded logbuf
		uint_t pos = m_logBufPos & LOGBUFSIZEMASK;

		// print first segment
		log->print(m_logBuf+pos);

		// backup cursor char
		char c = m_logBuf[pos];
		m_logBuf[pos] = 0;

		// print rewinded segment
		log->print(m_logBuf);

		// restore backuped char
		m_logBuf[pos] = c;
	}
}

void CoreSystem::removeLog(ILogHandler *log)
{
	std::vector<ILogHandler*>::iterator it;
//	mLogCol::iterator it;

	it = m_logSeq.begin();

	for (; it != m_logSeq.end(); ++it) {
		if (*it == log)
			break;
	}

	if (it == m_logSeq.end())
		return;

	m_logSeq.erase(it);
}

void CoreSystem::print(const char *text)
{
	// save to log buffer
	const char *p = text;

	for (; *p; p++) {
		m_line[m_linePos] = *p;
		m_linePos++;

		if (*p == '\n') {
			m_line[m_linePos] = 0;
			m_linePos = 0;
			printLine();
		}

		if (m_linePos == LINE_SIZE) {
			m_line[LINE_SIZE-2] = '\n';
			m_line[LINE_SIZE-1] = 0;
			m_linePos = 0;
			printLine();
		}
	}
}

void CoreSystem::printLine()
{
	const char *p = m_line;
	for (; *p; p++) {
		m_logBuf[m_logBufPos&LOGBUFSIZEMASK] = *p;
		m_logBufPos++;
	}

	std::vector<ILogHandler*>::iterator it=m_logSeq.begin();
	for (; it!=m_logSeq.end(); ++it) {
		(*it)->print(m_line);
	}
}

int CoreSystem::generateId()
{
	return ++m_lastId;
}

void CoreSystem::registerProgress(IProgressHandler *progress)
{
	size_t i;

	for (i=0; i<m_progressSeq.size(); i++) {
		if (m_progressSeq[i] == progress)
			break;
	}

	if (i != m_progressSeq.size()) {
		return;
	}

	m_progressSeq.push_back(progress);
}

void CoreSystem::removeProgress(IProgressHandler *progress)
{
	std::vector<IProgressHandler*>::iterator it;
	//	mLogCol::iterator it;

	it = m_progressSeq.begin();

	for (; it != m_progressSeq.end(); ++it) {
		if (*it == progress)
			break;
	}

	if (it == m_progressSeq.end())
		return;

	m_progressSeq.erase(it);
}

// implement IProgressHandler
void CoreSystem::beginProgress(const std::string &title)
{
	sys_noSleep.setBool(true);

	std::vector<IProgressHandler*>::iterator it;
	for (it=m_progressSeq.begin(); it!=m_progressSeq.end(); ++it) {
		(*it)->beginProgress(title);
	}
}

// return false if want go on, otherwise return true
bool CoreSystem::showProgress(uint_t percent, const std::string &msg)
{
	std::vector<IProgressHandler*>::iterator it;
	bool ret;

	for (it=m_progressSeq.begin(); it!=m_progressSeq.end(); ++it) {
		ret |= (*it)->showProgress(percent, msg);
	}

	return ret;
}

void CoreSystem::endProgress()
{
	std::vector<IProgressHandler*>::iterator it;
	for (it=m_progressSeq.begin(); it!=m_progressSeq.end(); ++it) {
		(*it)->endProgress();
	}

	sys_noSleep.setBool(false);
}

void CoreSystem::printCpuInfo()
{
	Printf(_("..found %s\n"), m_cpuInfo.cpu_type.c_str());
	Printf(_("..found %d logical cores\n"), m_cpuInfo.numLogicCores);

	std::string features = "..features:";

	if (m_cpuInfo.features & PF_MMX)
		features += " MMX";
	if (m_cpuInfo.features & PF_3DNOW)
		features += " 3DNOW";
	if (m_cpuInfo.features & PF_SSE)
		features += " SSE";
	if (m_cpuInfo.features & PF_SSE2)
		features += " SSE2";
	if (m_cpuInfo.features & PF_SSE3)
		features += " SSE3";
	if (m_cpuInfo.features & PF_ALTIVEC)
		features += " ALTIVEC";
	if (m_cpuInfo.features & PF_HTT)
		features += " HTT";

	Printf("%s\n", features.c_str());
}

void CoreSystem::registerTickable(TickPriority priority, ITickable *tickable)
{
	m_tickableLists[priority].push_back(tickable);
}

void CoreSystem::removeTickable(TickPriority priority, ITickable *tickable)
{
	m_tickableLists[priority].remove(tickable);
}

int CoreSystem::run()
{
	while (!m_canExit) {
		forceTick(0);
	}

	return 0;
}

void CoreSystem::forceTick(int mssleep)
{
	RefObject::checkDeferredDeleteObject();
	Event::processEvents();

	for (int i = 0; i < TickNumber; i++) {
		AX_FOREACH(ITickable *tickable, m_tickableLists[i]) {
			tickable->tick();
		}
	}

	if (mssleep && !sys_noSleep.getBool())
		OsUtil::sleep(mssleep);
}

AX_END_NAMESPACE

