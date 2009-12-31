/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/

#ifndef PROPERTYEDITOR_ITEMS_H
#define PROPERTYEDITOR_ITEMS_H

#include "private.h"

class IProperty {
	IProperty(const IProperty&);
	IProperty &operator=(const IProperty&);
public:
	enum Kind {
		kNormal,
		kGroup
	};

	inline IProperty()
		: m_parent(0), m_changed(0), m_dirty(0), m_fake(0), m_reset(0) {}

	virtual ~IProperty() {}

	// ### pure
	bool changed() const { return m_changed; }
	void setChanged(bool b);

	bool dirty() const { return m_dirty; }
	void setDirty(bool b);

	bool hasReset() const { return m_reset; }
	void setHasReset(bool b) { m_reset = b; }

	bool isFake() const { return m_fake; }
	void setFake(bool b) { m_fake = b; }

	virtual Kind kind() const = 0;
	virtual bool isSeparator() const { return false; }
	virtual IProperty *parent() const { return m_parent; }
	virtual void setParent(IProperty *parent) { m_parent = parent; }
	virtual QString propertyName() const = 0;
	virtual QVariant value() const = 0;
	virtual void setValue(const QVariant &value) = 0;
	virtual QString toString() const = 0;
	virtual QVariant decoration() const = 0;
	virtual bool hasEditor() const = 0;
	virtual QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const = 0;
	virtual void updateEditorContents(QWidget *editor) { Q_UNUSED(editor); }
	virtual void updateValue(QWidget *editor) { Q_UNUSED(editor); }
	virtual bool hasExternalEditor() const = 0;
	virtual QWidget *createExternalEditor(QWidget *parent) = 0;

protected:
	IProperty *m_parent;
	uint m_changed : 1;
	uint m_dirty : 1;
	uint m_fake : 1;
	uint m_reset : 1;
};

class IPropertyGroup: public IProperty {
public:
	virtual int indexOf(IProperty *property) const = 0;
	virtual int propertyCount() const = 0;
	virtual IProperty *propertyAt(int index) const = 0;
};

template <typename T>
class AbstractProperty: public IProperty
{
public:
	AbstractProperty(const T &value, const QString &name)
		: m_value(value), m_name(name) {}

	IProperty::Kind  kind() const { return IProperty::kNormal; }

	//
	// IProperty Interface
	//
	QVariant decoration() const { return QVariant(); }
	QString propertyName() const { return m_name; }
	QVariant value() const { return qVariantFromValue(m_value); }

	bool hasEditor() const { return true; }
	bool hasExternalEditor() const { return false; }
	QWidget *createExternalEditor(QWidget *parent) { Q_UNUSED(parent); return 0; }

protected:
	T m_value;
	const QString m_name;
};

class AbstractPropertyGroup: public IPropertyGroup {
public:
	AbstractPropertyGroup(const QString &name)
		: m_name(name) {}

	~AbstractPropertyGroup()
	{ qDeleteAll(m_properties); }

	IProperty::Kind kind() const { return kGroup; }

	//
	// IPropertyGroup Interface
	//
	int indexOf(IProperty *property) const { return m_properties.indexOf(property); }
	int propertyCount() const { return m_properties.size(); }
	IProperty *propertyAt(int index) const { return m_properties.at(index); }

	//
	// IProperty Interface
	//

	inline QString propertyName() const { return m_name; }
	inline QVariant decoration() const { return QVariant(); }

	QString toString() const;

	inline bool hasEditor() const { return true; }

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	virtual void updateEditorContents(QWidget *editor);

	inline bool hasExternalEditor() const { return false; }

	QWidget *createExternalEditor(QWidget *parent) { Q_UNUSED(parent); return 0; }

protected:
	const QString m_name;
	QList<IProperty*> m_properties;
};

class PropertyCollection: public IPropertyGroup
{
public:
	PropertyCollection(const QString &name);
	~PropertyCollection();

	inline IProperty::Kind kind() const { return kGroup; }

	void addProperty(IProperty *property);
	void removeProperty(IProperty *property);

	//
	// IPropertyGroup Interface
	//
	int indexOf(IProperty *property) const;
	int propertyCount() const;
	IProperty *propertyAt(int index) const;

	//
	// IProperty Interface
	//
	QString propertyName() const;

	QVariant value() const;
	void setValue(const QVariant &value);

	QVariant decoration() const { return QVariant(); }
	QString toString() const;

	bool hasEditor() const;
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;

	bool hasExternalEditor() const;
	QWidget *createExternalEditor(QWidget *parent);

private:
	const QString m_name;
	QList<IProperty*> m_properties;
};

class IntProperty: public AbstractProperty<int>
{
public:
	IntProperty(int value, const QString &name);

	QString specialValue() const;
	void setSpecialValue(const QString &specialValue);

	void setRange(int low, int hi);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	QString m_specialValue;
	int m_low;
	int m_hi;
};

class BoolProperty: public AbstractProperty<bool>
{
public:
	BoolProperty(bool value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QVariant decoration() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class LongLongProperty: public AbstractProperty<qlonglong>
{
public:
	LongLongProperty(qlonglong value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class DoubleProperty: public AbstractProperty<double>
{
public:
	DoubleProperty(double value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class SpinBoxDoubleProperty: public AbstractProperty<double>
{
public:
	SpinBoxDoubleProperty(double value, const QString &name);

	QString specialValue() const;
	void setSpecialValue(const QString &specialValue);

	void setRange(double low, double hi);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	QString m_specialValue;
	double m_low;
	double m_hi;
};

#if 0
class CharProperty: public AbstractProperty<QChar>
{
public:
	CharProperty(QChar value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};
#endif

class StringProperty: public AbstractPropertyGroup
{
public:
	StringProperty(const QString &value, const QString &name, bool hasComment = false, const QString &comment = QString());


	QVariant value() const;
	void setValue(const QVariant &value);
	QString toString() const;

	bool hasEditor() const;
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

protected:
	QString m_value;
};

class SeparatorProperty: public StringProperty
{
public:
	SeparatorProperty(const QString &value, const QString &name);

	bool isSeparator() const { return true; }
	bool hasEditor() const;
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class FileLineEdit : public QWidget {
	Q_OBJECT
public:
	FileLineEdit(QWidget*parent, const QString &value, const QString &path, const QString &filter);

	void setFilename(const QString &text);
	QString filename() const;

Q_SIGNALS:
	void fileSelected(const QString &);

private slots:
	void on_toolButton_clicked();

private:
	QHBoxLayout *horizontalLayout;
	QLineEdit *lineEdit;
	QToolButton *toolButton;
	QString m_path;
	QString m_filter;
};

class FileProperty : public StringProperty {
public:
	FileProperty(const QString &value, const QString &name, const QString &path, const QString &filter);

	bool hasEditor() const;
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	QString m_path;
	QString m_filter;
};

class EnumProperty: public AbstractProperty<int>
{
public:
	EnumProperty(const Member::EnumItems &items, int value, const QString &name);

	QStringList items() const;

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	typedef QPair<QString,int> EnumItem;
	typedef QVector<EnumItem> EnumItems;
	EnumItems m_items;
};

class FlagsProperty: public AbstractProperty<int>
{
public:
	FlagsProperty(const Member::EnumItems &items, int value, const QString &name);

	QStringList items() const;

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	typedef QPair<QString,int> EnumItem;
	typedef QVector<EnumItem> EnumItems;
	EnumItems m_items;
};


class ColorProperty: public AbstractPropertyGroup
{
public:
	ColorProperty(const QColor &value, const QString &name);

	QVariant value() const;
	void setValue(const QVariant &value);
	QVariant decoration() const;

	QString toString() const { return QLatin1String(" ") + AbstractPropertyGroup::toString(); } // ### temp hack remove me!!
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class VectorProperty : public AbstractPropertyGroup {
public:
	VectorProperty(const Vector3 &value, const QString &name);

	QVariant value() const;
	void setValue(const QVariant &value);
#if 0
	QVariant decoration() const;

	QString toString() const { return QLatin1String(" ") + AbstractPropertyGroup::toString(); } // ### temp hack remove me!!
	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
#endif
};

class PointProperty : public AbstractPropertyGroup {
public:
	PointProperty(const Point &value, const QString &name);

	QVariant value() const;
	void setValue(const QVariant &value);
};

class RectProperty : public AbstractPropertyGroup {
public:
	RectProperty(const Rect &value, const QString &name);

	QVariant value() const;
	void setValue(const QVariant &value);
};


#if 0
class DateTimeProperty: public AbstractProperty<QDateTime>
{
public:
	DateTimeProperty(const QDateTime &value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class DateProperty: public AbstractProperty<QDate>
{
public:
	DateProperty(const QDate &value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class TimeProperty: public AbstractProperty<QTime>
{
public:
	TimeProperty(const QTime &value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};

class UrlProperty: public AbstractPropertyGroup
{
public:
	UrlProperty(const QUrl &value, const QString &name);

	QVariant value() const;
	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);

private:
	QUrl m_value;
};

class StringListProperty: public AbstractProperty<QStringList>
{
public:
	StringListProperty(const QStringList &value, const QString &name);

	void setValue(const QVariant &value);
	QString toString() const;

	QWidget *createEditor(QWidget *parent, const QObject *target, const char *receiver) const;
	void updateEditorContents(QWidget *editor);
	void updateValue(QWidget *editor);
};
#endif

#endif // PROPERTYEDITOR_ITEMS_H
