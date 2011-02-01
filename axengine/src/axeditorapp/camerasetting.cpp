/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "camerasetting.h"

#define DirIndex 0 // 视坐标前向量索引
#define LeftIndex 1 // 视坐标左向量索引
#define UpIndex 2 // 视坐标上向量索引

#define CheckIsEqual(num) \
	if (fabs(num) < 0.001f)\
	{\
		return ;\
	}

CameraSetting::CameraSetting(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

CameraSetting::~CameraSetting()
{

}

void CameraSetting::refreshDlg(const RenderCamera &camera)
{
	ui.eyeX->setValue(camera.origin().x);
	ui.eyeY->setValue(camera.origin().y);
	ui.eyeZ->setValue(camera.origin().z);

	ui.dirX->setValue(camera.viewAxis()[DirIndex].x);
	ui.dirY->setValue(camera.viewAxis()[DirIndex].y);
	ui.dirZ->setValue(camera.viewAxis()[DirIndex].z);

	ui.upX->setValue(camera.viewAxis()[UpIndex].x);
	ui.upY->setValue(camera.viewAxis()[UpIndex].y);
	ui.upZ->setValue(camera.viewAxis()[UpIndex].z);
}

void CameraSetting::on_eyeX_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.origin();

	CheckIsEqual(v.x - value);

	v.x = value;

	Matrix mat;
	mat.axis = (camara.viewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_eyeY_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.origin();

	CheckIsEqual(v.y - value);

	v.y = value;

	Matrix mat;
	mat.axis = (camara.viewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_eyeZ_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.origin();

	CheckIsEqual(v.z - value);
	v.z = value;

	Matrix mat;
	mat.axis = (camara.viewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirX_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[DirIndex].x - value);
	mat3[DirIndex].x = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirY_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[DirIndex].y - value);
	mat3[DirIndex].y = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirZ_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[DirIndex].z - value);
	mat3[DirIndex].z = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upX_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[UpIndex].x - value);
	mat3[UpIndex].x = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upY_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[UpIndex].y - value);
	mat3[UpIndex].y = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upZ_valueChanged(double value)
{
	RenderCamera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.viewAxis();

	CheckIsEqual(mat3[UpIndex].z - value);
	mat3[UpIndex].z = value;

	Matrix mat;
	mat.axis = (mat3);
	mat.origin = (camara.origin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}