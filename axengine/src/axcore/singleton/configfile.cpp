/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "../private.h"

namespace Axon {

	ConfigFile::ConfigFile() {
	}

	ConfigFile::~ConfigFile() {
		SCOPE_LOCK;
		m_data.clear();
	}


	bool ConfigFile::load(const String& filename) {
		SCOPE_LOCK;
	//	SCOPE_CONVERT;

	#if 0 // don't clear, so we can load more than one file
		Clear();	// clear first
	#endif

		m_filename = filename;

		char* buffer;

		size_t size = g_fileSystem->readFile(filename, (void**)&buffer);

		if (!size || !buffer)
			return false;

		TiXmlDocument doc;

		doc.Parse(buffer, NULL, TIXML_ENCODING_UTF8);
		g_fileSystem->freeFile(buffer);

		if (doc.Error()) {
			Errorf(_("ConfigFile::load: error parse %s in line %d - %s")
				, filename.c_str()
				, doc.ErrorRow()
				, doc.ErrorDesc());
			return false;
		}

		TiXmlNode* root = doc.FirstChild("ConfigRoot");

		// no root
		if (!root)
			goto error_exit;

		TiXmlElement* section;
		TiXmlAttribute* attr;
		for (section = root->FirstChildElement("Section"); section; section = section->NextSiblingElement("Section")) {
			// no more section
			if (!section)
				break;

			// parse section name
			const char* section_name = section->Attribute("name");

			// if no "name" attribute be found, skip this section
			if (!section_name) {
				Errorf(_("ConfigFile::load: error parse '%s', leak section name"), filename.c_str());
				continue;
			}

			std::map<String, String>& item_hash = m_data[ section_name ];

			TiXmlElement* item;
			for (item = section->FirstChildElement("Item"); item; item = item->NextSiblingElement("Item")) {
				// no more item
				if (!item)
					break;

				const char *item_name = 0, *item_value = 0, *item_comment = 0;

				// parse item
				for (attr = item->FirstAttribute(); attr; attr=attr->Next()) {
					if (!strcmp(attr->Name(), "name")) {
						item_name = attr->Value();
					} else if (!strcmp(attr->Name(), "value")) {
						item_value = attr->Value();
					} else if (!strcmp(attr->Name(), "comment")) {
						item_comment = attr->Value();
					}
				}

				if (!item_name || !item_value) {
					Errorf(_("ConfigFile::load: error parse '%s', leak item name"), filename.c_str());
					continue;
				}

				// add to hash
				item_hash[ item_name ] = item_value;
			}
			// add to hash
	//		mData[ U2W(section_name) ] = item_hash;
		}

		// free XML document
		doc.Clear();
		return true;

	error_exit:
		doc.Clear();
		return false;
	}

	bool
	ConfigFile::save() const {
		SCOPE_LOCK;

		if (m_filename.empty())
			return false;

		return saveTo(m_filename);
	}

	bool
	ConfigFile::saveTo(const String& filename) const {
		SCOPE_LOCK;
	//	SCOPE_CONVERT;

		File* file = g_fileSystem->openFileWrite(filename);

		if (!file) {
			Debugf(_("ConfigFile::SaveTo: cann't open file %s to write\n"), filename.c_str());
			return false;
		}

		file->printf("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
		file->printf("<ConfigRoot>\n");

		SectionType::const_iterator sit = m_data.begin();

		for (; sit != m_data.end(); ++sit) {
			file->printf("\t<Section name=\"%s\">\n", sit->first.c_str());
			const ItemType& items = sit->second;
			ItemType::const_iterator iit = items.begin();

			for (; iit != items.end(); ++iit) {
				file->printf("\t\t<Item name=\"%s\" value=\"%s\"/>\n", iit->first.c_str(), iit->second.c_str());
			}

			file->printf("\t</Section>\n", sit->first.c_str());
		}

		file->printf("</ConfigRoot>\n");

		file->close();
		delete file;

		return true;
	}

	size_t
	ConfigFile::getSectionCount() const {
		SCOPE_LOCK;

		return m_data.size();
	}

	StringSeq
	ConfigFile::getSections() const {
		SCOPE_LOCK;

		StringSeq sv;

		SectionType::const_iterator it;

		for (it = m_data.begin(); it != m_data.end(); ++it) {
			sv.push_back(it->first);
		}

		return sv;
	}

	size_t
	ConfigFile::getItemCount(const String& section) const {
		SCOPE_LOCK;

		SectionType::const_iterator section_it;

		section_it = m_data.find(section);

		if (section_it != m_data.end()) {
			return section_it->second.size();
		} else {
			return 0;
		}
	}

	StringPairSeq
	ConfigFile::getItems(const String& section) const {
		SCOPE_LOCK;

		StringPairSeq spv;

		SectionType::const_iterator section_it;
		ItemType::const_iterator item_it;

		section_it = m_data.find(section);

		if (section_it != m_data.end()) {
			const ItemType& items = section_it->second;

			for (item_it = items.begin(); item_it != items.end(); ++item_it) {
				spv.push_back(*item_it);
			}
		}

		return spv;
	}

	String
	ConfigFile::getKeyValue(const String& section, const String& key) const {
		SCOPE_LOCK;

		SectionType::const_iterator section_it;
		ItemType::const_iterator item_it;
		String value;

		section_it = m_data.find(section);

		if (section_it != m_data.end()) {
			const ItemType& items = section_it->second;

			item_it = items.find(key);

			if (item_it != items.end())
				value = item_it->second;
		}

		return value;
	}

	void
	ConfigFile::setValue(const String& section, const String& key, const String& value) {
		SCOPE_LOCK;

		m_data[section][key] = value;
	}

	void
	ConfigFile::clear() {
		SCOPE_LOCK;

		m_data.clear();
	}

} // namespace Axon
