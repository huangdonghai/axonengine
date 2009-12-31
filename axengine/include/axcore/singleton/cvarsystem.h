/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CVARSYSTEM_H
#define AX_CVARSYSTEM_H

AX_BEGIN_NAMESPACE

	class Cvar;
	class CvarSystem;

	//------------------------------------------------------------------------------
	// class Cvar
	//------------------------------------------------------------------------------

	class AX_API Cvar {
	public:
		friend class CvarSystem;

		enum Flag {
			Archive = 1,		// save to config file
			Init = 2,		// don't allow change from console, but can be set from the command line
			Latch = 4,		// changed after restart
			Readonly = 8,		// read only
			Temp = 0x10,		// temp created, not save to config file
			Cheat = 0x20,		// for debug only, only cheat mode can modify this
		};

		String getName() const;
		int getFlags() const;

		String getString() const;
		float getFloat() const;
		int getInteger() const;
		bool getBool() const { return getInteger() ? true : false; }

		bool isModified() const;
		void clearModifiedFlag();
		void forceSet(const String &sz_value);
		void forceSet(float float_value);
		void forceSet(int int_value);
		void set(const String &sz, bool force);
		void set(const String &sz_value);
		void set(float float_value);
		void set(int int_value);

		// static helper function
		static Cvar *create(const String &name, const String &default_string, uint_t flags);

	private:
		Cvar();
		Cvar(const String &name, const String &default_string, int flags);
		~Cvar();

		String m_name;
		String m_defaultString;		// cvar_restart will reset to this value
		String m_latchedString;		// for CVF_latch vars
		String m_stringValue;
		int m_flags;
		bool m_modified;			// set each time the cvar is changed
		uint_t m_modifiedCount;		// incremented each time the cvar is changed
		float m_floatValue;		// atof(m_stringValue)
		int m_integerValue;		// atoi(m_stringValue)
	};

	inline String Cvar::getName() const {
		return m_name;
	}

	inline String Cvar::getString() const {
		return m_stringValue;
	}

	inline int Cvar::getFlags() const {
		return m_flags;
	}

	inline float Cvar::getFloat() const {
		return m_floatValue;
	}

	inline int Cvar::getInteger() const {
		return m_integerValue;
	}

	inline bool Cvar::isModified() const {
		return m_modified;
	}

	inline void Cvar::clearModifiedFlag() {
		m_modified = false;
	}


	//------------------------------------------------------------------------------
	// class CvarSystem
	//------------------------------------------------------------------------------


	class AX_API CvarSystem : public ICmdHandler
	{
		AX_DECLARE_COMMAND_HANDLER(CvarSystem);

	public:
		friend class Cvar;

		CvarSystem();
		virtual ~CvarSystem();

		void initialize();
		void finalize();
		Cvar *createCvar(const String &name, const String &defaultString, uint_t flags);
		Cvar *createCvar(const String &name, const String &defaultString);
		bool executeCommand(const CmdArgs &params);

	protected:
		void removeCvar(const String &name);

	private:
		// console command
		void set_f(const CmdArgs &param);
		void list_f(const CmdArgs &param);
		void toggleCvar_f(const CmdArgs &param);

	private:
		// member data
		typedef Dict<String, Cvar*, hash_istr, equal_istr> CvarDict;
		CvarDict m_cvarDict;
		uint_t m_dirtyFlags;

	};



AX_END_NAMESPACE

#endif // end guardian
