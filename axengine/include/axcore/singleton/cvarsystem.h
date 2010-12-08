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

class AX_API Cvar
{
public:
	friend class CvarSystem;

	enum Flag {
		Archive = 1, // save to config file
		Init = 2, // don't allow change from console, but can be set from the command line
		Latch = 4, // changed after restart
		Readonly = 8, // read only
		Temp = 0x10, // temp created, not save to config file
		Cheat = 0x20, // for debug only, only cheat mode can modify this
	};

	Cvar(const std::string &name, const std::string &default_string, int flags);
	~Cvar();

	const std::string &getName() const;
	int getFlags() const;

	const std::string &getString() const;
	float getFloat() const;
	int getInteger() const;
	bool getBool() const { return getInteger() ? true : false; }

	void setString(const std::string &sz_value);
	void setFloat(float float_value);
	void setInt(int int_value);
	void setBool(bool b);

	bool isModified() const;
	void clearModifiedFlag();

	void forceSet(const std::string &sz_value);
	void forceSet(float float_value);
	void forceSet(int int_value);
	void setString(const std::string &sz, bool force);

private:
	std::string m_name;
	std::string m_defaultString; // cvar_restart will reset to this value
	std::string m_latchedString; // for CVF_latch vars
	std::string m_stringValue;
	int m_flags;
	bool m_modified; // set each time the cvar is changed
	uint_t m_modifiedCount; // incremented each time the cvar is changed
	float m_floatValue; // atof(m_stringValue)
	int m_integerValue; // atoi(m_stringValue)

	Cvar *m_staticNext;
	static Cvar *ms_staticLink;
};

inline const std::string & Cvar::getName() const
{
	return m_name;
}

inline const std::string & Cvar::getString() const
{
	return m_stringValue;
}

inline int Cvar::getFlags() const
{
	return m_flags;
}

inline float Cvar::getFloat() const
{
	return m_floatValue;
}

inline int Cvar::getInteger() const
{
	return m_integerValue;
}

inline bool Cvar::isModified() const
{
	return m_modified;
}

inline void Cvar::clearModifiedFlag()
{
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
	bool executeCommand(const CmdArgs &params);

protected:
#if 0
	Cvar *createCvar(const String &name, const String &defaultString, uint_t flags);
	Cvar *createCvar(const String &name, const String &defaultString);
#endif
	void registerCvar(Cvar *cvar);
	void removeCvar(const std::string &name);

private:
	void set(const std::string &name, const std::string& value);

	// console command
	void set_f(const CmdArgs &param);
	void list_f(const CmdArgs &param);
	void toggleCvar_f(const CmdArgs &param);

private:
	// member data
	typedef Dict<std::string, Cvar*> CvarDict;
	CvarDict m_cvarDict;
	uint_t m_dirtyFlags;

	Dict<std::string, std::string> m_penddingSets;
};

AX_END_NAMESPACE

#endif // end guardian
