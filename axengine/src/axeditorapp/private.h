/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_EDITOR_COMMON_H
#define AX_EDITOR_COMMON_H


#include <axcore/public.h>
#include <axclient/public.h>
#include <axlogic/public.h>
#include <axeditor/public.h>

AX_USE_NAMESPACE;

Q_DECLARE_METATYPE (Vector3);
Q_DECLARE_METATYPE (Point);
Q_DECLARE_METATYPE (Rect);

class Workbench;
struct GlobalData {
	GlobalData(QApplication *m_app);
	~GlobalData();

	QFont defaultFont;
	QCursor cursorHand;
	QCursor cursorMove;
	QCursor cursorRotate;
	QCursor cursorScale;
	QCursor cursorViewPan;
	QCursor cursorViewZoom;
	QCursor cursorViewRotate;
	QString language;
	QString style;
	QString curModel;
	QString curMaterial;
};

class Application;

extern GlobalData *g_globalData;
extern Application *g_app;
extern Workbench *g_workbench;
extern QSettings *g_settings;

inline QString u2q(const String &str) {
	return QString::fromUtf8(str.c_str(), str.size());
}

inline String q2u(const QString &qstr) {
	String str;
#if defined(_MSC_VER) && _MSC_VER >= 1400
	// VS2005 crashes if the string is empty
	if (!qstr.length())
		return str;
#endif
	QByteArray ba = qstr.toUtf8();
	return String(ba.constData(), ba.size());
}

inline QColor x2q(const Color3 &x) {
	Rgb rgb = x.toRgb();
	return QColor(rgb.r, rgb.g, rgb.b);
}

inline Color3 q2x(const QColor &q) {
	Color3 res = Color3(q.red(), q.green(), q.blue());
	res *= 1.0f / 255.0f;
	return res;
}

inline Variant q2x(const QVariant &q) {
	int type = q.userType();

	if (type == QVariant::Bool) {
		return Variant(q.toBool());
	}

	if (type == QVariant::Int) {
		return Variant(q.toInt());
	}

	if (type == QVariant::Double) {
		return Variant(q.toFloat());
	}

	if (type == QVariant::String) {
		return Variant(q2u(q.toString()));
	}

	if (type == QVariant::Color) {
		return Variant(q2x(qvariant_cast<QColor>(q)));
	}

	if (type == QMetaTypeId<Vector3>::qt_metatype_id()) {
		return Variant(qvariant_cast<Vector3>(q));
	}

	if (type == QMetaTypeId<Point>::qt_metatype_id()) {
		return Variant(qvariant_cast<Point>(q));
	}

	if (type == QMetaTypeId<Rect>::qt_metatype_id()) {
		return Variant(qvariant_cast<Rect>(q));
	}
	return Variant();
}

#endif // AX_EDITOR_COMMON_H
