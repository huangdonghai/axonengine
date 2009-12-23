/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#include "selectbynamedlg.h"
#include "private.h"

//#define NumObjecType 4

typedef Context::ActorDict ActorDict;

enum ObjectType{ NONE,
				 STATIC = MapActor::kStatic, 
				 SPEEDTREE = MapActor::kSpeedTree,
				 BRUSH = MapActor::kBrush,
				 ENTITY = MapActor::kEntity,
				 NumObjecType
				};


bool typeCheck[NumObjecType];

SelectByNameDlg::SelectByNameDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	ui.static_2->setChecked(true);
	ui.entity->setChecked(true);
	ui.speedTree->setChecked(true);
	ui.brush->setChecked(true);

	//ui.selection->setHeaderLabel(tr("Object name List"));
	ui.selection->sortItems(0, Qt::AscendingOrder);			// 顺序排序
	
	for (int i=0; i<NumObjecType; ++i)
	{
		typeCheck[i] = true;
	}

	m_refreshSelected = true;

	ui.input->setFocus(Qt::ActiveWindowFocusReason);
}

SelectByNameDlg::~SelectByNameDlg()
{

}

void SelectByNameDlg::onDialogActived()
{
	ui.input->clear();
	ui.selection->clearSelection();

	refreshDlg();

	ui.input->setFocus(Qt::ActiveWindowFocusReason);
}

void SelectByNameDlg::refreshDlg()
{	
	// 获取已选中的物体名字
	QList<QTreeWidgetItem*> itemList = ui.selection->selectedItems();
	QList<QString> selectedNames;

	QList<QTreeWidgetItem*>::iterator it;
	for (it=itemList.begin(); it!=itemList.end(); ++it)
	{
		selectedNames.push_back((*it)->text(0));
	}

	ui.selection->clear();

	const ActorDict& actorDict = g_mapContext->getActorDict();

	QTreeWidgetItem* item;
	ActorDict::const_iterator itr;

	for (itr=actorDict.begin(); itr!=actorDict.end(); ++itr)
	{
		MapActor* actor = static_cast<MapActor*>(itr->second);

		if (actor == NULL || actor->isDeleted())
		{
			continue ;
		}

		int type = (actor)->getType();

		if (typeCheck[type] == true)
		{
			String str = actor->getGameNode()->get_objectName();

			QTreeWidgetItem* item = new QTreeWidgetItem(ui.selection);

			item->setText(0, u2q(str));
			item->setData(0, Qt::UserRole, QVariant(actor->getId()));
			item->setData(0, Qt::ToolTipRole, QVariant(MapActor::typeToString(actor->getType())));
		}
	}

	if (itemList.empty() == false)
	{
		disableRefreshSelection();

		for (int i=0; i<ui.selection->topLevelItemCount(); ++i)
		{
			QTreeWidgetItem* item = ui.selection->topLevelItem(i);

			QList<QString>::iterator it;
			for (it=selectedNames.begin(); it!=selectedNames.end(); ++it)
			{
				if (*it == item->text(0))
				{
					item->setSelected(true);
					selectedNames.erase(it);

					if (itemList.empty())
					{
						enableRefreshSelection();

						return ;
					}
					
					break;
				}
			}
		}
	}

	enableRefreshSelection();
}

void SelectByNameDlg::enableRefreshSelection()
{
	m_refreshSelected = true;
	on_selection_itemSelectionChanged();
	ui.selection->setFocus(Qt::NoFocusReason);
}

void SelectByNameDlg::disableRefreshSelection()
{
	m_refreshSelected = false;
}

void SelectByNameDlg::on_input_textEdited(QString text)
{
	if (text.isEmpty())
	{
		g_mapContext->selectNone();
		ui.selection->clearSelection();

		return ;
	}

	disableRefreshSelection();
	ui.selection->clearSelection();

	int numSelected = 0;
	int firstIndexSelected = 0;

	for (int i=0; i<ui.selection->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem* item = ui.selection->topLevelItem(i);

		QString str = item->text(0).left(text.length());

		if (ui.matchCase->isChecked())
		{
			if (str.compare(text, Qt::CaseSensitive) == 0)
			{
				item->setSelected(true);

				++numSelected;
				
				if (numSelected == 1)
				{
					firstIndexSelected = i;
					//ui.selection->scrollTo(ui.selection->model()->index(i, 0));
				}
			}
		}
		else
		{
			if (str.compare(text, Qt::CaseInsensitive) == 0)
			{
				item->setSelected(true);

				++numSelected;
				if (numSelected == 1)
				{
					firstIndexSelected = i;
					//ui.selection->scrollTo(ui.selection->model()->index(i, 0));
				}
			}
		}
	}
	
	if (numSelected > 0)
	{
		ui.selection->scrollTo(ui.selection->model()->index(firstIndexSelected + numSelected - 1, 0));

		ui.selection->scrollTo(ui.selection->model()->index(firstIndexSelected, 0));
	}

	enableRefreshSelection();
	ui.input->setFocus(Qt::ActiveWindowFocusReason);
}

void SelectByNameDlg::on_selection_itemSelectionChanged()
{
	QList<QTreeWidgetItem*> itemList = ui.selection->selectedItems();

	int numSelected = ui.selection->selectedItems().size();
	int totalNum = ui.selection->topLevelItemCount();

	char ch[50];
	QString text(tr("Total Objects:"));

	_itoa(totalNum, ch, 10);
	text += ch;

	text += tr(" Selected Objects:");
	_itoa(numSelected, ch, 10);
	text += ch;

	ui.objInfo->setText(text);

	if (m_refreshSelected == false || ui.immediate->isChecked() == false)
	{
		return ;
	}

	g_mapContext->selectNone();

	AgentList actorList;
	//const ActorDict& actorDict = gEditorContext->getActorDict();

	QTreeWidgetItem* item;
	ActorDict::const_iterator itr;

	//for (itr=actorDict.begin(); itr!=actorDict.end(); ++itr)
	//{
	//	QString str = u2q(itr->second->getGameNode()->get_objectName());

	//	for (int i=0; i<itemList.size(); ++i)
	//	{
	//		if (itemList[i]->text(0) == str)
	//		{
	//			//gEditorContext->setSelection(itr->second);
	//			actorList.push_back(itr->second);

	//			break;
	//		}
	//	}
	//}
	
	//QString str = u2q(itr->second->getGameNode()->get_objectName());

	for (int i=0; i<itemList.size(); ++i)
	{
		int id = itemList[i]->data(0, Qt::UserRole).toInt();
		actorList.push_back(g_mapContext->findActor(id));
	}

	if (actorList.empty() == false)
	{
		g_mapContext->setSelection(actorList);

		resetCamera();
	}
}

void SelectByNameDlg::on_matchCase_clicked(bool state)
{
	
}

void SelectByNameDlg::on_static_2_clicked(bool state)
{
	typeCheck[STATIC] = state;

	refreshDlg();
}

void SelectByNameDlg::on_entity_clicked(bool state)
{
	typeCheck[ENTITY] = state;

	refreshDlg();
}

void SelectByNameDlg::on_speedTree_clicked(bool state)
{
	typeCheck[SPEEDTREE] = state;

	refreshDlg();
}

void SelectByNameDlg::on_brush_clicked(bool state)
{
	typeCheck[BRUSH] = state;

	refreshDlg();
}

void SelectByNameDlg::on_selectAll_released()
{
	disableRefreshSelection();

	ui.selection->selectAll();
	ui.selection->setFocus(Qt::ActiveWindowFocusReason);

	enableRefreshSelection();
}

void SelectByNameDlg::on_selectNone_released()
{
	disableRefreshSelection();

	ui.selection->clearSelection();
	ui.selection->setFocus(Qt::ActiveWindowFocusReason);

	enableRefreshSelection();
}

void SelectByNameDlg::on_selectInvert_released()
{
	disableRefreshSelection();

	for (int i=0; i<ui.selection->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem* item = ui.selection->topLevelItem(i);

		item->setSelected(!(item->isSelected()));
	}

	ui.selection->setFocus(Qt::ActiveWindowFocusReason);

	enableRefreshSelection();
}

void SelectByNameDlg::on_typeAll_released()
{
	ui.static_2->setChecked(true);
	ui.entity->setChecked(true);
	ui.speedTree->setChecked(true);
	ui.brush->setChecked(true);

	for (int i=0; i<NumObjecType; ++i)
	{
		typeCheck[i] = true;
	}

	refreshDlg();
}

void SelectByNameDlg::on_typeNone_released()
{
	ui.static_2->setChecked(false);
	ui.entity->setChecked(false);
	ui.speedTree->setChecked(false);
	ui.brush->setChecked(false);

	for (int i=0; i<NumObjecType; ++i)
	{
		typeCheck[i] = false;
	}

	refreshDlg();
}

void SelectByNameDlg::on_typeInvert_released()
{
	ui.static_2->setChecked(!ui.static_2->isChecked());
	ui.entity->setChecked(!ui.entity->isChecked());
	ui.speedTree->setChecked(!ui.speedTree->isChecked());
	ui.brush->setChecked(!ui.brush->isChecked());

	for (int i=0; i<NumObjecType; ++i)
	{
		typeCheck[i] = !typeCheck[i];
	}

	refreshDlg();
}

void SelectByNameDlg::on_checkBox_stateChanged(int state)
{
	/*if (state == Qt::Unchecked)
	{

	}
	else if (state == Qt::Checked)
	{

	}*/
}

void SelectByNameDlg::on_select_released()
{
	/*
	bool checked = ui.immediate->isChecked();
	ui.immediate->setChecked(true);

	on_selection_itemSelectionChanged();

	ui.immediate->setChecked(checked);
	*/

	m_refreshSelected = true;
	on_selection_itemSelectionChanged();
	resetCamera();

	this->close();
}

void SelectByNameDlg::on_cancel_released()
{
	this->close();
}

void SelectByNameDlg::on_immediate_stateChanged(int state)
{
	if (state == Qt::Unchecked)
	{
		
	}
	else if (state == Qt::Checked)
	{
		on_selection_itemSelectionChanged();
	}
}

void SelectByNameDlg::on_cameraFollows_stateChanged(int state)
{
	if (state == Qt::Unchecked)
	{

	}
	else if (state == Qt::Checked)
	{
		resetCamera();	
	}
}

void SelectByNameDlg::resetCamera()
{
	if (ui.cameraFollows->isChecked() == false)
	{
		return ;
	}

	QList<QTreeWidgetItem*> itemList = ui.selection->selectedItems();

	if (itemList.isEmpty() == false)
	{
		int id = itemList[0]->data(0, Qt::UserRole).toInt();

		Agent* actor = g_mapContext->findActor(id);

		if (actor != NULL)
		{
			AffineMat mat;

			mat.axis = (g_mapContext->getActiveView()->getCamera().getViewAxis());
			mat.origin = (actor->getOrigin() - mat.axis[0] * 10);

			g_mapContext->getActiveView()->setEyeMatrix(mat);
		}
	}
}