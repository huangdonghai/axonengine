/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CORE_CORESYSTEM_H
#define AX_CORE_CORESYSTEM_H

AX_BEGIN_NAMESPACE

// processor features
enum CPU_FEATURE {
	PF_MMX = 0x0001,	// Multi Media Extensions
	PF_3DNOW = 0x0002,	// 3DNow! and 3d now plus
	PF_SSE = 0x0004,	// Streaming SIMD Extensions
	PF_SSE2 = 0x0008,	// Streaming SIMD Extensions 2
	PF_SSE3 = 0x0010,	// Streaming SIMD Extentions 3 aka Prescott's New Instructions
	PF_ALTIVEC = 0x0020,	// AltiVec
	PF_HTT = 0x0100,	// Hyper-Threading Technology
	PF_CMOV = 0x0200,	// Conditional Move (CMOV) and fast floating point comparison (FCOMI) instructions
	PF_FTZ = 0x0400,	// Flush-To-Zero mode (denormal results are flushed to zero)
	PF_DAZ = 0x0800 // Denormals-Are-Zero mode (denormal source operands are set to zero)
};

struct CpuInfo {
	int cpuid;
	int features;
	int family;
	int model;
	int stepping;
	uint_t numLogicCores;
	String vendor;
	String cpu_type;
};

struct ILogHandler {
	virtual void print(const char *text) = 0;
	virtual ~ILogHandler() = 0 {}	// let gcc happy
};

struct IProgressHandler {
	virtual ~IProgressHandler() {}
	virtual void beginProgress(const String &title) = 0;
	// return true if want go on, otherwise return false
	virtual bool showProgress(uint_t percent, const String &msg) = 0;
	virtual void endProgress() = 0;
};

struct INotifyHandler {
	virtual void notify(int index) = 0;
};

class ITickable {
public:
	virtual void tick() = 0;
};


class AX_API System {
public:

	enum TickPriority {
		TickEvent,
		TickGame,
		TickEditor,

		TickNumber
	};

	System();
	virtual ~System();

	void initialize();
	void finalize();

	// id will increase every generation
	int generateId();

	// register output system, base system can output info to multiply target at
	// the same time
	void registerLog(ILogHandler *log);
	void removeLog(ILogHandler *log);
	void print(const char *text);

	// register progress handler, base system can do progress showing in multiply
	// target at the same tine
	void registerProgress(IProgressHandler *progress);
	void removeProgress(IProgressHandler *progress);

	// implement IProgressHandler
	virtual void beginProgress(const String &title);
	// return true if want interrupt, false if want go on, otherwise return true
	virtual bool showProgress(uint_t percent, const String &msg);
	virtual void endProgress();

	// tick
	void registerTickable(TickPriority priority, ITickable *tickable);
	void removeTickable(TickPriority priority, ITickable *tickable);
	int run();
	void forceTick(int mssleep);
	void setExitable() { m_canExit = true; }

	// cpu info
	const CpuInfo &getCpuInfo();

protected:
	void printCpuInfo();
	void printLine();

private:
	enum {
		LOGBUFSIZE = 16*1024,
		LOGBUFSIZEMASK = LOGBUFSIZE-1,
		LINE_SIZE = 1024,
	};

	bool m_initialized;
	uint_t m_logBufPos;
	char m_logBuf[LOGBUFSIZE+1];	// add a trail nil char
	char m_line[LINE_SIZE];
	int m_linePos;

	Sequence<ILogHandler*>		m_logSeq;
	Sequence<IProgressHandler*>	m_progressSeq;

	// for id
	int m_lastId;

	// cpuinfo
	CpuInfo m_cpuInfo;

	// run loop
	typedef List<ITickable*>	ITickableList;
	ITickableList m_tickableLists[TickNumber];
	bool m_canExit;
};

inline const CpuInfo &System::getCpuInfo() { return m_cpuInfo; }


AX_END_NAMESPACE


#endif // AX_CORE_CORESYSTEM_H
