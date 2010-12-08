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

	bool load(const std::string &filename);
	bool save() const;
	bool saveTo(const std::string &filename) const;
	size_t getSectionCount() const;
	StringSeq getSections() const;
	size_t getItemCount(const std::string &section) const;
	StringPairSeq getItems(const std::string &section) const;
	std::string getKeyValue(const std::string &section, const std::string &key) const;
	void setValue(const std::string &section, const std::string &key, const std::string &value);

private:
	void clear();
	
private:
	typedef std::map<std::string, std::string>	ItemType;
	typedef std::map<std::string, ItemType > SectionType;
	SectionType m_data;
	std::string m_filename;
};


AX_END_NAMESPACE

#endif // AX_CONFIGFILE_H
