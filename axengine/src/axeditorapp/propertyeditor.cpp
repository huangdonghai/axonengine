/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "propertyeditor.h"
#include "propertyeditor_model.h"
#include "propertyeditor_delegate.h"

Q_GLOBAL_STATIC_WITH_ARGS(PropertyCollection, dummy_collection, (QLatin1String("<empty>")))

PropertyEditor::PropertyEditor(QWidget *parent)
	: QTreeView(parent)
	, m_model(new PropertyEditorModel(this))
	, m_itemDelegate(new PropertyEditorDelegate(this))
{
    connect(m_itemDelegate, SIGNAL(editorOpened()), this, SIGNAL(editorOpened()));
    connect(m_itemDelegate, SIGNAL(editorClosed()), this, SIGNAL(editorClosed()));
    setModel(m_model);
    setItemDelegate(m_itemDelegate);
    setTextElideMode (Qt::ElideMiddle);
	setIndentation(12);

    connect(header(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerDoubleClicked(int)));

    connect(m_itemDelegate, SIGNAL(resetProperty(const QString &)), m_model, SIGNAL(resetProperty(const QString &)));
    setInitialInput(0);

    setAlternatingRowColors(true);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollMode(ScrollPerPixel);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(activated(QModelIndex)), this, SLOT(expand(QModelIndex)));

    connect(m_model, SIGNAL(propertyChanged(IProperty*)), this, SIGNAL(propertyChanged(IProperty*)));

	connect(this,SIGNAL(expanded(QModelIndex)),this,SLOT(setProperHeight()));
	connect(this,SIGNAL(collapsed(QModelIndex)),this,SLOT(setProperHeight()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);
}

PropertyEditor::~PropertyEditor()
{
}

bool PropertyEditor::isReadOnly() const
{
    return m_itemDelegate->isReadOnly();
}

void PropertyEditor::setReadOnly(bool readOnly)
{
    m_itemDelegate->setReadOnly(readOnly);
}

void PropertyEditor::setInitialInput(IProperty *initialInput)
{
    const int oldColumnWidth  = columnWidth(0);

    QScrollBar *sb = verticalScrollBar();

    const int position = sb->value();
    const bool resizeToColumn = !m_model->initialInput() || m_model->initialInput() == dummy_collection();

    if (!initialInput)
        initialInput = dummy_collection();

    m_model->setInitialInput(initialInput);

    setSelectionMode(QTreeView::SingleSelection);
    setSelectionBehavior(QTreeView::SelectRows);
    setRootIsDecorated(true);

    setEditTriggers(QAbstractItemView::CurrentChanged|QAbstractItemView::SelectedClicked);
    setRootIndex(m_model->indexOf(initialInput));

    if (resizeToColumn) {
        resizeColumnToContents(0);
    } else {
        setColumnWidth (0, oldColumnWidth);
    }

	setProperHeight();
    sb->setValue(position);
}

IProperty *PropertyEditor::initialInput() const
{
    return m_model->initialInput();
}

void PropertyEditor::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
   // designer fights the style it uses. :(
    static const bool mac_style = QApplication::style()->inherits("QMacStyle");
    static const int windows_deco_size = 9;

    QStyleOptionViewItem opt = viewOptions();

	QModelIndex indexnext = indexBelow(index);

    IProperty *property = static_cast<const PropertyEditorModel*>(model())->privateData(index);
	IProperty *propnext = static_cast<const PropertyEditorModel*>(model())->privateData(indexnext);

    if (index.column() == 0 && property && property->changed()) {
        opt.font.setBold(true);
    }

    if (property && property->isSeparator()) {
        painter->fillRect(rect, opt.palette.dark());
    }

    if (0) {
		if (model()->hasChildren(index))
			opt.state |= QStyle::State_Children;

        QRect primitive(rect.left(), rect.top(), indentation(), rect.height());

        if (!mac_style) {
            primitive.moveLeft(primitive.left() + (primitive.width() - windows_deco_size)/2);
            primitive.moveTop(primitive.top() + (primitive.height() - windows_deco_size)/2);
            primitive.setWidth(windows_deco_size);
            primitive.setHeight(windows_deco_size);
        }

        opt.rect = primitive;

        if (isExpanded(index))
            opt.state |= QStyle::State_Open;

		if (propnext && propnext->isSeparator()) {
			opt.state &= ~QStyle::State_Sibling;
		} else {
			opt.state |= QStyle::State_Sibling | QStyle::State_Item;
		}

        style()->drawPrimitive(QStyle::PE_IndicatorBranch, &opt, painter, this);
    }

    const QPen savedPen = painter->pen();
    const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->setPen(QPen(color));
    painter->drawLine(rect.x(), rect.bottom(), rect.right(), rect.bottom());
    painter->setPen(savedPen);

	QTreeView::drawBranches(painter,rect,index);
	return;
}

void PropertyEditor::keyPressEvent(QKeyEvent *ev)
{
/*    QApplication::syncX();*/
    QTreeView::keyPressEvent(ev);
}

QStyleOptionViewItem PropertyEditor::viewOptions() const
{
    QStyleOptionViewItem option = QTreeView::viewOptions();
    option.showDecorationSelected = true;
    return option;
}

void PropertyEditor::focusInEvent(QFocusEvent *event)
{
    QAbstractScrollArea::focusInEvent(event);
    viewport()->update();
}

void PropertyEditor::headerDoubleClicked(int column)
{
    resizeColumnToContents(column);
}

void  PropertyEditor::dragEnterEvent(QDragEnterEvent *event)
{
//    if (!isReadOnly() && ResourceMimeData::isResourceMimeData(event->mimeData(), ResourceMimeData::Image))
//       event->acceptProposedAction();
//   else
        event->ignore();
}

void  PropertyEditor::dragMoveEvent(QDragMoveEvent *event)
{
//    if (!isReadOnly() && ResourceMimeData::isResourceMimeData(event->mimeData(), ResourceMimeData::Image))
//        event->acceptProposedAction();
//    else
        event->ignore();
}

void PropertyEditor::dropEvent (QDropEvent * event)
{
#if 0
    bool accept = false;
    do {
        if (isReadOnly())
            break;

        const QModelIndex index = indexAt(event->pos());
        if (!index.isValid())
            break;

        ResourceMimeData md;
        if (!md.fromMimeData(event->mimeData()) || md.type() != ResourceMimeData::Image)
            break;

        accept = m_model->resourceImageDropped(index, md);
    } while (false);

    if (accept) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
#else
	event->ignore();
#endif
}

void PropertyEditor::setProperHeight() {

	QModelIndex root = rootIndex();
	int h = rowHeight(root);
	int row = 0;

	for (QModelIndex next = indexBelow(root); next.isValid(); next = indexBelow(next), row++) {
		h += rowHeight(next);

		IProperty *property = static_cast<const PropertyEditorModel*>(m_model)->privateData(next);

		if (property && property->isSeparator()) {
			setFirstColumnSpanned(next.row(), next.parent(), true);
		} else {
			setFirstColumnSpanned(next.row(), next.parent(), false);
		}
	}

	setMinimumHeight(h + + header()->geometry().height() + 2);

	// compute total height
//	QSize size = this->maximumViewportSize();
//	setMinimumHeight(size.height() + header()->geometry().height());
	return;
}

void PropertyEditor::initFromObject(Object *obj) {
	if (!obj) {
		setInitialInput(nullptr);
		return;
	}

	PropertyCollection *collection = new PropertyCollection(QLatin1String("<root>"));

	MetaInfo *typeinfo = obj->getMetaInfo();
	Variant prop;

	for (; typeinfo; typeinfo = typeinfo->getBase()) {
		IProperty *p;
		p = new SeparatorProperty("", typeinfo->getName()); collection->addProperty(p);

		const MemberSeq &members = typeinfo->getMembers();

		foreach(Member *m, members) {
			if (!m->isProperty()) {
				continue;
			}

			if (m->isConst()) {
				continue;
			}

			p = nullptr;

			Variant::TypeId type = m->getPropType();
			switch (type) {
			case Variant::kBool:
				p = new BoolProperty(m->getPropertyNoCheck(obj), m->getName());
				break;
			case Variant::kInt:
				p = new IntProperty(m->getPropertyNoCheck(obj), m->getName());
				break;
			case Variant::kFloat:
				p = new DoubleProperty(m->getPropertyNoCheck(obj), m->getName());
				break;
			case Variant::kString:
				p = new StringProperty(u2q(m->getPropertyNoCheck(obj)), m->getName());
				break;
			case Variant::kVector3:
				p = new VectorProperty(m->getPropertyNoCheck(obj), m->getName());
				break;
			case Variant::kColor3:
				p = new ColorProperty(x2q(m->getPropertyNoCheck(obj)), m->getName());
				break;
			}

			if (p) {
				collection->addProperty(p);
			}
		}
	}

	setInitialInput(collection);
}

void PropertyEditor::applyToObject(Object *obj) {
	if (!obj) {
		return;
	}

	IProperty *root = m_model->initialInput();

	if (!root) {
		return;
	}

	if (root->kind() != IProperty::kGroup) {
		return;
	}

	IPropertyGroup *group = (IPropertyGroup*)root;

	for (int i = 0; i < group->propertyCount(); i++) {
		IProperty *prop = group->propertyAt(i);

		if (prop->isFake()) {
			continue;
		}

		if (prop->isSeparator()) {
			continue;
		}

		obj->setProperty(q2u(prop->propertyName()).c_str(), q2x(prop->value()));
	}
}

void PropertyEditor::setData(const QString &propName, QVariant value){
	IProperty *root = m_model->initialInput();

	if (!root) {
		return;
	}

	if (root->kind() != IProperty::kGroup) {
		return;
	}

	IPropertyGroup *group = (IPropertyGroup*)root;

	for (int i = 0; i < group->propertyCount(); i++) {
		IProperty *prop = group->propertyAt(i);

		if (prop->isFake()) {
			continue;
		}

		if (prop->isSeparator()) {
			continue;
		}
		if (prop->propertyName() == propName){
			prop->setValue(value);
			m_model->refreshData(prop);
			return;
		}		
	}
}

bool PropertyEditor::initScriptProp(Object *obj) {
	if (!obj || !obj->getScriptClass()) {
		setInitialInput(nullptr);
		return false;
	}

	const SqClass *ci = obj->getScriptClass();

	const SqProperties &props = ci->getMembers();

	PropertyCollection *collection = new PropertyCollection(QLatin1String("<root>"));
	AX_FOREACH(SqProperty *m, props) {
		IProperty *p = 0;
		IProperty *group = 0;

		switch (m->getPropKind()) {
		case Variant::kVoid:
			break;
		case Variant::kBool:
			p = new BoolProperty(m->getPropertyNoCheck(obj), m->getName());
			break;
		case Variant::kInt:
			p = new IntProperty(m->getPropertyNoCheck(obj), m->getName());
			break;
		case Variant::kFloat:
			p = new DoubleProperty(m->getPropertyNoCheck(obj), m->getName());
			break;
		case Variant::kString:
			p = new StringProperty(u2q(m->getPropertyNoCheck(obj)), m->getName());
			break;
		case Variant::kObject:
			break;
		case Variant::kVector3:
			p = new VectorProperty(m->getPropertyNoCheck(obj), m->getName());
			break;
		case Variant::kColor3:
			p = new ColorProperty(x2q(m->getPropertyNoCheck(obj)), m->getName());
			break;
		case Variant::kPoint:
		case Variant::kRect:
			break;
		case Member::kEnum:
			p = new EnumProperty(m->getEnumItems(), m->getPropertyNoCheck(obj), m->getName());
			break;
		case Member::kFlag:
			p = new FlagsProperty(m->getEnumItems(), m->getPropertyNoCheck(obj), m->getName());
			break;
		case Member::kTexture:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/textures", "*.dds");
			break;
		case Member::kSound:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/sounds", "*.ogg");
			break;
		case Member::kModel:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/models", "*.mesh");
			break;
		case Member::kAnimation:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/models", "*.anim");
			break;
		case Member::kSpeedTree:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/speedtrees", "*.spt");
			break;
		case Member::kMaterial:
			p = new FileProperty(u2q(m->getPropertyNoCheck(obj)), m->getName(), "/materials", "*.mtr");
			break;
		case Member::kGroup:
			p = new SeparatorProperty("", m->getName());
			break;
		}

		if (p) {
			collection->addProperty(p);
		}
	}

	setInitialInput(collection);

	return true;
}
