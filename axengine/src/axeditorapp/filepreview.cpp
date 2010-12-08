/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/



#include "filepreview.h"
#include "renderwidget.h"

FilePreview::FilePreview(void)
{
	Init();
}

FilePreview::~FilePreview(void)
{
}

static QWidget* CreateWidgetFromQPixmap(const QPixmap &pixmap, QWidget *parent)
{
	QLabel *label = new QLabel(parent);
	label->setScaledContents(true);
	label->setPixmap(pixmap);
	return label;
}

static QWidget* CreateWidgetFromImage(const std::string &filePathName, Image::FileType type, QWidget *parent)
{
	Image image;
	if (!image.loadFileByType(filePathName, type, Image::NoCompressed | Image::ExpandAlpha))
		return NULL;

	const byte_t *bits = image.getData(0, TexFormat::BGRA8);

	QImage imageQ(const_cast<byte_t*>(bits), image.getWidth(), image.getHeight(), QImage::Format_ARGB32);
	QPixmap pixmap = QPixmap::fromImage(imageQ);

	return CreateWidgetFromQPixmap(pixmap, parent);
}

static QWidget* CreateWidgetFromQImageFile(const std::string &filePathName, QWidget *parent)
{
	QPixmap pixmap;
	std::string gamePath = g_fileSystem->dataPathToOsPath(filePathName);
	if (!pixmap.load(u2q(gamePath))) {
		return NULL;
	}

	return CreateWidgetFromQPixmap(pixmap, parent);
}

static QWidget* CreateWidgetFrom3DFile(const std::string &filePathName, QWidget *parent)
{
	PreviewWidget *preview = new PreviewWidget(parent);
	std::string modelName = PathUtil::removeExt(filePathName);
#if 0
	Render::Model *modelPtr = gRenderSystem->createModel(modelName);
#endif
//	preview->setModel(modelPtr);
	return preview;
}

static inline QWidget *CreateWidgetFromTga(const std::string &filePathName, QWidget *parent)
{	return CreateWidgetFromImage(filePathName, Image::TGA, parent);	}
static inline QWidget *CreateWidgetFromDds(const std::string &filePathName, QWidget *parent)
{	return CreateWidgetFromImage(filePathName, Image::DDS, parent);	}
static inline QWidget *CreateWidgetFromJp2(const std::string &filePathName, QWidget *parent)
{	return CreateWidgetFromImage(filePathName, Image::JP2, parent);	}
#if 0
static inline QWidget *CreateWidgetFromPng(const String &filePathName, QWidget *parent)
{	return CreateWidgetFromCxImage(filePathName, Image::PNG, parent);	}
#endif
static inline QWidget *CreateWidgetFromHdr(const std::string &filePathName, QWidget *parent)
{	return CreateWidgetFromImage(filePathName, Image::HDR, parent);	}
//static inline QWidget *CreateWidgetFromJpg(const String &filePathName, QWidget *parent)
//{	return CreateWidgetFromCxImage(filePathName, Image::JPEG, parent);	}


void FilePreview::Init()
{
	AX_ASSERT(m_createWidgetDict.size() == 0);
	m_createWidgetDict["TGA"] = &CreateWidgetFromTga;
	m_createWidgetDict["JP2"] = &CreateWidgetFromJp2;
	m_createWidgetDict["HDR"] = &CreateWidgetFromHdr;
	m_createWidgetDict["DDS"] = &CreateWidgetFromDds;
#if 0
	mMapCreateWidget["PNG"] = &CreateWidgetFromPng;
#endif
	//mMapCreateWidget[L"MA"] = &CreateWidgetFrom3DFile;
	//mMapCreateWidget[L"MB"] = &CreateWidgetFrom3DFile;
	m_createWidgetDict["MODEL"] = &CreateWidgetFrom3DFile;

	m_createWidgetDict["BMP"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["GIF"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["PBM"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["PGM"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["PPM"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["XBM"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["XPM"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["JPG"] = &CreateWidgetFromQImageFile;
	m_createWidgetDict["JPEG"] = &CreateWidgetFromQImageFile;
}

QWidget* FilePreview::CreateWidget(const std::string &filePathName, QWidget *parent)
{
	std::string ext = PathUtil::getExt(filePathName);
	if (ext[0] == L'.')
		ext = ext.c_str() + 1;
	strupr(const_cast<char*>(ext.c_str()));

	if (m_createWidgetDict.find(ext) == m_createWidgetDict.end()) {
		return NULL;
	}
	
	return m_createWidgetDict[ext](filePathName, parent);
}

