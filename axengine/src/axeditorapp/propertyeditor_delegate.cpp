/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "propertyeditor_delegate.h"
#include "propertyeditor_model.h"

EditorWithReset::EditorWithReset(const IProperty *property, PropertyEditorModel *model, QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    m_property = property;
    m_child_editor = 0;
    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_model = model;

    QToolButton *button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIcon(QIcon(QLatin1String("resetproperty.png")));
    button->setIconSize(QSize(8,8));
    button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding));
    m_layout->addWidget(button);
    connect(button, SIGNAL(clicked()), this, SLOT(emitResetProperty()));
}

void EditorWithReset::emitResetProperty()
{
    emit resetProperty(m_property, m_model);
}

void EditorWithReset::setChildEditor(QWidget *child_editor)
{
    m_child_editor = child_editor;

    m_child_editor->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
    m_layout->insertWidget(0, m_child_editor);
    setFocusProxy(m_child_editor);
}

PropertyEditorDelegate::PropertyEditorDelegate(QObject *parent)
    : QItemDelegate(parent),
      m_readOnly(false),
      m_syncing(false),
      m_lastEdited(0)
{
}

PropertyEditorDelegate::~PropertyEditorDelegate()
{
}

bool PropertyEditorDelegate::eventFilter(QObject *object, QEvent *event)
{
    QWidget *editor = qobject_cast<QWidget*>(object);
    if (editor && qobject_cast<QLineEdit*>(editor->parent()))
        editor = editor->parentWidget();
    if (editor && qobject_cast<EditorWithReset*>(editor->parent()))
        editor = editor->parentWidget();

    switch (event->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease: {
            QKeyEvent *ke = static_cast<QKeyEvent*>(event);
            if (!(ke->modifiers() & Qt::ControlModifier)
                && (ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down)) {
                event->ignore();
                return true;
            }
            if (object->metaObject()->className() == QLatin1String("QtKeySequenceEdit")) {
                event->ignore();
                return false;
            }
        } break;

        case QEvent::FocusOut:
            if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
                QWidget *w = QApplication::focusWidget();
                while (w) { // do not worry about focus changes internally in the editor
                    if (w == editor)
                        return false;
                    w = w->parentWidget();
                }

                emit commitData(editor);
            }
            return false;

        default:
            break;
    }

    return QItemDelegate::eventFilter(editor ? editor : object, event);
}

void PropertyEditorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    QStyleOptionViewItem option = opt;

    const QAbstractItemModel *model = index.model();
    IProperty *property = static_cast<const PropertyEditorModel*>(model)->privateData(index);
    if (index.column() == 0 && property && property->changed()) {
        option.font.setBold(true);
    }

    if (property && property->isSeparator()) {
        option.palette.setColor(QPalette::Text, option.palette.color(QPalette::BrightText));
        option.font.setBold(true);
        option.state &= ~QStyle::State_Selected;
    }

    if (index.column() == 1) {
        option.state &= ~QStyle::State_Selected;
    }

    option.state &= ~QStyle::State_HasFocus;

    if (property && property->isSeparator()) {
        const QBrush bg = option.palette.dark();
        painter->fillRect(option.rect, bg);
    }

    const QPen savedPen = painter->pen();

    QItemDelegate::paint(painter, option, index);

    const QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &option));
    painter->setPen(QPen(color));
    if (index.column() == 1 || !(property && property->isSeparator())) {
        const int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->drawLine(option.rect.x(), option.rect.bottom(),
            option.rect.right(), option.rect.bottom());
    painter->setPen(savedPen);
}

QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(opt, index) + QSize(2, 2);
}

bool PropertyEditorDelegate::isReadOnly() const
{
    return m_readOnly;
}

void PropertyEditorDelegate::setReadOnly(bool readOnly)
{
    // ### close the editor
    m_readOnly = readOnly;
}

void PropertyEditorDelegate::slotDestroyed(QObject *object)
{
    if (m_lastEdited == object) {
        m_lastEdited = 0;
        emit editorClosed();
    }
}

QWidget *PropertyEditorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    PropertyEditorModel *model = const_cast<PropertyEditorModel *>(static_cast<const PropertyEditorModel *>(index.model()));
    const IProperty *property = model->privateData(index);
    if (property == 0)
        return 0;

    QWidget *editor = 0;

    if (!isReadOnly() && property->hasEditor()) { // ### always true
        if (property->hasReset()) {
            EditorWithReset *editor_w_reset
                = new EditorWithReset(property, model, parent);
            QWidget *child_editor
                = property->createEditor(editor_w_reset, editor_w_reset, SIGNAL(sync()));
            editor_w_reset->setChildEditor(child_editor);
            connect(editor_w_reset, SIGNAL(sync()), this, SLOT(sync()));
            connect(editor_w_reset, SIGNAL(resetProperty(const IProperty *, PropertyEditorModel *)),
                        this, SLOT(resetProperty(const IProperty *, PropertyEditorModel *)));

            editor = editor_w_reset;
            if (QLineEdit* edit = qobject_cast<QLineEdit*>(child_editor)) {
                // in case of TextPropertyEditor install the filter on it's private QLineEdit
                edit->installEventFilter(const_cast<PropertyEditorDelegate *>(this));
            } else
                child_editor->installEventFilter(const_cast<PropertyEditorDelegate *>(this));
        } else {
            editor = property->createEditor(parent, this, SLOT(sync()));
            editor->installEventFilter(const_cast<PropertyEditorDelegate *>(this));
        }
    }

    connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotDestroyed(QObject *)));

    PropertyEditorDelegate *that = const_cast<PropertyEditorDelegate *>(this);
    if (!m_lastEdited)
        emit that->editorOpened();
    m_lastEdited = editor;
    return editor;
}

void PropertyEditorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (EditorWithReset *editor_w_reset = qobject_cast<EditorWithReset*>(editor))
        editor = editor_w_reset->childEditor();

    const QAbstractItemModel *model = index.model();
    IProperty *property = static_cast<const PropertyEditorModel*>(model)->privateData(index);
    if (property && property->hasEditor() && !m_syncing) {
        property->updateEditorContents(editor);
    }
}

void PropertyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (EditorWithReset *editor_w_reset = qobject_cast<EditorWithReset*>(editor))
        editor = editor_w_reset->childEditor();

    if (IProperty *property = static_cast<const PropertyEditorModel*>(model)->privateData(index)) {
        property->updateValue(editor);
		model->setData(index, property->value(), Qt::EditRole);
    }
}

void PropertyEditorDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, const QPixmap &pixmap) const
{
    QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}

void PropertyEditorDelegate::sync()
{
    m_syncing = true;
    QWidget *w = qobject_cast<QWidget*>(sender());
    if (w == 0)
        return;
    emit commitData(w);
    m_syncing = false;
}

void PropertyEditorDelegate::resetProperty(const IProperty *property, PropertyEditorModel *model)
{
    QString propName = property->propertyName();
    emit resetProperty(propName);
}

void PropertyEditorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QItemDelegate::updateEditorGeometry(editor, option, index);
    editor->setGeometry(editor->geometry().adjusted(0, 0, -1, -1));
}
