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

void CameraSetting::refreshDlg(const Render::Camera &camera)
{
	ui.eyeX->setValue(camera.getOrigin().x);
	ui.eyeY->setValue(camera.getOrigin().y);
	ui.eyeZ->setValue(camera.getOrigin().z);

	ui.dirX->setValue(camera.getViewAxis()[DirIndex].x);
	ui.dirY->setValue(camera.getViewAxis()[DirIndex].y);
	ui.dirZ->setValue(camera.getViewAxis()[DirIndex].z);

	ui.upX->setValue(camera.getViewAxis()[UpIndex].x);
	ui.upY->setValue(camera.getViewAxis()[UpIndex].y);
	ui.upZ->setValue(camera.getViewAxis()[UpIndex].z);
}

void CameraSetting::on_eyeX_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.getOrigin();

	CheckIsEqual(v.x - value);

	v.x = value;

	AffineMat mat;
	mat.axis = (camara.getViewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_eyeY_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.getOrigin();

	CheckIsEqual(v.y - value);

	v.y = value;

	AffineMat mat;
	mat.axis = (camara.getViewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_eyeZ_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Vector3 v = camara.getOrigin();

	CheckIsEqual(v.z - value);
	v.z = value;

	AffineMat mat;
	mat.axis = (camara.getViewAxis());
	mat.origin = (v);

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirX_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[DirIndex].x - value);
	mat3[DirIndex].x = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirY_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[DirIndex].y - value);
	mat3[DirIndex].y = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_dirZ_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[DirIndex].z - value);
	mat3[DirIndex].z = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upX_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[UpIndex].x - value);
	mat3[UpIndex].x = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upY_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[UpIndex].y - value);
	mat3[UpIndex].y = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}

void CameraSetting::on_upZ_valueChanged(double value)
{
	Render::Camera camara = g_mapContext->getActiveView()->getCamera();
	Matrix3 mat3 = camara.getViewAxis();

	CheckIsEqual(mat3[UpIndex].z - value);
	mat3[UpIndex].z = value;

	AffineMat mat;
	mat.axis = (mat3);
	mat.origin = (camara.getOrigin());

	g_mapContext->getActiveView()->setEyeMatrix(mat);
}