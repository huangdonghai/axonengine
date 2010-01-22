#include "script_p.h"

AX_BEGIN_NAMESPACE
SquirrelError::SquirrelError(sqVM *vm) 
{
	const SQChar *s;
	sq_getlasterror(vm->m_vm);
	sq_getstring(vm->m_vm,-1,&s);
	if (s) {
		desc = s;
	} else {
		desc = _SC("unknown error");
	}
}



sqObject::sqObject(void)
{
	sq_resetobject(&m_obj);
}

sqObject::~sqObject()
{
	reset();
}

sqObject::sqObject(const sqObject &o)
{
	m_obj = o.m_obj;
	sq_addref(VM,&m_obj);
}

sqObject::sqObject(HSQOBJECT o)
{
	m_obj = o;
	sq_addref(VM,&m_obj);
}

void sqObject::reset(void)
{
	if (VM)
		sq_release(VM,&m_obj);
	else if ( m_obj._type!=OT_NULL && m_obj._unVal.pRefCounted )
		printf( "sqObject::~sqObject - Cannot release\n" ); 

	sq_resetobject(&m_obj);
} // sqObject::reset

sqObject sqObject::clone()
{
	sqObject ret;
	if (getType() == OT_TABLE || getType() == OT_ARRAY) {
		sq_pushobject(VM, m_obj);
		sq_clone(VM, -1);
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 2);
	}

	return ret;
}


bool sqObject::operator == (const sqObject &o)
{
	bool cmp = false;
	HSQUIRRELVM v = VM;
	int oldtop = sq_gettop(v);

	sq_pushobject(v, getObjectHandle());
	sq_pushobject(v, o.getObjectHandle());
	if (sq_cmp(v) == 0)
		cmp = true;

	sq_settop(v, oldtop);
	return cmp;
}

bool sqObject::compareUserPointer( const sqObject &o )
{
	if ( m_obj._type == o.getObjectHandle()._type )
		if ( m_obj._unVal.pUserPointer == o.getObjectHandle()._unVal.pUserPointer )
			return true;

	return false;
}

void sqObject::arrayAppend(const sqObject &o)
{
	if (sq_isarray(m_obj)) {
		sq_pushobject(VM, m_obj);
		sq_pushobject(VM, o.m_obj);
		sq_arrayappend(VM, -2);
		sq_pop(VM, 1);
	}
}

void sqObject::attachToStackObject(HSQUIRRELVM vm, int idx)
{
	HSQOBJECT t;
	sq_getstackobj(vm, idx, &t);
	sq_addref(vm, &t);
	sq_release(vm, &m_obj);
	m_obj = t;
}

bool sqObject::setDelegate(sqObject &obj)
{
	if (obj.getType() == OT_TABLE || obj.getType() == OT_NULL) {
			switch(m_obj._type) {
			case OT_USERDATA:
			case OT_TABLE:
				sq_pushobject(VM, m_obj);
				sq_pushobject(VM, obj.m_obj);
				if (SQ_SUCCEEDED(sq_setdelegate(VM,-2))) {
					sq_pop(VM, 1);
					return true;
				}
				sq_pop(VM, 1);
				break;
			}
	}
	return false;
}

sqObject sqObject::getDelegate()
{
	sqObject ret;
	if (m_obj._type == OT_TABLE || m_obj._type == OT_USERDATA)
	{
		int top = sq_gettop(VM);
		sq_pushobject(VM, m_obj);
		sq_getdelegate(VM, -1);
		ret.attachToStackObject(VM, -1);
		sq_settop(VM, top);
		//		sq_pop(VM, 2);
	}
	return ret;
}

bool sqObject::isNull() const
{
	return sq_isnull(m_obj);
}

bool sqObject::isNumeric() const
{
	return sq_isnumeric(m_obj);
}

int sqObject::len() const
{
	int ret = 0;
	if (sq_isarray(m_obj) || sq_istable(m_obj) || sq_isstring(m_obj)) {
		sq_pushobject(VM, m_obj);
		ret = sq_getsize(VM,-1);
		sq_pop(VM, 1);
	}
	return ret;
}

#define _SETVALUE_INT_BEGIN \
	bool ret = false; \
	int top = sq_gettop(VM); \
	sq_pushobject(VM, m_obj); \
	sq_pushinteger(VM, key);

#define _SETVALUE_INT_END \
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) { \
	ret = true; \
	} \
	sq_settop(VM, top); \
	return ret;

bool sqObject::setValue(int key, const sqObject &val)
{
	_SETVALUE_INT_BEGIN
		sq_pushobject(VM, val.m_obj);
	_SETVALUE_INT_END
}

bool sqObject::setValue(int key, int n)
{
	_SETVALUE_INT_BEGIN
		sq_pushinteger(VM, n);
	_SETVALUE_INT_END
}

bool sqObject::setValue(int key, float f)
{
	_SETVALUE_INT_BEGIN
		sq_pushfloat(VM, f);
	_SETVALUE_INT_END
}

bool sqObject::setValue(int key, const SQChar *s)
{
	_SETVALUE_INT_BEGIN
		sq_pushstring(VM, s,-1);
	_SETVALUE_INT_END
}

bool sqObject::setValue(int key, bool b)
{
	_SETVALUE_INT_BEGIN
		sq_pushbool(VM, b);
	_SETVALUE_INT_END
}

bool sqObject::setValue(const sqObject &key, const sqObject &val)
{
	bool ret = false;
	int top = sq_gettop(VM);
	sq_pushobject(VM, m_obj);
	sq_pushobject(VM, key.m_obj);
	sq_pushobject(VM, val.m_obj);
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) {
		ret = true;
	}
	sq_settop(VM, top);
	return ret;
}

#define _SETVALUE_STR_BEGIN \
	bool ret = false; \
	int top = sq_gettop(VM); \
	sq_pushobject(VM, m_obj); \
	sq_pushstring(VM, key,-1);

#define _SETVALUE_STR_END \
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) { \
	ret = true; \
	} \
	sq_settop(VM, top); \
	return ret;

bool sqObject::setValue(const SQChar *key, const sqObject &val)
{
	_SETVALUE_STR_BEGIN
	sq_pushobject(VM, val.m_obj);
	_SETVALUE_STR_END
}

bool sqObject::setValue(const SQChar *key, int n)
{
	_SETVALUE_STR_BEGIN
	sq_pushinteger(VM, n);
	_SETVALUE_STR_END
}

bool sqObject::setValue(const SQChar *key, float f)
{
	_SETVALUE_STR_BEGIN
	sq_pushfloat(VM, f);
	_SETVALUE_STR_END
}

bool sqObject::setValue(const SQChar *key, const SQChar *s)
{
	_SETVALUE_STR_BEGIN
	sq_pushstring(VM, s,-1);
	_SETVALUE_STR_END
}

bool sqObject::setValue(const SQChar *key, bool b)
{
	_SETVALUE_STR_BEGIN
	sq_pushbool(VM, b);
	_SETVALUE_STR_END
}

// === BEGIN User Pointer, User Data ===

bool sqObject::setUserPointer(const SQChar * key, SQUserPointer up)
{
	_SETVALUE_STR_BEGIN
	sq_pushuserpointer(VM, up);
	_SETVALUE_STR_END
}

SQUserPointer sqObject::getUserPointer(const SQChar * key)
{
	SQUserPointer ret = NULL;
	if (getSlot(key)) {
		sq_getuserpointer(VM,-1,&ret);
		sq_pop(VM, 1);
	} // if
	sq_pop(VM, 1);
	return ret;
}

bool sqObject::setUserPointer(int key, SQUserPointer up)
{
	_SETVALUE_INT_BEGIN
		sq_pushuserpointer(VM, up);
	_SETVALUE_INT_END
}

SQUserPointer sqObject::getUserPointer(int key)
{
	SQUserPointer ret = NULL;
	if (getSlot(key)) {
		sq_getuserpointer(VM,-1,&ret);
		sq_pop(VM, 1);
	} // if
	sq_pop(VM, 1);
	return ret;
}

// === User Data ===

bool sqObject::newUserData(const SQChar * key, int size, SQUserPointer * typetag)
{
	_SETVALUE_STR_BEGIN
		sq_newuserdata(VM, size);
	if (typetag) {
		sq_settypetag(VM,-1, typetag);
	} // if
	_SETVALUE_STR_END
}

bool sqObject::getUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
{
	bool ret = false;
	if (getSlot(key)) {
		sq_getuserdata(VM,-1, data, typetag);
		sq_pop(VM, 1);
		ret = true;
	} // if
	sq_pop(VM, 1);
	return ret;
}

bool sqObject::rawGetUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
{
	bool ret = false;
	if (rawGetSlot(key)) {
		sq_getuserdata(VM,-1, data, typetag);
		sq_pop(VM, 1);
		ret = true;
	} // if
	sq_pop(VM, 1);
	return ret;
}

// === END User Pointer ===

// === BEGIN Arrays ===

bool sqObject::arrayResize(int newSize)
{
	//  int top = sq_gettop(VM);
	sq_pushobject(VM, getObjectHandle());
	bool res = sq_arrayresize(VM,-1, newSize) == SQ_OK;
	sq_pop(VM, 1);
	//  sq_settop(VM, top);
	return res;
}

bool sqObject::arrayExtend(int amount)
{
	int newLen = len()+amount;
	return arrayResize(newLen);
}

bool sqObject::arrayReverse(void)
{
	sq_pushobject(VM, getObjectHandle());
	bool res = sq_arrayreverse(VM,-1) == SQ_OK;
	sq_pop(VM, 1);
	return res;
}

sqObject sqObject::arrayPop(SQBool returnPoppedVal)
{
	sqObject ret;
	int top = sq_gettop(VM);
	sq_pushobject(VM, getObjectHandle());

	if (sq_arraypop(VM,-1, returnPoppedVal) == SQ_OK) {
		if (returnPoppedVal) {
			ret.attachToStackObject(VM, -1);
		} // if
	} // if

	sq_settop(VM, top);
	return ret;
}

// === END Arrays ===

SQObjectType sqObject::getType() const
{
	return m_obj._type;
}

bool sqObject::getSlot(int key) const
{
	sq_pushobject(VM, m_obj);
	sq_pushinteger(VM, key);

	if (SQ_SUCCEEDED(sq_get(VM,-2))) {
		return true;
	}

	return false;
}


sqObject sqObject::getValue(int key)const
{
	sqObject ret;

	if (getSlot(key)) {
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

float sqObject::getFloat(int key) const
{
	float ret = 0.0f;

	if (getSlot(key)) {
		sq_getfloat(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

int sqObject::getInt(int key) const
{
	int ret = 0;

	if (getSlot(key)) {
		sq_getinteger(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

const SQChar *sqObject::getString(int key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

bool sqObject::getBool(int key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret ? true : false;
}

bool sqObject::exists(const SQChar *key) const
{
	if (getSlot(key)) {
		sq_pop(VM, 2);
		return true;
	} else {
		sq_pop(VM, 1);
		return false;
	}
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool sqObject::getSlot(const SQChar *name) const
{
	sq_pushobject(VM, m_obj);
	sq_pushstring(VM, name,-1);
	if (SQ_SUCCEEDED(sq_get(VM,-2))) {
		return true;
	}

	return false;
}

bool sqObject::rawGetSlot(const SQChar *name) const {
	sq_pushobject(VM, m_obj);
	sq_pushstring(VM, name,-1);
	if (SQ_SUCCEEDED(sq_rawget(VM,-2))) {
		return true;
	}
	return false;
} // sqObject::rawGetSlot

sqObject sqObject::getValue(const SQChar *key)const
{
	sqObject ret;
	if (getSlot(key)) {
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

float sqObject::getFloat(const SQChar *key) const
{
	float ret = 0.0f;
	if (getSlot(key)) {
		sq_getfloat(VM,-1,&ret);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

int sqObject::getInt(const SQChar *key) const
{
	int ret = 0;
	if (getSlot(key)) {
		sq_getinteger(VM,-1,&ret);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

const SQChar *sqObject::getString(const SQChar *key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

bool sqObject::getBool(const SQChar *key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret ? true : false;
}

SQUserPointer sqObject::getInstanceUP(SQUserPointer tag) const
{
	SQUserPointer up;
	sq_pushobject(VM, m_obj);

	if (SQ_FAILED(sq_getinstanceup(VM,-1,(SQUserPointer*)&up, tag))) {
		sq_reseterror(VM);
		up = NULL;
	}

	sq_pop(VM, 1);
	return up;
}

bool sqObject::setInstanceUP(SQUserPointer up)
{
	if (!sq_isinstance(m_obj))
		return false;

	sq_pushobject(VM, m_obj);
	sq_setinstanceup(VM,-1, up);
	sq_pop(VM, 1);
	return true;
}

sqObject sqObject::getAttributes(const SQChar *key)
{
	sqObject ret;
	int top = sq_gettop(VM);
	sq_pushobject(VM, m_obj);

	if (key)
		sq_pushstring(VM, key,-1);
	else
		sq_pushnull(VM);

	if (SQ_SUCCEEDED(sq_getattributes(VM,-2))) {
		ret.attachToStackObject(VM, -1);
	}

	sq_settop(VM, top);
	return ret;
}

bool sqObject::beginIteration()
{
	if (!sq_istable(m_obj) && !sq_isarray(m_obj) && !sq_isclass(m_obj))
		return false;

	sq_pushobject(VM, m_obj);
	sq_pushnull(VM);
	return true;
}

bool sqObject::next(sqObject &key, sqObject &val)
{
	if (SQ_SUCCEEDED(sq_next(VM, -2))) {
		key.attachToStackObject(VM, -2);
		val.attachToStackObject(VM, -1);
		sq_pop(VM, 2);
		return true;
	}

	return false;
}

void sqObject::endIteration()
{
	sq_pop(VM, 2);
}

bool sqObject::getTypeTag(SQUserPointer * typeTag)
{
	if (SQ_SUCCEEDED(sq_getobjtypetag(&m_obj, typeTag))) {
		return true;
	}

	return false;
} // sqObject::getTypeTag

const SQChar * sqObject::getTypeName(const SQChar * key)
{
#if 0
	// This version will work even if SQ_SUPPORT_INSTANCE_TYPE_INFO is not enabled.
	SqPlus::ScriptStringVar256 varNameTag;
	SqPlus::getVarNameTag(varNameTag, sizeof(varNameTag), key);
	SQUserPointer data=0;
	if (!RawGetUserData(varNameTag,&data)) {
		return NULL;
	} // if
	SqPlus::VarRefPtr vr = (SqPlus::VarRefPtr)data;
	return vr->varType->GetTypeName();
#else // This version will only work if SQ_SUPPORT_INSTANCE_TYPE_INFO is enabled.
	sqObject so = getValue(key);
	if (so.isNull()) return NULL;
	return so.getTypeName();
#endif
} // sqObject::getTypeName

const SQChar * sqObject::getTypeName(int key)
{
	sqObject so = getValue(key);
	if (so.isNull()) return NULL;
	return so.getTypeName();
} // sqObject::getTypeName

const SQChar * sqObject::getTypeName(void)
{
#if 0 // TODO
	SQUserPointer typeTag=NULL;
	if (SQ_SUCCEEDED(sq_getobjtypetag(&_o,&typeTag))) {
		sqObject typeTable = SquirrelVM::GetRootTable().GetValue(SQ_PLUS_TYPE_TABLE);
		if (typeTable.IsNull()) {
			return NULL; // Not compiled with SQ_SUPPORT_INSTANCE_TYPE_INFO enabled.
		} // if
		return typeTable.GetString(int((size_t)typeTag));
	} // if
#endif
	return NULL;
} // sqObject::getTypeName

sqObject sqObject::getBase(void)
{
	sqObject ret;
	sq_pushobject(VM, m_obj);
	sq_getbase(VM, -1);
	ret.attachToStackObject(VM, -1);
	sq_pop(VM, 2);

	return ret;
}

const SQChar* sqObject::toString() const
{
	return sq_objtostring(&m_obj);
}

SQInteger sqObject::toInteger() const
{
	return sq_objtointeger(&m_obj);
}

SQFloat sqObject::toFloat() const
{
	return sq_objtofloat(&m_obj);
}

bool sqObject::toBool() const
{
	//<<FIXME>>
	return m_obj._unVal.nInteger ? true : false;
}

void sqObject::getVariant(Variant &result) const
{
	switch (m_obj._type) {
	case OT_NULL:
		return;
	case OT_INTEGER:
		result.init(Variant::kInt, &m_obj._unVal.nInteger);
		return;
	case OT_FLOAT:
		result.init(Variant::kFloat, &m_obj._unVal.fFloat);
		return;
	case OT_BOOL:
		result.init(Variant::kBool, &m_obj._unVal.nInteger);
		return;
	case OT_STRING:
		{
			result.init(Variant::kString);
			result.ref<String>() = sq_objtostring(&m_obj);
		}
		return;
	case OT_TABLE:
	case OT_ARRAY:
	case OT_USERDATA:
	case OT_CLOSURE:
	case OT_NATIVECLOSURE:
	case OT_GENERATOR:
	case OT_USERPOINTER:
	case OT_THREAD:
	case OT_FUNCPROTO:
	case OT_CLASS:
	case OT_WEAKREF:
		{
			result.init(Variant::kScriptValue);
			result.ref<ScriptValue>().getSquirrelObject() = m_obj;
		}
		return;
	case OT_INSTANCE:
		{
			SquirrelClassDecl *typetag = 0;
			void *userdata = getInstanceUP(0);
			sq_getobjtypetag(&m_obj, (SQUserPointer *)&typetag);

			if (typetag == &__Vector3_decl) {
				result.init(Variant::kVector3, userdata, Variant::InitRef);
			} else if (typetag == &__Color3_decl) {
				result.init(Variant::kColor3, userdata, Variant::InitRef);
			} else if (typetag == &__Point_decl) {
				result.init(Variant::kPoint, userdata, Variant::InitRef);
			} else if (typetag == &__Rect_decl) {
				result.init(Variant::kRect, userdata, Variant::InitRef);
			} else if (typetag == &__Matrix_decl) {
				result.init(Variant::kMatrix, userdata, Variant::InitRef);
			} else if (typetag == &__Object_c_decl) {
				result.init(Variant::kObject, userdata, Variant::InitRef);
			}
		}
		return;
	}
	return;
}

void StackHandler::getRawData(int idx, Variant &result)
{
	sqObject obj;
	obj.attachToStackObject(v, idx);
	obj.getVariant(result);
	return;

	// make sure result is emtpy first
	AX_ASSERT(result.getTypeId() == Variant::kVoid);

	HSQOBJECT t;
	sq_getstackobj(VM, idx, &t);

	switch (t._type) {
	case OT_NULL:
		return;
	case OT_INTEGER:
		result.init(Variant::kInt, &t._unVal.nInteger);
		return;
	case OT_FLOAT:
		result.init(Variant::kFloat, &t._unVal.fFloat);
		return;
	case OT_BOOL:
		result.init(Variant::kBool, &t._unVal.nInteger);
		return;
	case OT_STRING:
		{
			result.init(Variant::kString);
			result.ref<String>() = sq_objtostring(&t);
		}
		return;
	case OT_TABLE:
	case OT_ARRAY:
	case OT_USERDATA:
	case OT_CLOSURE:
	case OT_NATIVECLOSURE:
	case OT_GENERATOR:
	case OT_USERPOINTER:
	case OT_THREAD:
	case OT_FUNCPROTO:
	case OT_CLASS:
	case OT_WEAKREF:
		{
			result.init(Variant::kScriptValue);
			result.ref<ScriptValue>().getSquirrelObject() = t;
		}
		return;
	case OT_INSTANCE:
		{
			SquirrelClassDecl *typetag;
			void *userdata;
			sq_getobjtypetag(&t, (SQUserPointer *)&typetag);
			sq_getinstanceup(v, idx, &userdata, typetag);

			if (typetag == &__Vector3_decl) {
				result.init(Variant::kVector3, userdata, Variant::InitRef);
			} else if (typetag == &__Color3_decl) {
				result.init(Variant::kColor3, userdata, Variant::InitRef);
			} else if (typetag == &__Point_decl) {
				result.init(Variant::kPoint, userdata, Variant::InitRef);
			} else if (typetag == &__Rect_decl) {
				result.init(Variant::kRect, userdata, Variant::InitRef);
			} else if (typetag == &__Matrix_decl) {
				result.init(Variant::kMatrix, userdata, Variant::InitRef);
			} else if (typetag == &__Object_c_decl) {
				result.init(Variant::kObject, userdata, Variant::InitRef);
			}
		}
		return;
	}
	return;
}

int StackHandler::retRawData(const ConstRef &arg)
{
	if (arg.getTypeId() == Variant::kVoid)
		return 0;

	switch (arg.getTypeId()) {
	case Variant::kVoid:
		return 0;
	case Variant::kBool:
		sq_pushbool(v, arg.ref<bool>());
		return 1;
	case Variant::kInt:
		sq_pushinteger(v, arg.ref<int>());
		return 1;
	case Variant::kFloat:
		sq_pushfloat(v, arg.ref<float>());
		return 1;
	case Variant::kString:
		sq_pushstring(v, arg.ref<String>().c_str(), arg.ref<String>().size());
		return 1;
	case Variant::kObject:
		return 0;
	case Variant::kVector3:
		push_Vector3(v, arg.ref<Vector3>());
		return 1;
	case Variant::kColor3:
		push_Color3(v, arg.ref<Color3>());
		return 1;
	case Variant::kPoint:
		push_Point(v, arg.ref<Point>());
		return 1;
	case Variant::kRect:
		push_Rect(v, arg.ref<Rect>());
		return 1;
	case Variant::kMatrix:
		push_Matrix(v, arg.ref<Matrix>());
		return 1;
	case Variant::kTable:
		return 0;
	case Variant::kScriptValue:
		sq_pushobject(v, arg.ref<ScriptValue>().getSquirrelObject());
		return 1;
	}

	return 0;
}


AX_END_NAMESPACE
