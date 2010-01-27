#include "script_p.h"

AX_BEGIN_NAMESPACE

struct Connection {
	Object *sender;
	Member *signal;
	Object *receiver;
	Member *slot;

	IntrusiveLink<Connection> senderLink;
	IntrusiveLink<Connection> receiverLink;
};

typedef IntrusiveList<Connection, &Connection::senderLink> SenderList;
typedef IntrusiveList<Connection, &Connection::receiverLink> ReceiverList;


//--------------------------------------------------------------------------
// class Object
//--------------------------------------------------------------------------

Object::Object() : m_scriptClass(0)
{}

Object::~Object()
{
	resetObjectName();
}

Member *Object::findMember(const FixedString &name) const
{
	CppClass *typeinfo = getCppClass();

	while (typeinfo) {
		Member *member = typeinfo->findMember(name);
		if (member) return member;
		typeinfo = typeinfo->getBase();
	}

	if (!m_scriptClass)
		return 0;

	return m_scriptClass->findMember(name);
}

CppClass *Object::getCppClass() const
{
	return Object::registerCppClass();
}

CppClass *Object::registerCppClass()
{
	static CppClass *ms_metaInfo = 0;

	if (!ms_metaInfo) {
		ms_metaInfo = new CppClass_<Object>("Object", nullptr);
		ms_metaInfo->addProperty("objectName", &Object::get_objectName, &Object::set_objectName);

		g_scriptSystem->registerCppClass(ms_metaInfo);
	}
	return ms_metaInfo;
}


void Object::set_objectName(const String &name)
{
	setObjectName(name);
}

String Object::get_objectName() const
{
	return m_objectName;
}


bool Object::inherits(const FixedString &cls) const
{
	CppClass *typeinfo = getCppClass();

	for (; typeinfo; typeinfo=typeinfo->getBase()) {
		if (cls == typeinfo->getName()) {
			return true;
		}
	}

	return false;
}

bool Object::getProperty(const FixedString &name, Variant &ret) const
{
	Member *m = findMember(name);
	if (!m || !m->isProperty()) {
		return false;
	}

	return m->getProperty(this, ret);
}

bool Object::getProperty(const FixedString &name, const Ref &ref) const
{
	Member *m = findMember(name);
	if (!m || !m->isProperty()) {
		return false;
	}

	Variant ret;
	bool v = m->getProperty(this, ret);
	if (!v) return false;

	return ret.castTo(ref);	
}

bool Object::setProperty(const FixedString &name, const ConstRef &value)
{
	Member *m = findMember(name);
	if (!m || !m->isProperty() || m->isConst()) {
		return false;
	}

	m->setProperty(this, ConstRef(value.getTypeId(), value.getPointer()));

	return true;
}

bool Object::setProperty(const FixedString &name, const char *value)
{
	Member *member = findMember(name);

	if (!member) {
		return false;
	}

	if (!member->isProperty()) {
		return false;
	}

	if (member->isConst()) {
		return false;
	}

	Variant var;
	var.fromString(member->getPropType(), value);
	member->setProperty(this, ConstRef(var.getTypeId(), var.getPointer()));

	return true;
}

void Object::writeProperties(File *f, int indent) const
{
	String indstr(indent*2, ' ');
#define INDENT if (indent) f->printf("%s", indstr.c_str());

	// write properties
	CppClass *typeinfo = getCppClass();

	Variant prop;
	while (typeinfo) {
		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) continue;

			if (m->isConst()) continue;

			if (!m->getProperty(this, prop)) continue;

			INDENT; f->printf("  %s=\"%s\"\n", m->getName().c_str(), prop.toString().c_str());
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const ScriptClass *classinfo = getScriptClass();
	if (!classinfo) return;

	const SqProperties &props = classinfo->getMembers();

	AX_FOREACH(SqProperty *m, props) {
		if (m->getPropKind() == Member::kGroup)
			continue;

		if (!m->getProperty(this, prop)) continue;

		INDENT; f->printf("  %s=\"%s\"\n", m->getName().c_str(), prop.toString().c_str());
	}

#undef INDENT
}

void Object::readProperties(const TiXmlElement *node)
{
	const TiXmlAttribute *attr = node->FirstAttribute();

	for (; attr; attr = attr->Next()) {
		this->setProperty(attr->Name(), attr->Value());
	}
}

void Object::copyPropertiesFrom(const Object *rhs)
{
	// write properties
	CppClass *typeinfo = rhs->getCppClass();
	Variant prop;

	while (typeinfo) {
		// don't copy objectname
		if (typeinfo == Object::registerCppClass())
			break;

		const MemberSeq &members = typeinfo->getMembers();

		AX_FOREACH(Member *m, members) {
			if (!m->isProperty()) {
				continue;
			}

			if (m->isConst()) {
				continue;
			}

			if (!m->getProperty(rhs, prop)) continue;

			setProperty(m->getName(), prop);
		}

		typeinfo = typeinfo->getBase();
	}

	// write script properties
	const ScriptClass *classinfo = getScriptClass();
	if (!classinfo) return;

	const SqProperties &props = classinfo->getMembers();

	AX_FOREACH(SqProperty *m, props) {
		if (m->getPropKind() == Member::kGroup)
			continue;

		if (!m->getProperty(rhs, prop)) continue;

		setProperty(m->getName(), prop);
	}
}

void Object::setObjectName(const String &name)
{
	if (m_objectName == name) {
		return;
	}

	resetObjectName();

	m_objectName = name;
	m_objectNamespace = getNamespace();

	if (m_objectName.empty())
		return;

	if (m_scriptInstance.isNull())
		initScriptClass(nullptr);

	sqObject ns = g_mainVM->getScoped(m_objectNamespace.c_str());
	ns.setValue(m_objectName.c_str(), m_scriptInstance.getSqObject());
}

void Object::resetObjectName()
{
	if (m_objectName.empty())
		return;

	sqObject ns = g_mainVM->getScoped(m_objectNamespace.c_str());
	ns.setValue(m_objectName.c_str(), sqObject());
}

void Object::doPropertyChanged()
{
	onPropertyChanged();
}

void Object::onPropertyChanged()
{
}

void Object::setRuntime(const FixedString &name, const Variant &val)
{
}

Variant Object::getRuntime(const FixedString &name)
{
	return Variant();
}

void Object::initScriptClass(const ScriptClass *sqclass)
{
	if (!m_scriptInstance.isNull())
		Errorf("object script already initialized");

	if (!sqclass)
		sqclass = g_scriptSystem->findScriptClass("Object");

	AX_ASSERT(sqclass);

	sq_pushobject(VM, sqclass->getScriptValue().getSqObject()); // +1
	SQRESULT sqresult = sq_createinstance(VM, -1);	// +2
	AX_ASSURE(SQ_SUCCEEDED(sqresult));

	m_scriptInstance.getSqObject().attachToStackObject(VM, -1);
	sq_pop(VM, 2); // -2

	AX_ASSURE(m_scriptInstance.isInstance());
	m_scriptInstance.getSqObject().setInstanceUP(this);

	m_scriptClass = sqclass;
}

bool Object::invokeMethodRt(const FixedString &methodName, const Ref &ret,
							const ConstRef &arg0, const ConstRef &arg1, const ConstRef &arg2,
							const ConstRef &arg3, const ConstRef &arg4)
{
	CppClass *mi = getCppClass();
	if (!mi) return false;

	Member *member = mi->findMember(methodName);

	if (!member)
		return false;

	if (member->getType() != Member::kMethodType)
		return false;

	// check types
	Variant::TypeId needReturnType = member->getReturnType();
	Variant realRet(ret.getTypeId(), ret.getPointer(), Variant::InitRef);
	bool castRet = false;

	if (ret.getTypeId() != needReturnType) {
		if (needReturnType != Variant::kVoid && ret.getTypeId() != Variant::kVoid) {
			if (!Variant::canCast(needReturnType, ret.getTypeId())) {
				return false;
			} else {
				AX_INIT_STACK_VARIANT(realRet, needReturnType);
				castRet = true;
			}
		}
	}

	ConstRef args[] = { arg0, arg1, arg2, arg3, arg4 };

	Variant realArg[Member::MaxArgs];
	const Variant::TypeId *needTypeIds = member->getArgsType();
	for (int i = 0; i <member->argc(); i++) {
		if (args[i].getTypeId() == needTypeIds[i])
			continue;

		if (!Variant::canCast(args[i].getTypeId(), needTypeIds[i]))
			return false;

		AX_INIT_STACK_VARIANT(realArg[i], needTypeIds[i]);

		bool casted = args[i].castTo(realArg[i]);
		if (!casted)
			return false;

		args[i].set(realArg[i].getTypeId(), realArg[i].getPointer());
	}

	const void *argDatas[] = {
		args[0].getPointer(), args[1].getPointer(), args[2].getPointer(), args[3].getPointer(), args[4].getPointer()
	};

	// really call
	int numResult = member->invoke(this, realRet.getPointer(), argDatas);

	if (numResult && castRet) {
		return realRet.castTo(ret);
	}

	return true;
}

bool Object::invokeMethod(const FixedString &methodName, const ConstRef &arg0/*=ConstRef()*/, const ConstRef &arg1/*=ConstRef()*/, const ConstRef &arg2/*=ConstRef()*/, const ConstRef &arg3/*=ConstRef()*/, const ConstRef &arg4/*=ConstRef()*/)
{
	return invokeMethodRt(methodName, Ref(), arg0, arg1, arg2, arg3, arg4);
}

bool Object::invokeScriptRt(const FixedString &methodName, const Ref &ret, const ConstRef &arg0, const ConstRef &arg1, const ConstRef &arg2, const ConstRef &arg3, const ConstRef &arg4)
{
	if (m_scriptInstance.isNull())
		return false;


	sqObject func = m_scriptInstance.getSqObject().getValue(methodName.c_str());

	if (func.isNull()) {
		if (!m_currentState.isNull())
			func = m_currentState.getSqObject().getValue(methodName.c_str());
	}

	if (func.isNull() || !func.isCallable())
		return false;

	// push this
	ScopedStack ss(VM);

	sq_pushobject(VM, func);
	sqVM::pushMeta(VM, AX_ARG(this));
	SQRESULT sqr = sq_call(VM, 1, 0, true);

	if (SQ_SUCCEEDED(sqr)) return true;

	return false;
}

bool Object::invokeScript(const FixedString &methodName, const ConstRef &arg0/*=ConstRef()*/, const ConstRef &arg1/*=ConstRef()*/, const ConstRef &arg2/*=ConstRef()*/, const ConstRef &arg3/*=ConstRef()*/, const ConstRef &arg4/*=ConstRef()*/)
{
	return invokeScriptRt(methodName, Ref(), arg0, arg1, arg2, arg3, arg4);
}

AX_END_NAMESPACE
