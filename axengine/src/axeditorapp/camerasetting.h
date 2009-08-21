/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef CAMERASETTING_H
#define CAMERASETTING_H

#include "private.h"
#include <QDialog>
#include "ui_camerasetting.h"

class CameraSetting : public QDialog
{
	Q_OBJECT

public:
	CameraSetting(QWidget *parent = 0);
	~CameraSetting();

	void refreshDlg(const Axon::Render::Camera &camera);

private:
	Ui::CameraSettingClass ui;

private slots:
	void on_upZ_valueChanged(double);
	void on_upY_valueChanged(double);
	void on_upX_valueChanged(double);
	void on_dirZ_valueChanged(double);
	void on_dirY_valueChanged(double);
	void on_dirX_valueChanged(double);
	void on_eyeZ_valueChanged(double);
	void on_eyeY_valueChanged(double);
	void on_eyeX_valueChanged(double);
};

#endif // CAMERASETTING_H
