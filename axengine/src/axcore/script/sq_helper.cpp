#include "script_p.h"

AX_BEGIN_NAMESPACE
SquirrelError::SquirrelError(SquirrelVM *vm) 
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



SquirrelObject::SquirrelObject(void)
{
	sq_resetobject(&_o);
}

SquirrelObject::~SquirrelObject()
{
	reset();
}

SquirrelObject::SquirrelObject(const SquirrelObject &o)
{
	_o = o._o;
	sq_addref(VM,&_o);
}

SquirrelObject::SquirrelObject(HSQOBJECT o)
{
	_o = o;
	sq_addref(VM,&_o);
}

void SquirrelObject::reset(void)
{
	if (VM)
		sq_release(VM,&_o);
	else if ( _o._type!=OT_NULL && _o._unVal.pRefCounted )
		printf( "SquirrelObject::~SquirrelObject - Cannot release\n" ); 

	sq_resetobject(&_o);
} // SquirrelObject::reset

SquirrelObject SquirrelObject::clone()
{
	SquirrelObject ret;
	if (getType() == OT_TABLE || getType() == OT_ARRAY) {
		sq_pushobject(VM, _o);
		sq_clone(VM, -1);
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 2);
	}

	return ret;
}


bool SquirrelObject::operator == (const SquirrelObject &o)
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

bool SquirrelObject::compareUserPointer( const SquirrelObject &o )
{
	if ( _o._type == o.getObjectHandle()._type )
		if ( _o._unVal.pUserPointer == o.getObjectHandle()._unVal.pUserPointer )
			return true;

	return false;
}

void SquirrelObject::arrayAppend(const SquirrelObject &o)
{
	if (sq_isarray(_o)) {
		sq_pushobject(VM, _o);
		sq_pushobject(VM, o._o);
		sq_arrayappend(VM, -2);
		sq_pop(VM, 1);
	}
}

void SquirrelObject::attachToStackObject(HSQUIRRELVM vm, int idx)
{
	HSQOBJECT t;
	sq_getstackobj(vm, idx, &t);
	sq_addref(vm, &t);
	sq_release(vm, &_o);
	_o = t;
}

bool SquirrelObject::setDelegate(SquirrelObject &obj)
{
	if (obj.getType() == OT_TABLE || obj.getType() == OT_NULL) {
			switch(_o._type) {
			case OT_USERDATA:
			case OT_TABLE:
				sq_pushobject(VM, _o);
				sq_pushobject(VM, obj._o);
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

SquirrelObject SquirrelObject::getDelegate()
{
	SquirrelObject ret;
	if (_o._type == OT_TABLE || _o._type == OT_USERDATA)
	{
		int top = sq_gettop(VM);
		sq_pushobject(VM, _o);
		sq_getdelegate(VM, -1);
		ret.attachToStackObject(VM, -1);
		sq_settop(VM, top);
		//		sq_pop(VM, 2);
	}
	return ret;
}

bool SquirrelObject::isNull() const
{
	return sq_isnull(_o);
}

bool SquirrelObject::isNumeric() const
{
	return sq_isnumeric(_o);
}

int SquirrelObject::len() const
{
	int ret = 0;
	if (sq_isarray(_o) || sq_istable(_o) || sq_isstring(_o)) {
		sq_pushobject(VM, _o);
		ret = sq_getsize(VM,-1);
		sq_pop(VM, 1);
	}
	return ret;
}

#define _SETVALUE_INT_BEGIN \
	bool ret = false; \
	int top = sq_gettop(VM); \
	sq_pushobject(VM, _o); \
	sq_pushinteger(VM, key);

#define _SETVALUE_INT_END \
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) { \
	ret = true; \
	} \
	sq_settop(VM, top); \
	return ret;

bool SquirrelObject::setValue(int key, const SquirrelObject &val)
{
	_SETVALUE_INT_BEGIN
		sq_pushobject(VM, val._o);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, int n)
{
	_SETVALUE_INT_BEGIN
		sq_pushinteger(VM, n);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, float f)
{
	_SETVALUE_INT_BEGIN
		sq_pushfloat(VM, f);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, const SQChar *s)
{
	_SETVALUE_INT_BEGIN
		sq_pushstring(VM, s,-1);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, bool b)
{
	_SETVALUE_INT_BEGIN
		sq_pushbool(VM, b);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(const SquirrelObject &key, const SquirrelObject &val)
{
	bool ret = false;
	int top = sq_gettop(VM);
	sq_pushobject(VM, _o);
	sq_pushobject(VM, key._o);
	sq_pushobject(VM, val._o);
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) {
		ret = true;
	}
	sq_settop(VM, top);
	return ret;
}

#define _SETVALUE_STR_BEGIN \
	bool ret = false; \
	int top = sq_gettop(VM); \
	sq_pushobject(VM, _o); \
	sq_pushstring(VM, key,-1);

#define _SETVALUE_STR_END \
	if (SQ_SUCCEEDED(sq_rawset(VM,-3))) { \
	ret = true; \
	} \
	sq_settop(VM, top); \
	return ret;

bool SquirrelObject::setValue(const SQChar *key, const SquirrelObject &val)
{
	_SETVALUE_STR_BEGIN
	sq_pushobject(VM, val._o);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, int n)
{
	_SETVALUE_STR_BEGIN
	sq_pushinteger(VM, n);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, float f)
{
	_SETVALUE_STR_BEGIN
	sq_pushfloat(VM, f);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, const SQChar *s)
{
	_SETVALUE_STR_BEGIN
	sq_pushstring(VM, s,-1);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, bool b)
{
	_SETVALUE_STR_BEGIN
	sq_pushbool(VM, b);
	_SETVALUE_STR_END
}

// === BEGIN User Pointer, User Data ===

bool SquirrelObject::setUserPointer(const SQChar * key, SQUserPointer up)
{
	_SETVALUE_STR_BEGIN
	sq_pushuserpointer(VM, up);
	_SETVALUE_STR_END
}

SQUserPointer SquirrelObject::getUserPointer(const SQChar * key)
{
	SQUserPointer ret = NULL;
	if (getSlot(key)) {
		sq_getuserpointer(VM,-1,&ret);
		sq_pop(VM, 1);
	} // if
	sq_pop(VM, 1);
	return ret;
}

bool SquirrelObject::setUserPointer(int key, SQUserPointer up)
{
	_SETVALUE_INT_BEGIN
		sq_pushuserpointer(VM, up);
	_SETVALUE_INT_END
}

SQUserPointer SquirrelObject::getUserPointer(int key)
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

bool SquirrelObject::newUserData(const SQChar * key, int size, SQUserPointer * typetag)
{
	_SETVALUE_STR_BEGIN
		sq_newuserdata(VM, size);
	if (typetag) {
		sq_settypetag(VM,-1, typetag);
	} // if
	_SETVALUE_STR_END
}

bool SquirrelObject::getUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
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

bool SquirrelObject::rawGetUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
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

bool SquirrelObject::arrayResize(int newSize)
{
	//  int top = sq_gettop(VM);
	sq_pushobject(VM, getObjectHandle());
	bool res = sq_arrayresize(VM,-1, newSize) == SQ_OK;
	sq_pop(VM, 1);
	//  sq_settop(VM, top);
	return res;
}

bool SquirrelObject::arrayExtend(int amount)
{
	int newLen = len()+amount;
	return arrayResize(newLen);
}

bool SquirrelObject::arrayReverse(void)
{
	sq_pushobject(VM, getObjectHandle());
	bool res = sq_arrayreverse(VM,-1) == SQ_OK;
	sq_pop(VM, 1);
	return res;
}

SquirrelObject SquirrelObject::arrayPop(SQBool returnPoppedVal)
{
	SquirrelObject ret;
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

SQObjectType SquirrelObject::getType()
{
	return _o._type;
}

bool SquirrelObject::getSlot(int key) const
{
	sq_pushobject(VM, _o);
	sq_pushinteger(VM, key);

	if (SQ_SUCCEEDED(sq_get(VM,-2))) {
		return true;
	}

	return false;
}


SquirrelObject SquirrelObject::getValue(int key)const
{
	SquirrelObject ret;

	if (getSlot(key)) {
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

float SquirrelObject::getFloat(int key) const
{
	float ret = 0.0f;

	if (getSlot(key)) {
		sq_getfloat(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

int SquirrelObject::getInt(int key) const
{
	int ret = 0;

	if (getSlot(key)) {
		sq_getinteger(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

const SQChar *SquirrelObject::getString(int key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

bool SquirrelObject::getBool(int key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret ? true : false;
}

bool SquirrelObject::exists(const SQChar *key) const
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

bool SquirrelObject::getSlot(const SQChar *name) const
{
	sq_pushobject(VM, _o);
	sq_pushstring(VM, name,-1);
	if (SQ_SUCCEEDED(sq_get(VM,-2))) {
		return true;
	}

	return false;
}

bool SquirrelObject::rawGetSlot(const SQChar *name) const {
	sq_pushobject(VM, _o);
	sq_pushstring(VM, name,-1);
	if (SQ_SUCCEEDED(sq_rawget(VM,-2))) {
		return true;
	}
	return false;
} // SquirrelObject::rawGetSlot

SquirrelObject SquirrelObject::getValue(const SQChar *key)const
{
	SquirrelObject ret;
	if (getSlot(key)) {
		ret.attachToStackObject(VM, -1);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

float SquirrelObject::getFloat(const SQChar *key) const
{
	float ret = 0.0f;
	if (getSlot(key)) {
		sq_getfloat(VM,-1,&ret);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

int SquirrelObject::getInt(const SQChar *key) const
{
	int ret = 0;
	if (getSlot(key)) {
		sq_getinteger(VM,-1,&ret);
		sq_pop(VM, 1);
	}
	sq_pop(VM, 1);
	return ret;
}

const SQChar *SquirrelObject::getString(const SQChar *key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret;
}

bool SquirrelObject::getBool(const SQChar *key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(VM,-1,&ret);
		sq_pop(VM, 1);
	}

	sq_pop(VM, 1);
	return ret?true:false;
}

SQUserPointer SquirrelObject::getInstanceUP(SQUserPointer tag) const
{
	SQUserPointer up;
	sq_pushobject(VM, _o);

	if (SQ_FAILED(sq_getinstanceup(VM,-1,(SQUserPointer*)&up, tag))) {
		sq_reseterror(VM);
		up = NULL;
	}

	sq_pop(VM, 1);
	return up;
}

bool SquirrelObject::setInstanceUP(SQUserPointer up)
{
	if (!sq_isinstance(_o))
		return false;

	sq_pushobject(VM, _o);
	sq_setinstanceup(VM,-1, up);
	sq_pop(VM, 1);
	return true;
}

SquirrelObject SquirrelObject::getAttributes(const SQChar *key)
{
	SquirrelObject ret;
	int top = sq_gettop(VM);
	sq_pushobject(VM, _o);

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

bool SquirrelObject::beginIteration()
{
	if (!sq_istable(_o) && !sq_isarray(_o) && !sq_isclass(_o))
		return false;

	sq_pushobject(VM, _o);
	sq_pushnull(VM);
	return true;
}

bool SquirrelObject::next(SquirrelObject &key, SquirrelObject &val)
{
	if (SQ_SUCCEEDED(sq_next(VM, -2))) {
		key.attachToStackObject(VM, -2);
		val.attachToStackObject(VM, -1);
		sq_pop(VM, 2);
		return true;
	}

	return false;
}

bool SquirrelObject::getTypeTag(SQUserPointer * typeTag)
{
	if (SQ_SUCCEEDED(sq_getobjtypetag(&_o, typeTag))) {
		return true;
	}

	return false;
} // SquirrelObject::getTypeTag

const SQChar * SquirrelObject::getTypeName(const SQChar * key)
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
	SquirrelObject so = getValue(key);
	if (so.isNull()) return NULL;
	return so.getTypeName();
#endif
} // SquirrelObject::getTypeName

const SQChar * SquirrelObject::getTypeName(int key)
{
	SquirrelObject so = getValue(key);
	if (so.isNull()) return NULL;
	return so.getTypeName();
} // SquirrelObject::getTypeName

const SQChar * SquirrelObject::getTypeName(void)
{
#if 0 // TODO
	SQUserPointer typeTag=NULL;
	if (SQ_SUCCEEDED(sq_getobjtypetag(&_o,&typeTag))) {
		SquirrelObject typeTable = SquirrelVM::GetRootTable().GetValue(SQ_PLUS_TYPE_TABLE);
		if (typeTable.IsNull()) {
			return NULL; // Not compiled with SQ_SUPPORT_INSTANCE_TYPE_INFO enabled.
		} // if
		return typeTable.GetString(int((size_t)typeTag));
	} // if
#endif
	return NULL;
} // SquirrelObject::getTypeName

SquirrelObject SquirrelObject::getBase(void)
{
	SquirrelObject ret;
	sq_pushobject(VM, _o);
	sq_getbase(VM, -1);
	ret.attachToStackObject(VM, -1);
	sq_pop(VM, 2);

	return ret;
}

const SQChar* SquirrelObject::toString()
{
	return sq_objtostring(&_o);
}

SQInteger SquirrelObject::toInteger()
{
	return sq_objtointeger(&_o);
}

SQFloat SquirrelObject::toFloat()
{
	return sq_objtofloat(&_o);
}

bool SquirrelObject::toBool()
{
	//<<FIXME>>
	return _o._unVal.nInteger ? true : false;
}

void SquirrelObject::endIteration()
{
	sq_pop(VM, 2);
}

void StackHandler::getRawData( int idx, Variant &result)
{
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
