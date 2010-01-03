/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_CONFIGFILE_H
#define AX_CONFIGFILE_H


AX_BEGIN_NAMESPACE

/// configure file include
///		sections
///			items: key/value pairs
/// newer item with same name will overwrite old one

/*-----------------------------------------------------------------------------
	ConfigFile
-----------------------------------------------------------------------------*/

class AX_API ConfigFile : public ThreadSafe {
public:
	ConfigFile();
	virtual ~ConfigFile();

	bool load(const String &filename);
	bool save() const;
	bool saveTo(const String &filename) const;
	size_t getSectionCount() const;
	StringSeq getSections() const;
	size_t getItemCount(const String &section) const;
	StringPairSeq getItems(const String &section) const;
	String getKeyValue(const String &section, const String &key) const;
	void setValue(const String &section, const String &key, const String &value);

private:
	void clear();
	
private:
	typedef std::map<String, String>	ItemType;
	typedef std::map<String, ItemType > SectionType;
	SectionType m_data;
	String m_filename;
};


AX_END_NAMESPACE

#endif // AX_CONFIGFILE_H
