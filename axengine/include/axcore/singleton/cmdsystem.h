/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CCMDSYSTEM_H
#define AX_CCMDSYSTEM_H

#define AX_DECLARE_COMMAND_HANDLER(cls)						\
public:														\
	typedef cls _selfclass;									\
	typedef void (cls::*CmdFunc_t)(const CmdArgs &params);	\
	struct MyCmdEntry {										\
		const char *name;									\
		CmdFunc_t fn;										\
	};														\
	virtual void HandleCmd(const CmdArgs &params);			\
	virtual CmdEntry *GetCmdEntries(uint_t *datasize);

// begin command mapping
#define AX_BEGIN_COMMAND_MAP(cls)													\
	void cls::HandleCmd(const CmdArgs &params) {									\
		const MyCmdEntry *entry;													\
																					\
		AX_ASSERT(!params.tokened[0].empty());										\
		const char *name = params.tokened[0].c_str(); uint_t datasize;				\
		for (entry = (MyCmdEntry*)GetCmdEntries(&datasize); entry->name; entry++) {	\
			if (!StringUtil::stricmp(entry->name, name)) {							\
				(this->*(entry->fn))(params);										\
			}																		\
		}																			\
	}																				\
	CmdEntry *cls::GetCmdEntries(uint_t *datasize) {								\
		static MyCmdEntry entries[] = {


// command entry
#define AX_COMMAND_ENTRY(name, func)												\
		{ name, &_selfclass::func },


// end command mapping
#define AX_END_COMMAND_MAP()														\
		{ NULL, NULL } };															\
		*datasize = sizeof(MyCmdEntry); return (CmdEntry *)&entries[0]; }


AX_BEGIN_NAMESPACE

struct CmdArgs {
	StringSeq tokened;	// tokenized parameter by ' '(SPACE), usually use this
	String rawParam;	// raw parameter string, for some complicate, scription command
};

struct CmdEntry {
	const char *name;
	void *fn;
};

struct ICmdHandler {
	virtual CmdEntry *GetCmdEntries(uint_t *datasize) = 0;	// for internal use
	virtual void HandleCmd(const CmdArgs &params) = 0;
	virtual ~ICmdHandler() = 0 {}
};


struct Cmd {
	String name;
	ICmdHandler *handler;
};

typedef Dict<String, Cmd, hash_istr, equal_istr> CmdDict;

#if 1
class AX_API CmdSystem : public ICmdHandler
{
	AX_DECLARE_COMMAND_HANDLER(CmdSystem);

public:
	// parameters for console command execution
	enum ExecType {
		Exec_Immediately,	// execute immediately, don't add to command buffer
		Exec_Append,		// add to end of the command buffer (normal case)
		Exec_Insert // insert to command buffer head
	};

	CmdSystem();
	~CmdSystem();

	void initialize(void);
	void finalize(void);
	bool isCmd(const String &name);
	void executeString(const String &text, ExecType cet = Exec_Immediately);
	void enumerate(void (*func)(const String &name));
	void registerHandler(ICmdHandler *handler);
	void removeHandler(ICmdHandler *handler);
	void runFrame(uint_t frame_time);

public:
	// console command
	bool executeCmd(const CmdArgs &params);
	CmdArgs parseCmdString(const String &text);

	void list_f(const CmdArgs &param);
	void quit_f(const CmdArgs &param);
	void crash_f(const CmdArgs &param);
	void error_f(const CmdArgs &param);
	void debug_f(const CmdArgs &param);
	void script_f(const CmdArgs &param);
	void runFile_f(const CmdArgs &param);

	void execCmdLine(int argc, char *argv[]);
	// script command
	int list_s(int);

private:
	// member data
	CmdDict m_cmdDict;

	// console command buffer
	enum { CMD_BUFFER_SIZE = 0x4000 };
	char m_buffer[CMD_BUFFER_SIZE];
	size_t m_bufferCurSize;
};
#endif

AX_END_NAMESPACE

#endif // AX_CCMDSYSTEM_H
