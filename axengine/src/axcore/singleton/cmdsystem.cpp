/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"


namespace Axon {

	AX_BEGIN_COMMAND_MAP(CmdSystem)
		AX_COMMAND_ENTRY("cmdlist",	list_f)
		AX_COMMAND_ENTRY("quit",	quit_f)
		AX_COMMAND_ENTRY("exit",	quit_f)
		AX_COMMAND_ENTRY("crash",	crash_f)
		AX_COMMAND_ENTRY("error",	error_f)
		AX_COMMAND_ENTRY("debug",	debug_f)
		AX_COMMAND_ENTRY("script",	script_f)
		AX_COMMAND_ENTRY("runfile",	runFile_f)
	AX_END_COMMAND_MAP()

	CmdSystem::CmdSystem()
		: m_cmdDict()
	{}

	CmdSystem::~CmdSystem() {
	}

	void CmdSystem::initialize(void) {
		Printf(_("Initializing CmdSystem...\n"));
		registerHandler(this);
		Printf(_("Initialized CmdSystem\n"));
	}

	void CmdSystem::finalize(void) {
		Printf(_("Finalizing CmdSystem...\n"));
		removeHandler(this);
		Printf(_("Finalized CmdSystem\n"));
	}

	bool CmdSystem::isCmd(const String& name) {
		if (m_cmdDict.find(name) != m_cmdDict.end())
			return true;
		else
			return false;
	}

	void CmdSystem::executeString(const String& text, ExecType cet) {
		// check if is script command first
		if (text.size() > 1) {
			if (text[0] == L'/') {
				g_scriptSystem->executeString(text.c_str() + 1);
				return;
			}
		}
		CmdArgs param = parseCmdString(text);

		if (param.tokened.size() == 0)
			return;

		if (executeCmd(param))
			return;

		if (g_cvarSystem->executeCommand(param))
			return;

		Printf(_("Cann't found command or variable '%s'\n"), param.tokened[0].c_str());
	}

	void CmdSystem::enumerate(void (*func)(const String& name)) {
	}

	void CmdSystem::registerHandler(ICmdHandler* handler) {
		uint_t datasize;
		__CmdEntry* entry = handler->GetCmdEntries(&datasize);
		Cmd cmd;

		for (; entry->name; ((byte_t*&)entry)+=datasize) {
			cmd.name = entry->name;
			cmd.handler = handler;
			m_cmdDict[entry->name] = cmd;
		}
	}

	void CmdSystem::removeHandler(ICmdHandler* handler) {
		uint_t datasize;
		__CmdEntry* entry = handler->GetCmdEntries(&datasize);

		for (; entry->name; ((byte_t*&)entry)+=datasize) {
			m_cmdDict.erase(entry->name);
		}
	}

	void CmdSystem::runFrame(uint_t frame_time) {
	}


	bool CmdSystem::executeCmd(const CmdArgs& params) {
		CmdDict::iterator it = m_cmdDict.find(params.tokened[0]);
		if (it == m_cmdDict.end())
			return false;

		it->second.handler->HandleCmd(params);

		return true;
	}

	CmdArgs CmdSystem::parseCmdString(const String& text) {
		CmdArgs param;

		param.tokened = StringUtil::tokenizeSeq(text.c_str());

		size_t pos = text.find(L' ');

		if (pos != String::npos) {
			param.rawParam = text.c_str() + pos + 1;
		}

		return param;
	}

	void CmdSystem::list_f(const CmdArgs& param) {
		CmdDict::iterator it = m_cmdDict.begin();
		int count = 0;
		const char* arg;

		if (param.tokened.size() > 1) {
			arg = param.tokened[1].c_str();
		} else {
			arg = NULL;
		}

		for (; it!=m_cmdDict.end(); ++it) {
			if (arg && !StringUtil::filterString(arg, (*it).second.name.c_str(), false)) {
				continue;
			}

			Printf("%s\n", (*it).second.name.c_str());
			count++;
		}

		Printf(_("%d total console commands\n"), count);
	}

	void CmdSystem::quit_f(const CmdArgs& param) {
		g_system->finalize();

		exit(0);
	}

	void CmdSystem::crash_f(const CmdArgs& param) {
		*(int *)NULL = 0x12345678;
	}

	void CmdSystem::error_f(const CmdArgs& param) {
		const char* msg;
		if (param.tokened.size() > 1) {
			msg = param.tokened[1].c_str();
		} else {
			msg = _("Console fire error");
		}

		Errorf(msg);
	}

	void CmdSystem::debug_f(const CmdArgs& param) {
		AX_ASSERT(0);
	}

	void CmdSystem::script_f(const CmdArgs& param) {
		g_scriptSystem->executeString(param.rawParam);
	}

	void CmdSystem::runFile_f(const CmdArgs& param) {
		if (param.tokened.size() < 2)
			return;

	//	SCOPE_CONVERT;

	#if 1
		g_scriptSystem->executeFile(param.tokened[1]);
	#else
		size_t filesize;
		char* filebuf;

		filesize = g_fileSystem->ReadFile(param.tokened[1], (void**)&filebuf);

		if (!filesize || !filebuf)
			return;

		g_scriptSystem->executeString(U2W(filebuf));
	#endif
	}

	int CmdSystem::list_s(int) {
		return 0;
	}

	void CmdSystem::execCmdLine(int argc, char* argv[]) {
		CmdArgs cmdparam;
		for (int i = 0; i < argc; i++) {
			// not recognize
			if (argv[i][0] != '+') {
				continue;
			}

			if (!cmdparam.tokened.empty()) {
				this->executeCmd(cmdparam);
				cmdparam.tokened.clear();
			}

			cmdparam.tokened.push_back(argv[i]+1);

			i++;
			while (i < argc) {
				if (argv[i][0] == '+') {
					break;
				}

				cmdparam.tokened.push_back(argv[i++]);
			}
		}

		if (!cmdparam.tokened.empty()) {
			this->executeCmd(cmdparam);
			cmdparam.tokened.clear();
		}
	}



} // namespace Axon


