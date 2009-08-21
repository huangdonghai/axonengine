/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef PROPERTYEDITOR_DELEGATE_H
#define PROPERTYEDITOR_DELEGATE_H

#include <QItemDelegate>

class IProperty;
class PropertyEditorModel;

class PropertyEditorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    PropertyEditorDelegate(QObject *parent = 0);
    virtual ~PropertyEditorDelegate();

    virtual bool eventFilter(QObject *object, QEvent *event);

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

//
// QItemDelegate Interface
//
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const;

    virtual QSize sizeHint(const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;

    virtual void setEditorData(QWidget *editor,
                               const QModelIndex &index) const;

    virtual void setModelData(QWidget *editor,
                              QAbstractItemModel *model,
                              const QModelIndex &index) const;

    virtual void updateEditorGeometry(QWidget *editor,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) const;

signals:
    void resetProperty(const QString &propertyName);
    void editorOpened();
    void editorClosed();
public slots:
    void sync();
    void resetProperty(const IProperty *property, PropertyEditorModel *model);
    void slotDestroyed(QObject *object);

protected:
    virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
                                const QRect &rect, const QPixmap &pixmap) const;

private:
    bool m_readOnly;
    bool m_syncing;
    mutable QObject *m_lastEdited;
};

class EditorWithReset : public QWidget
{
	Q_OBJECT
public:
	EditorWithReset(const IProperty *property, PropertyEditorModel *model, QWidget *parent = 0);
	void setChildEditor(QWidget *child_editor);
	QWidget *childEditor() const { return m_child_editor; }
	private slots:
		void emitResetProperty();
signals:
		void sync();
		void resetProperty(const IProperty *property, PropertyEditorModel *model);
private:
	QWidget *m_child_editor;
	QHBoxLayout *m_layout;
	const IProperty *m_property;
	PropertyEditorModel *m_model;
};


#endif // PROPERTYEDITOR_DELEGATE_H
