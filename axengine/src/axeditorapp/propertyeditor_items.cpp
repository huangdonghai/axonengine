/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "propertyeditor_items.h"
#include "widgetlib.h"
#include "flagbox.h"

static QString matchStringInKeys(const QString &str, const QMap<QString, QVariant> &items) {
	for (QMap<QString, QVariant>::const_iterator it = items.begin(); it != items.end(); ++it) {
		if (it.key().contains(str))
			return it.key();
	}
	return str;
}


void IProperty::setDirty(bool b)
{
	if (isFake()) {
		IProperty *p = parent();
		while (p != 0 && p->isFake())
			p = p->parent();
		if (p != 0)
			p->setDirty(true);
	} else {
		m_dirty = b;
	}
}

void IProperty::setChanged(bool b)
{
	if (isFake()) {
		IProperty *p = parent();
		while (p != 0 && p->isFake())
			p = p->parent();
		if (p != 0)
			p->setChanged(true);
	} else {
		m_changed = b;
	}
	setDirty(true);
}

// -------------------------------------------------------------------------

QWidget *AbstractPropertyGroup::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	Q_UNUSED(target);
	Q_UNUSED(receiver);

	QLabel *label = new QLabel(parent);
	label->setIndent(2); // ### hardcode it should have the same value of textMargin in QItemDelegate
	label->setBackgroundRole(QPalette::Base);
	return label;
}

void AbstractPropertyGroup::updateEditorContents(QWidget *editor)
{
	QLabel *label = qobject_cast<QLabel*>(editor);
	if (label == 0)
		return;
	label->setText(toString());
}

QString AbstractPropertyGroup::toString() const
{
	QString text = QString(QLatin1Char('['));
	for (int i=0; i<propertyCount(); ++i) {
		if (i)
			text += QLatin1String(", ");
		text += propertyAt(i)->toString();
	}
	text += QLatin1Char(']');
	return text;
}

// -------------------------------------------------------------------------
// Return an icon containing a check box indicator
static QIcon drawCheckBox(bool value)
{
	QStyleOptionButton opt;
	opt.state |= value ? QStyle::State_On : QStyle::State_Off;
	opt.state |= QStyle::State_Enabled;
	const QStyle *style = QApplication::style();
	// Figure out size of an indicator and make sure it is not scaled down in a list view item
	// by making the pixmap as big as a list view icon and centering the indicator in it.
	// (if it is smaller, it can't be helped)
	const int indicatorWidth = style->pixelMetric(QStyle::PM_IndicatorWidth, &opt);
	const int indicatorHeight = style->pixelMetric(QStyle::PM_IndicatorHeight, &opt);
	const int listViewIconSize = style->pixelMetric(QStyle::PM_ListViewIconSize);
	const int pixmapWidth = qMax(indicatorWidth, listViewIconSize);
	const int pixmapHeight = qMax(indicatorHeight, listViewIconSize);

	opt.rect = QRect(0, 0, indicatorWidth, indicatorHeight);
	QPixmap pixmap = QPixmap(pixmapWidth, pixmapHeight);
	pixmap.fill(Qt::transparent);
	{
		// Center?
		const int xoff = (pixmapWidth  > indicatorWidth)  ? (pixmapWidth  - indicatorWidth)  / 2 : 0;
		const int yoff = (pixmapHeight > indicatorHeight) ? (pixmapHeight - indicatorHeight) / 2 : 0;
		QPainter painter(&pixmap);
		painter.translate(xoff, yoff);
		style->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, &painter);
	}
	return QIcon(pixmap);
}

BoolProperty::BoolProperty(bool value, const QString &name)
: AbstractProperty<bool>(value, name)
{
}

void BoolProperty::setValue(const QVariant &value)
{
	m_value = value.toBool();
}

QString BoolProperty::toString() const
{
	return m_value ? QLatin1String("true") : QLatin1String("false");
}

QWidget *BoolProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QComboBox *combo = new QComboBox(parent);
	combo->view()->setTextElideMode(Qt::ElideLeft);
	combo->setFrame(0);
	combo->addItems(QStringList() << QString::fromUtf8("false") << QString::fromUtf8("true"));
	QObject::connect(combo, SIGNAL(activated(int)), target, receiver);

	return combo;
}

void BoolProperty::updateEditorContents(QWidget *editor)
{
	if (QComboBox *combo = qobject_cast<QComboBox*>(editor)) {
		combo->setCurrentIndex(m_value ? 1 : 0);
	}
}

void BoolProperty::updateValue(QWidget *editor)
{
	if (const QComboBox *combo = qobject_cast<const QComboBox*>(editor)) {
		const bool newValue = combo->currentIndex() ? true : false;

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}

QVariant BoolProperty::decoration() const
{
	static const QIcon checkedIcon = drawCheckBox(true);
	static const QIcon uncheckedIcon = drawCheckBox(false);
	return m_value ? checkedIcon : uncheckedIcon;

}

// -------------------------------------------------------------------------
PropertyCollection::PropertyCollection(const QString &name)
: m_name(name)
{
}

PropertyCollection::~PropertyCollection()
{
	qDeleteAll(m_properties);
}

void PropertyCollection::addProperty(IProperty *property)
{
	property->setParent(this);
	m_properties.append(property);
}

void PropertyCollection::removeProperty(IProperty *property)
{
	Q_UNUSED(property);
}

int PropertyCollection::indexOf(IProperty *property) const
{
	return m_properties.indexOf(property);
}

int PropertyCollection::propertyCount() const
{
	return m_properties.size();
}

IProperty *PropertyCollection::propertyAt(int index) const
{
	return m_properties.at(index);
}

QString PropertyCollection::propertyName() const
{
	return m_name;
}

QVariant PropertyCollection::value() const
{
	return QVariant();
}

void PropertyCollection::setValue(const QVariant &value)
{
	Q_UNUSED(value);
}

QString PropertyCollection::toString() const
{
	return QString();
}

bool PropertyCollection::hasEditor() const
{
	return false;
}

QWidget *PropertyCollection::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	Q_UNUSED(parent);
	Q_UNUSED(target);
	Q_UNUSED(receiver);
	return 0;
}

bool PropertyCollection::hasExternalEditor() const
{
	return false;
}

QWidget *PropertyCollection::createExternalEditor(QWidget *parent)
{
	Q_UNUSED(parent);
	return 0;
}

// -------------------------------------------------------------------------

StringProperty::StringProperty(const QString &value, const QString &name,
							   bool hasComment, const QString &comment)
							   : AbstractPropertyGroup(name),
							   m_value(value)
{
	if (hasComment) {
		StringProperty *pcomment = new StringProperty(comment, QLatin1String("comment"));
		pcomment->setParent(this);
		m_properties << pcomment;
	}
}


QVariant StringProperty::value() const
{
	return m_value;
}

void StringProperty::setValue(const QVariant &value)
{
	m_value = value.toString();
}

QString StringProperty::toString() const
{
	return m_value;
}

bool StringProperty::hasEditor() const
{
	return true;
}

QWidget *StringProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QLineEdit* textEditor = new QLineEdit(parent);

	QObject::connect(textEditor, SIGNAL(textChanged(QString)), target, receiver);
	return textEditor;
}

void StringProperty::updateEditorContents(QWidget *editor)
{
	if (QLineEdit *textEditor = qobject_cast<QLineEdit*>(editor)) {
		if (textEditor->text() != m_value)
			textEditor->setText(m_value);
	}
}

void StringProperty::updateValue(QWidget *editor)
{
	if (const QLineEdit *textEditor = qobject_cast<const QLineEdit*>(editor)) {
		const QString newValue = textEditor->text();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}


// -------------------------------------------------------------------------

EnumProperty::EnumProperty(const Member::EnumItems& items, int value, const QString& name)
	: AbstractProperty<int>(value, name)
{
	AX_FOREACH(const Member::EnumItem& ei, items) {
		m_items.push_back(qMakePair(u2q(ei.first), ei.second));
	}
}

QStringList EnumProperty::items() const
{
	return QStringList();
}

void EnumProperty::setValue(const QVariant &value)
{
	m_value = value.toInt();
}

QString EnumProperty::toString() const
{
	AX_FOREACH(const EnumItem& item, m_items) {
		if (item.second == m_value) {
			return item.first;
		}
	}
	return QString();
}

QWidget *EnumProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QComboBox *combo = new QComboBox(parent);
	combo->view()->setTextElideMode(Qt::ElideLeft);
	combo->setFrame(0);
#if 0
	combo->addItems(items());
#else
	AX_FOREACH(const EnumItem& item, m_items) {
		combo->addItem(item.first, item.second);
	}
#endif
	QObject::connect(combo, SIGNAL(activated(int)), target, receiver);
	return combo;
}

void EnumProperty::updateEditorContents(QWidget *editor)
{
	if (QComboBox *combo = qobject_cast<QComboBox*>(editor)) {
		int index = 0;
		AX_FOREACH(const EnumItem& item, m_items) {
			if (item.second == m_value) {
				break;
			}
			index++;
		}

		combo->setCurrentIndex(index);
	}
}

void EnumProperty::updateValue(QWidget *editor)
{
	if (QComboBox *combo = qobject_cast<QComboBox*>(editor)) {
		const int newValue = combo->currentIndex();
		const int v = combo->itemData(newValue).toInt();

		if (v != m_value) {
			m_value = v;
			setChanged(true);
		}
	}
}

// -------------------------------------------------------------------------

FlagsProperty::FlagsProperty(const Member::EnumItems& items, int value, const QString& name)
	: AbstractProperty<int>(value, name)
{
	AX_FOREACH(const Member::EnumItem& ei, items) {
		m_items.push_back(qMakePair(u2q(ei.first), ei.second));
	}
}

QStringList FlagsProperty::items() const
{
	return QStringList();
}

void FlagsProperty::setValue(const QVariant &value)
{
	m_value = value.toInt();
}

QString FlagsProperty::toString() const
{
	QString result;
	bool first = true;
	AX_FOREACH(const EnumItem& item, m_items) {
		if ((item.second & m_value) == item.second) {
			if (!first) {
				result += "|";
			}
			result += item.first;
			first = false;
		}
	}
	return result;
}

QWidget *FlagsProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QList<FlagBoxModelItem> l;
	QVectorIterator<EnumItem> it(m_items);
	const int v = m_value;
	int initialIndex = -1;
	int i = 0;
	while (it.hasNext()) {
		const EnumItem& ei = it.next();
		int value = ei.second;
		bool checked = (value == 0) ? (v == 0) : ((value & v) == value);
		l.append(FlagBoxModelItem(ei.first, value, checked));
		if ((value & v) == value) {
			if (initialIndex == -1)
				initialIndex = i;
			else if (FlagBoxModel::bitcount(value) > FlagBoxModel::bitcount(l.at(initialIndex).value()))
				initialIndex = i;
		}
		++i;
	}

	FlagBox *editor = new FlagBox(parent);
	editor->setItems(l);
	editor->setCurrentIndex(initialIndex);
	QObject::connect(editor, SIGNAL(activated(int)), target, receiver);
	return editor;
}

void FlagsProperty::updateEditorContents(QWidget *editor)
{
	FlagBox *box = qobject_cast<FlagBox*>(editor);
	if (box == 0)
		return;

	box->view()->reset();
}

void FlagsProperty::updateValue(QWidget *editor)
{
	FlagBox *box = qobject_cast<FlagBox*>(editor);
	if ((box == 0) || (box->currentIndex() < 0))
		return;

	unsigned int newValue = 0;

	FlagBoxModelItem &thisItem = box->item(box->currentIndex());
	if (thisItem.value() == 0) {
		// Uncheck all items except 0-mask
		for (int i=0; i<box->count(); ++i)
			box->item(i).setChecked(i == box->currentIndex());
	} else {
		// Compute new value, without including (additional) supermasks
		if (thisItem.isChecked())
			newValue = thisItem.value();
		for (int i=0; i<box->count(); ++i) {
			FlagBoxModelItem &item = box->item(i);
			if (item.isChecked() && (FlagBoxModel::bitcount(item.value()) == 1))
				newValue |= item.value();
		}
		if (newValue == 0) {
			// Uncheck all items except 0-mask
			for (int i=0; i<box->count(); ++i) {
				FlagBoxModelItem &item = box->item(i);
				item.setChecked(item.value() == 0);
			}
		} else if (newValue == m_value) {
			if (!thisItem.isChecked() && (FlagBoxModel::bitcount(thisItem.value()) > 1)) {
				// We unchecked something, but the original value still holds
				thisItem.setChecked(true);
			}
		} else {
			// Make sure 0-mask is not selected
			for (int i=0; i<box->count(); ++i) {
				FlagBoxModelItem &item = box->item(i);
				if (item.value() == 0)
					item.setChecked(false);
			}
			// Check/uncheck proper masks
			if (thisItem.isChecked()) {
				// Make sure submasks and supermasks are selected
				for (int i=0; i<box->count(); ++i) {
					FlagBoxModelItem &item = box->item(i);
					if ((item.value() != 0) && ((item.value() & newValue) == item.value()) && !item.isChecked())
						item.setChecked(true);
				}
			} else {
				// Make sure supermasks are not selected if they're no longer valid
				for (int i=0; i<box->count(); ++i) {
					FlagBoxModelItem &item = box->item(i);
					if (item.isChecked() && ((item.value() == thisItem.value()) || ((item.value() & newValue) != item.value())))
						item.setChecked(false);
				}
			}
		}
	}

	if (newValue != m_value) {
		m_value = newValue;
		setChanged(true);
	}
}

// -------------------------------------------------------------------------
// QIntPropertySpinBox also emits editingFinished when the spinbox is used
class QIntPropertySpinBox: public QSpinBox
{
public:
	QIntPropertySpinBox(QWidget *parent = 0)
		: QSpinBox(parent) { }

	void stepBy(int steps)
	{
		QSpinBox::stepBy(steps);
		emit editingFinished();
	}
};

IntProperty::IntProperty(int value, const QString &name)
	: AbstractProperty<int>(value, name), m_low(INT_MIN), m_hi(INT_MAX)
{
}

void IntProperty::setRange(int low, int hi)
{
	m_low = low;
	m_hi = hi;
}

QString IntProperty::specialValue() const
{
	return m_specialValue;
}

void IntProperty::setSpecialValue(const QString &specialValue)
{
	m_specialValue = specialValue;
}

void IntProperty::setValue(const QVariant &value)
{
	m_value = value.toInt();
}

QString IntProperty::toString() const
{
	return QString::number(m_value);
}

QWidget *IntProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QSpinBox *spinBox = new QIntPropertySpinBox(parent);
	spinBox->setFrame(0);
	spinBox->setSpecialValueText(m_specialValue);
	spinBox->setRange(m_low, m_hi);
	spinBox->setValue(m_value);
	spinBox->selectAll();

	QObject::connect(spinBox, SIGNAL(editingFinished()), target, receiver);

	return spinBox;
}

void IntProperty::updateEditorContents(QWidget *editor)
{
	if (QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor)) {
		spinBox->setValue(m_value);
	}
}

void IntProperty::updateValue(QWidget *editor)
{
	if (QSpinBox *spinBox = qobject_cast<QSpinBox*>(editor)) {
		const int newValue = spinBox->value();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}

// -------------------------------------------------------------------------
ColorProperty::ColorProperty(const QColor &value, const QString &name)
	: AbstractPropertyGroup(name)
{
	IntProperty *r = new IntProperty(value.red(), QLatin1String("red"));
	r->setFake(true);
	r->setRange(0, 255);
	r->setParent(this);

	IntProperty *g = new IntProperty(value.green(), QLatin1String("green"));
	g->setFake(true);
	g->setRange(0, 255);
	g->setParent(this);

	IntProperty *b = new IntProperty(value.blue(), QLatin1String("blue"));
	b->setFake(true);
	b->setRange(0, 255);
	b->setParent(this);

	m_properties << r << g << b;
}

QVariant ColorProperty::value() const
{
	return qVariantFromValue(QColor(propertyAt(0)->value().toInt(),
		propertyAt(1)->value().toInt(),
		propertyAt(2)->value().toInt()));
}

void ColorProperty::setValue(const QVariant &value)
{
	QColor c = qvariant_cast<QColor>(value);
	propertyAt(0)->setValue(c.red());
	propertyAt(1)->setValue(c.green());
	propertyAt(2)->setValue(c.blue());
}

QVariant ColorProperty::decoration() const
{
	QPixmap pix(14, 14);
	pix.fill(qvariant_cast<QColor>(value()));
//	QPainter painter(&pix);
//	painter.drawRect(0,0,11,11);
	return qVariantFromValue(pix);
}

QWidget *ColorProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	ColorButton *button = new ColorButton(parent);
	QObject::connect(button, SIGNAL(colorChanged(const QColor &)), target, receiver);
	return button;
}

void ColorProperty::updateEditorContents(QWidget *editor)
{
	ColorButton *button = qobject_cast<ColorButton *>(editor);
	if (!button)
		return;
	button->setColor(qvariant_cast<QColor>(value()));
}

void ColorProperty::updateValue(QWidget *editor)
{
	ColorButton *button = qobject_cast<ColorButton *>(editor);
	if (!button)
		return;
	const QVariant color = qVariantFromValue(button->color());
	if (color != value()) {
		setValue(color);
		setChanged(true);
	}
}

// -------------------------------------------------------------------------
VectorProperty::VectorProperty(const Vector3 &value, const QString &name)
	: AbstractPropertyGroup(name)
{
	DoubleProperty *x = new DoubleProperty(value.x, QLatin1String("x"));
	x->setFake(true);
	x->setParent(this);

	DoubleProperty *y = new DoubleProperty(value.y, QLatin1String("y"));
	y->setFake(true);
	y->setParent(this);

	DoubleProperty *z = new DoubleProperty(value.x, QLatin1String("z"));
	z->setFake(true);
	z->setParent(this);

	m_properties << x << y << z;
}

QVariant VectorProperty::value() const
{
	return qVariantFromValue(Vector3(propertyAt(0)->value().toDouble(),
		propertyAt(1)->value().toDouble(),
		propertyAt(2)->value().toDouble()));
}

void VectorProperty::setValue(const QVariant &value)
{
	Vector3 c = qvariant_cast<Vector3>(value);
	propertyAt(0)->setValue(c.x);
	propertyAt(1)->setValue(c.y);
	propertyAt(2)->setValue(c.z);
}

// -------------------------------------------------------------------------
PointProperty::PointProperty(const Point &value, const QString &name)
: AbstractPropertyGroup(name)
{
	IntProperty *x = new IntProperty(value.x, QLatin1String("x"));
	x->setFake(true);
	x->setParent(this);

	IntProperty *y = new IntProperty(value.y, QLatin1String("y"));
	y->setFake(true);
	y->setParent(this);

	m_properties << x << y;
}

QVariant PointProperty::value() const
{
	return qVariantFromValue(Point(propertyAt(0)->value().toInt(),
		propertyAt(1)->value().toInt()));
}

void PointProperty::setValue(const QVariant &value)
{	
	Point c = qvariant_cast<Point>(value);
	propertyAt(0)->setValue(c.x);
	propertyAt(1)->setValue(c.y);
}

// -------------------------------------------------------------------------
RectProperty::RectProperty(const Rect &value, const QString &name)
: AbstractPropertyGroup(name)
{
	IntProperty *x = new IntProperty(value.x, QLatin1String("x"));
	x->setFake(true);
	x->setParent(this);

	IntProperty *y = new IntProperty(value.y, QLatin1String("y"));
	y->setFake(true);
	y->setParent(this);

	IntProperty *width = new IntProperty(value.width, QLatin1String("width"));
	width->setFake(true);
	width->setParent(this);

	IntProperty *height = new IntProperty(value.height, QLatin1String("height"));
	height->setFake(true);
	height->setParent(this);
	m_properties << x << y << width << height;
}

QVariant RectProperty::value() const
{
	return qVariantFromValue(Rect(propertyAt(0)->value().toInt(),
		propertyAt(1)->value().toInt()
		,propertyAt(2)->value().toInt()
		,propertyAt(3)->value().toInt()));
}

void RectProperty::setValue(const QVariant &value)
{
	Rect c = qvariant_cast<Rect>(value);
	propertyAt(0)->setValue(c.x);
	propertyAt(1)->setValue(c.y);
	propertyAt(2)->setValue(c.width);
	propertyAt(3)->setValue(c.height);
}
#if 0
QVariant VectorProperty::decoration() const
{
	QPixmap pix(12, 12);
	pix.fill(qvariant_cast<QColor>(value()));
	return qVariantFromValue(pix);
}

QWidget *VectorProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
#if 0
	ColorButton *button = new ColorButton(parent);
	QObject::connect(button, SIGNAL(colorChanged(const QColor &)), target, receiver);
	return button;
#else
	return nullptr;
#endif
}

void VectorProperty::updateEditorContents(QWidget *editor)
{
#if 0
	ColorButton *button = qobject_cast<ColorButton *>(editor);
	if (!button)
		return;
	button->setColor(qvariant_cast<QColor>(value()));
#endif
}

void VectorProperty::updateValue(QWidget *editor)
{
#if 0
	ColorButton *button = qobject_cast<ColorButton *>(editor);
	if (!button)
		return;
	const QVariant color = qVariantFromValue(button->color());
	if (color != value()) {
		setValue(color);
		setChanged(true);
	}
#endif
}
#endif

#if 0
// -------------------------------------------------------------------------
DateTimeProperty::DateTimeProperty(const QDateTime &value, const QString &name)
	: AbstractProperty<QDateTime>(value, name)
{
}

void DateTimeProperty::setValue(const QVariant &value)
{
	m_value = value.toDateTime();
}

QString DateTimeProperty::toString() const
{
	return m_value.toString();
}

QWidget *DateTimeProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QDateTimeEdit *lineEdit = new QDateTimeEdit(parent);
	QObject::connect(lineEdit, SIGNAL(dateTimeChanged(QDateTime)), target, receiver);
	return lineEdit;
}

void DateTimeProperty::updateEditorContents(QWidget *editor)
{
	if (QDateTimeEdit *lineEdit = qobject_cast<QDateTimeEdit*>(editor)) {
		lineEdit->setDateTime(m_value);
	}
}

void DateTimeProperty::updateValue(QWidget *editor)
{
	if (QDateTimeEdit *lineEdit = qobject_cast<QDateTimeEdit*>(editor)) {
		const QDateTime newValue = lineEdit->dateTime();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}

	}
}

// -------------------------------------------------------------------------
DateProperty::DateProperty(const QDate &value, const QString &name)
	: AbstractProperty<QDate>(value, name)
{
}

void DateProperty::setValue(const QVariant &value)
{
	m_value = value.toDate();
}

QString DateProperty::toString() const
{
	return m_value.toString();
}

QWidget *DateProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QDateEdit *lineEdit = new QDateEdit(parent);
	QObject::connect(lineEdit, SIGNAL(dateChanged(QDate)), target, receiver);
	return lineEdit;
}

void DateProperty::updateEditorContents(QWidget *editor)
{
	if (QDateEdit *lineEdit = qobject_cast<QDateEdit*>(editor)) {
		lineEdit->setDate(m_value);
	}
}

void DateProperty::updateValue(QWidget *editor)
{
	if (QDateEdit *lineEdit = qobject_cast<QDateEdit*>(editor)) {
		const QDate newValue = lineEdit->date();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}

	}
}

// -------------------------------------------------------------------------
TimeProperty::TimeProperty(const QTime &value, const QString &name)
	: AbstractProperty<QTime>(value, name)
{
}

void TimeProperty::setValue(const QVariant &value)
{
	m_value = value.toTime();
}

QString TimeProperty::toString() const
{
	return m_value.toString();
}

QWidget *TimeProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QTimeEdit *lineEdit = new QTimeEdit(parent);
	QObject::connect(lineEdit, SIGNAL(timeChanged(QTime)), target, receiver);
	return lineEdit;
}

void TimeProperty::updateEditorContents(QWidget *editor)
{
	if (QTimeEdit *lineEdit = qobject_cast<QTimeEdit*>(editor)) {
		lineEdit->setTime(m_value);
	}
}

void TimeProperty::updateValue(QWidget *editor)
{
	if (QTimeEdit *lineEdit = qobject_cast<QTimeEdit*>(editor)) {
		const QTime newValue = lineEdit->time();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}

	}
}
#endif


// -------------------------------------------------------------------------
DoubleProperty::DoubleProperty(double value, const QString &name)
	: AbstractProperty<double>(value, name)
{
}

void DoubleProperty::setValue(const QVariant &value)
{
	m_value = value.toDouble();
}

QString DoubleProperty::toString() const
{
#if 0
	QString result;

	result.sprintf("%.+1f", m_value);
	return result;
#endif
	QString result = QString::number(m_value);
	if (result.indexOf('.') == -1) {
		result += ".0";
	}
	return result;
}

QWidget *DoubleProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	lineEdit->setFrame(0);
	lineEdit->setValidator(new QDoubleValidator(lineEdit));

	QObject::connect(lineEdit, SIGNAL(textChanged(QString)), target, receiver);
	return lineEdit;
}

void DoubleProperty::updateEditorContents(QWidget *editor)
{
	if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor)) {
		double v = lineEdit->text().toDouble();
		if (v != m_value)
			lineEdit->setText(toString());
	}
}

void DoubleProperty::updateValue(QWidget *editor)
{
	if (const QLineEdit *lineEdit = qobject_cast<const QLineEdit*>(editor)) {
		const double newValue = lineEdit->text().toDouble();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}

	}
}

// -------------------------------------------------------------------------
// QDoublePropertySpinBox also emits editingFinished when the spinbox is used
class QDoublePropertySpinBox: public QDoubleSpinBox
{
public:
	QDoublePropertySpinBox(QWidget *parent = 0)
		: QDoubleSpinBox(parent) { }

	void stepBy(int steps)
	{
		QDoubleSpinBox::stepBy(steps);
		emit editingFinished();
	}
};

SpinBoxDoubleProperty::SpinBoxDoubleProperty(double value, const QString &name)
	: AbstractProperty<double>(value, name), m_low(-HUGE_VAL), m_hi(HUGE_VAL)
{
}

void SpinBoxDoubleProperty::setRange(double low, double hi)
{
	m_low = low;
	m_hi = hi;
}

QString SpinBoxDoubleProperty::specialValue() const
{
	return m_specialValue;
}

void SpinBoxDoubleProperty::setSpecialValue(const QString &specialValue)
{
	m_specialValue = specialValue;
}

void SpinBoxDoubleProperty::setValue(const QVariant &value)
{
	m_value = value.toDouble();
}

QString SpinBoxDoubleProperty::toString() const
{
	return QString::number(m_value);
}

QWidget *SpinBoxDoubleProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QDoubleSpinBox *spinBox = new QDoublePropertySpinBox(parent);
	spinBox->setFrame(0);
	spinBox->setSpecialValueText(m_specialValue);
	spinBox->setDecimals(6);
	spinBox->setRange(m_low, m_hi);
	spinBox->setValue(m_value);
	spinBox->selectAll();

	QObject::connect(spinBox, SIGNAL(editingFinished()), target, receiver);

	return spinBox;
}

void SpinBoxDoubleProperty::updateEditorContents(QWidget *editor)
{
	if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor)) {
		spinBox->setValue(m_value);
	}
}

void SpinBoxDoubleProperty::updateValue(QWidget *editor)
{
	if (const QDoubleSpinBox *spinBox = qobject_cast<const QDoubleSpinBox*>(editor)) {
		const double newValue = spinBox->value();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}

#if 0
// -------------------------------------------------------------------------
CharProperty::CharProperty(QChar value, const QString &name)
	: AbstractProperty<QChar>(value, name)
{
}

void CharProperty::setValue(const QVariant &value)
{
	m_value = value.toChar();
}

QString CharProperty::toString() const
{
	return QString(m_value);
}

QWidget *CharProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	lineEdit->setFrame(0);
	lineEdit->setInputMask(QLatin1String("X; "));
	QObject::connect(lineEdit, SIGNAL(textChanged(QString)), target, receiver);

	return lineEdit;
}

void CharProperty::updateEditorContents(QWidget *editor)
{
	if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor)) {
		if (lineEdit->text() != QString(m_value)) {
			lineEdit->setText(QString(m_value));
			lineEdit->setCursorPosition(0);
		}
	}
}

void CharProperty::updateValue(QWidget *editor)
{
	if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor)) {
		lineEdit->setCursorPosition(0);
		QChar newValue = QLatin1Char(' ');
		if (lineEdit->text().size() > 0)
			newValue = lineEdit->text().at(0);

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}
#endif

// -------------------------------------------------------------------------
SeparatorProperty::SeparatorProperty(const QString &value, const QString &name)
	: StringProperty(value, name)
{
}

QWidget *SeparatorProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	Q_UNUSED(parent);
	Q_UNUSED(target);
	Q_UNUSED(receiver);
	return 0;
}

bool SeparatorProperty::hasEditor() const
{ return false; }

void SeparatorProperty::updateEditorContents(QWidget *editor)
{ Q_UNUSED(editor); }

void SeparatorProperty::updateValue(QWidget *editor)
{ Q_UNUSED(editor); }


#if 0
// -------------------------------------------------------------------------
UrlProperty::UrlProperty(const QUrl &value, const QString &name)
	: AbstractPropertyGroup(name),
m_value(value)
{
}

QVariant UrlProperty::value() const
{
	return m_value;
}

void UrlProperty::setValue(const QVariant &value)
{
	m_value = value.toUrl();
}

QString UrlProperty::toString() const
{
	return m_value.toString();
}

QWidget *UrlProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	QLineEdit *lineEdit = new QLineEdit(parent);
	lineEdit->setFrame(0);

	QObject::connect(lineEdit, SIGNAL(textChanged(QString)), target, receiver);
	return lineEdit;
}

void UrlProperty::updateEditorContents(QWidget *editor)
{
	if (QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor)) {
		if (QUrl(lineEdit->text()) != m_value)
			lineEdit->setText(m_value.toString());
	}
}

void UrlProperty::updateValue(QWidget *editor)
{
	if (const QLineEdit *lineEdit = qobject_cast<const QLineEdit*>(editor)) {
		const QUrl newValue = QUrl(lineEdit->text());

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}
#endif

FileLineEdit::FileLineEdit(QWidget* parent, const QString& value, const QString& path, const QString& filter)
	: QWidget(parent), m_path(path), m_filter(filter)
{
	horizontalLayout = new QHBoxLayout(this);
	horizontalLayout->setSpacing(0);
	horizontalLayout->setMargin(0);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	lineEdit = new QLineEdit(this);
	lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

	horizontalLayout->addWidget(lineEdit);

	toolButton = new QToolButton(this);
	toolButton->setObjectName(QString::fromUtf8("toolButton"));

	horizontalLayout->addWidget(toolButton);

	toolButton->setText("...");

	QMetaObject::connectSlotsByName(this);
}

#include "filedialog.h"
void FileLineEdit::on_toolButton_clicked()
{
	QString fn = FileDialog::getOpenFileName(this, "Selete File", m_path, m_filter);

	if (fn.isEmpty()) {
		return;
	}

	lineEdit->setText(fn);
	emit fileSelected(fn);
}

void FileLineEdit::setFilename(const QString& text)
{
	lineEdit->setText(text);
}

QString FileLineEdit::filename() const
{
	return lineEdit->text();
}

FileProperty::FileProperty(const QString& value, const QString& name, const QString& path, const QString& filter)
 	: StringProperty(value, name)
	, m_path(path)
	, m_filter(filter)
{

}

bool FileProperty::hasEditor() const
{
	return true;
}

QWidget* FileProperty::createEditor(QWidget *parent, const QObject *target, const char *receiver) const
{
	FileLineEdit* editor = new FileLineEdit(parent, m_value, m_path, m_filter);
	QObject::connect(editor, SIGNAL(fileSelected(QString)), target, receiver);
	return editor;
}

void FileProperty::updateEditorContents(QWidget *editor)
{
	FileLineEdit *fle = qobject_cast<FileLineEdit*>(editor);
	if (fle == 0)
		return;
	fle->setFilename(toString());
}

void FileProperty::updateValue(QWidget *editor)
{
	if (const FileLineEdit *fle = qobject_cast<const FileLineEdit*>(editor)) {
		const QString newValue = fle->filename();

		if (newValue != m_value) {
			m_value = newValue;
			setChanged(true);
		}
	}
}
