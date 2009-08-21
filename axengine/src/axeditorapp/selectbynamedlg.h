/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef SELECTBYNAMEDLG_H
#define SELECTBYNAMEDLG_H

#include <QDialog>
#include "ui_selectbynamedlg.h"

class SelectByNameDlg : public QDialog
{
	Q_OBJECT

public:
	SelectByNameDlg(QWidget *parent = 0);
	~SelectByNameDlg();

	void refreshDlg();
	void enableRefreshSelection();
	void disableRefreshSelection();
	void onDialogActived();
	void resetCamera();

private:
	Ui::SelectByNameDlgClass ui;

	bool m_refreshSelected;

private slots:
	void on_cameraFollows_stateChanged(int);
	void on_immediate_stateChanged(int);
	void on_cancel_released();
	void on_select_released();
	void on_checkBox_stateChanged(int);
	void on_typeInvert_released();
	void on_typeNone_released();
	void on_typeAll_released();
	void on_selectInvert_released();
	void on_selectNone_released();
	void on_selectAll_released();
	//void on_cancel_clicked(bool);
	//void on_select_clicked(bool);
	//void on_typeInvert_clicked(bool);
	//void on_typeNone_clicked(bool);
	//void on_typeAll_clicked(bool);
	void on_brush_clicked(bool);
	void on_speedTree_clicked(bool);
	void on_entity_clicked(bool);
	void on_static_2_clicked(bool);
	//void on_selectInvert_clicked(bool);
	//void on_selectNone_clicked(bool);
	//void on_selectAll_clicked(bool);
	void on_matchCase_clicked(bool);
	void on_selection_itemSelectionChanged();
	void on_input_textEdited(QString);
};

#endif // SELECTBYNAMEDLG_H
