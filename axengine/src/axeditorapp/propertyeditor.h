/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QTreeView>

#include "private.h"
#include "propertyeditor_items.h"

class PropertyEditorModel;
class PropertyEditorDelegate;

class PropertyEditor: public QTreeView
{
    Q_OBJECT
public:
    PropertyEditor(QWidget *parent = 0);
    ~PropertyEditor();

	void initFromObject(Object* obj);
	bool initScriptProp(Object* obj);
	void applyToObject(Object* obj);
    IProperty* initialInput() const;
    bool isReadOnly() const;

    PropertyEditorModel* editorModel() const { return m_model; }
	void setData(const QString& propName,QVariant value);

signals:
    void propertyChanged(IProperty *property);
    void editorOpened();
    void editorClosed();

public slots:
    void setInitialInput(IProperty *initialInput);
    void setReadOnly(bool readOnly);
	void setProperHeight();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent (QDropEvent * event);

    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;
    virtual void keyPressEvent(QKeyEvent *ev);
    virtual QStyleOptionViewItem viewOptions() const;
    virtual void focusInEvent(QFocusEvent *event);

private slots:
    void headerDoubleClicked(int);

private:
    PropertyEditorModel* m_model;
    PropertyEditorDelegate* m_itemDelegate;
};

#endif // PROPERTYEDITOR_H
