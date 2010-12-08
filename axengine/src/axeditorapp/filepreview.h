/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef __FILE_PREVIEW_H__
#define __FILE_PREVIEW_H__

#include "private.h"

class FilePreview
{
	typedef QWidget* (*CreateWidgetFunc)(const std::string &filePathName, QWidget *parent);
	Dict<std::string, CreateWidgetFunc>	m_createWidgetDict;

	void Init();
public:
	FilePreview(void);
	~FilePreview(void);

	QWidget *CreateWidget(const std::string &filePathName, QWidget *parent);
};

#endif // __FILE_PREVIEW_H__
