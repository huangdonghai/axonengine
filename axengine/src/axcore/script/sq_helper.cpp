#include "script_p.h"

AX_BEGIN_NAMESPACE

static SQRESULT _loadfile(HSQUIRRELVM v, const SQChar *filename, SQBool printerror)
{
	String name = "scripts/";
	name += filename;

	char *filebuf;
	size_t filesize = g_fileSystem->readFile(name, (void **)&filebuf);

	if (!filesize && !filebuf) {
		return sq_throwerror(v, _SC("cannot open the file"));
	}

	if (SQ_SUCCEEDED(sq_compilebuffer(v, filebuf, filesize, filename, printerror))) {
		return SQ_OK;
	} else {
		return SQ_ERROR;
	}

}

static SQRESULT _dofile(HSQUIRRELVM v,const SQChar *filename,SQBool retval,SQBool printerror)
{
	if (SQ_SUCCEEDED(_loadfile(v,filename,printerror))) {
		sq_push(v,-2);
		if(SQ_SUCCEEDED(sq_call(v,1,retval,SQTrue))) {
			sq_remove(v,retval?-2:-1); //removes the closure
			return 1;
		}
		sq_pop(v,1); //removes the closure
	}
	return SQ_ERROR;
}

static SQInteger loadfile(HSQUIRRELVM v)
{
	const SQChar *filename;
	SQBool printerror = SQFalse;
	sq_getstring(v,2,&filename);

	if (sq_gettop(v) >= 3) {
		sq_getbool(v,3,&printerror);
	}

	if (SQ_SUCCEEDED(_loadfile(v,filename,printerror)))
		return 1;

	return SQ_ERROR; // propagates the error
}

static SQInteger dofile(HSQUIRRELVM v)
{
	const SQChar *filename;
	SQBool printerror = SQFalse;
	sq_getstring(v,2,&filename);

	if (sq_gettop(v) >= 3) {
		sq_getbool(v,3,&printerror);
	}

	sq_push(v,1); //repush the this
	if (SQ_SUCCEEDED(_dofile(v,filename,SQTrue,printerror)))
		return 1;

	return SQ_ERROR; //propagates the error
}

static SQInteger registerClass(HSQUIRRELVM v)
{
	SquirrelObject so; so.attachToStackObject(2);
	return SQ_ERROR;
}

HSQUIRRELVM SquirrelVM::ms_rootVM = 0;

static SQRegFunction ax_funcs[] = {
	{ "loadfile", &loadfile, -2, _SC(".sb") },
	{ "dofile", &dofile, -2, _SC(".sb") },
	{ "AX_REGISTER_CLASS", &registerClass, -2, _SC(".y") },
	{ 0, 0 }
};

SquirrelError::SquirrelError(SquirrelVM *vm) 
{
	const SQChar *s;
	sq_getlasterror(vm->m_vm);
	sq_getstring(vm->m_vm,-1,&s);
	if (s) {
		desc = s;
	}
	else {
		desc = _SC("unknown error");
	}
}


SquirrelVM::SquirrelVM()
{
	if (ms_rootVM) {
		Errorf("Already create root vm");
	}

	ms_rootVM = sq_open(1024);
	AX_ASSERT(ms_rootVM);
	sq_setprintfunc(ms_rootVM, SquirrelVM::printFunc);
	sq_pushroottable(ms_rootVM);
	sqstd_register_iolib(ms_rootVM);
	sqstd_register_bloblib(ms_rootVM);
	sqstd_register_mathlib(ms_rootVM);
	sqstd_register_stringlib(ms_rootVM);
#ifdef SQPLUS_SQUIRRELVM_WITH_SYSTEMLIB        
	sqstd_register_systemlib(ms_rootVM);
#endif        
	sqstd_seterrorhandlers(ms_rootVM);
	//TODO error handler, compiler error handler
	sq_pop(ms_rootVM, 1);

	// register or replace functions
	int top = sq_gettop(ms_rootVM);
	sq_pushroottable(ms_rootVM);

	int i = 0;
	while (ax_funcs[i].name != 0) {
		SQRegFunction &f = ax_funcs[i];
		sq_pushstring(ms_rootVM, f.name, -1);
		sq_newclosure(ms_rootVM, f.f, 0);
		sq_setparamscheck(ms_rootVM, f.nparamscheck, f.typemask);
		sq_setnativeclosurename(ms_rootVM, -1, f.name);
		sq_createslot(ms_rootVM, -3);
		i++;
	}
	sq_settop(ms_rootVM, top);

	createObjectClosure();

	m_vm = ms_rootVM;
}

SquirrelVM::SquirrelVM(HSQUIRRELVM vm)
{

}

SquirrelVM::~SquirrelVM()
{

}

void SquirrelVM::printFunc(HSQUIRRELVM v, const SQChar* s, ...)
{
	static char temp[2048];
	va_list vl;
	va_start(vl, s);
	StringUtil::vsnprintf(temp, ArraySize(temp), s, vl);
	Printf("%s\n", temp);
	va_end(vl);
}

SquirrelObject SquirrelVM::compileFile(const SQChar *filename)
{
	if (SQ_SUCCEEDED(_loadfile(m_vm, filename, 1))) {
		SquirrelObject ret;
		ret.attachToStackObject(-1);
		sq_pop(m_vm, 1);
		return ret;
	}
	throw SquirrelError(this);
}

SquirrelObject SquirrelVM::compileBuffer(const SQChar *s, const SQChar *debugInfo/*=_SC("console_buffer")*/)
{
	SquirrelObject ret;
	if (SQ_SUCCEEDED(sq_compilebuffer(m_vm, s, (int)scstrlen(s)*sizeof(SQChar), debugInfo, 1))) {
		ret.attachToStackObject(-1);
		sq_pop(m_vm, 1);
		return ret;
	}
	throw SquirrelError(this);
}

SquirrelObject SquirrelVM::runBytecode(const SquirrelObject &bytecode, SquirrelObject *_this /*= NULL*/)
{
	SquirrelObject ret;
	sq_pushobject(m_vm, bytecode._o);
	if (_this) {
		sq_pushobject(m_vm, _this->_o);
	} else {
		sq_pushroottable(m_vm);
	}
	if (SQ_SUCCEEDED(sq_call(m_vm, 1, SQTrue, SQTrue))) {
		ret.attachToStackObject(-1);
		sq_pop(m_vm, 2);
		return ret;
	}
	sq_pop(m_vm, 1);
	throw SquirrelError(this);
}

SquirrelObject SquirrelVM::runFile(const SQChar *s, SquirrelObject *_this /*= NULL*/)
{
	SquirrelObject bytecode = compileFile(s);
	return runBytecode(bytecode, _this);
}

SquirrelObject SquirrelVM::runBuffer(const SQChar *s, SquirrelObject *_this /*= NULL*/)
{
	SquirrelObject bytecode = compileBuffer(s);
	return runBytecode(bytecode, _this);
}

SquirrelObject SquirrelVM::createClosure( const SQChar *name, SQFUNCTION f, SQInteger nparamscheck, const SQChar *typemask )
{
	sq_newclosure(ms_rootVM, f, 0);
	sq_setparamscheck(ms_rootVM, nparamscheck, typemask);
	sq_setnativeclosurename(ms_rootVM, -1, name);

	SquirrelObject sobj;
	sobj.attachToStackObject(-1);
	sq_pop(ms_rootVM, 1);
	return sobj;
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
	sq_addref(SquirrelVM::ms_rootVM,&_o);
}

SquirrelObject::SquirrelObject(HSQOBJECT o)
{
	_o = o;
	sq_addref(SquirrelVM::ms_rootVM,&_o);
}

void SquirrelObject::reset(void)
{
	if (SquirrelVM::ms_rootVM)
		sq_release(SquirrelVM::ms_rootVM,&_o);
	else if ( _o._type!=OT_NULL && _o._unVal.pRefCounted )
		printf( "SquirrelObject::~SquirrelObject - Cannot release\n" ); 

	sq_resetobject(&_o);
} // SquirrelObject::reset

SquirrelObject SquirrelObject::clone()
{
	SquirrelObject ret;
	if (getType() == OT_TABLE || getType() == OT_ARRAY) {
		sq_pushobject(SquirrelVM::ms_rootVM, _o);
		sq_clone(SquirrelVM::ms_rootVM,-1);
		ret.attachToStackObject(-1);
		sq_pop(SquirrelVM::ms_rootVM, 2);
	}

	return ret;
}

SquirrelObject & SquirrelObject::operator =(const SquirrelObject &o)
{
	//HSQOBJECT t;
	//t = o._o;
	//sq_addref(SquirrelVM::ms_rootVM,&t);
	sq_addref(SquirrelVM::ms_rootVM, (HSQOBJECT*)&o._o);
	sq_release(SquirrelVM::ms_rootVM, &_o);
	//_o = t;
	_o = o._o;
	return *this;
}

SquirrelObject & SquirrelObject::operator =(HSQOBJECT ho)
{
	sq_addref(SquirrelVM::ms_rootVM, &ho);
	sq_release(SquirrelVM::ms_rootVM, &_o);
	_o = ho;
	return *this;
}

SquirrelObject & SquirrelObject::operator =(int n)
{
	sq_pushinteger(SquirrelVM::ms_rootVM, n);
	attachToStackObject(-1);
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return *this;
}

SquirrelObject & SquirrelObject::operator =(HSQUIRRELVM v)
{
#if 0
	if ( v && SquirrelVM::m_rootVM ){
		SquirrelVM::m_rootVM->Push(v);
		AttachToStackObject(-1);
		sq_poptop(SquirrelVM::m_rootVM);
	}
	else Reset();
#endif
	// TODO
	return *this;
}

bool SquirrelObject::operator == (const SquirrelObject &o)
{
	bool cmp = false;
	HSQUIRRELVM v = SquirrelVM::ms_rootVM;
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
		sq_pushobject(SquirrelVM::ms_rootVM, _o);
		sq_pushobject(SquirrelVM::ms_rootVM, o._o);
		sq_arrayappend(SquirrelVM::ms_rootVM, -2);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}
}

void SquirrelObject::attachToStackObject(int idx)
{
	HSQOBJECT t;
	sq_getstackobj(SquirrelVM::ms_rootVM, idx, &t);
	sq_addref(SquirrelVM::ms_rootVM, &t);
	sq_release(SquirrelVM::ms_rootVM, &_o);
	_o = t;
}

bool SquirrelObject::setDelegate(SquirrelObject &obj)
{
	if (obj.getType() == OT_TABLE || obj.getType() == OT_NULL) {
			switch(_o._type) {
			case OT_USERDATA:
			case OT_TABLE:
				sq_pushobject(SquirrelVM::ms_rootVM, _o);
				sq_pushobject(SquirrelVM::ms_rootVM, obj._o);
				if (SQ_SUCCEEDED(sq_setdelegate(SquirrelVM::ms_rootVM,-2))) {
					sq_pop(SquirrelVM::ms_rootVM, 1);
					return true;
				}
				sq_pop(SquirrelVM::ms_rootVM, 1);
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
		int top = sq_gettop(SquirrelVM::ms_rootVM);
		sq_pushobject(SquirrelVM::ms_rootVM, _o);
		sq_getdelegate(SquirrelVM::ms_rootVM,-1);
		ret.attachToStackObject(-1);
		sq_settop(SquirrelVM::ms_rootVM, top);
		//		sq_pop(SquirrelVM::ms_rootVM, 2);
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
		sq_pushobject(SquirrelVM::ms_rootVM, _o);
		ret = sq_getsize(SquirrelVM::ms_rootVM,-1);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}
	return ret;
}

#define _SETVALUE_INT_BEGIN \
	bool ret = false; \
	int top = sq_gettop(SquirrelVM::ms_rootVM); \
	sq_pushobject(SquirrelVM::ms_rootVM, _o); \
	sq_pushinteger(SquirrelVM::ms_rootVM, key);

#define _SETVALUE_INT_END \
	if (SQ_SUCCEEDED(sq_rawset(SquirrelVM::ms_rootVM,-3))) { \
	ret = true; \
	} \
	sq_settop(SquirrelVM::ms_rootVM, top); \
	return ret;

bool SquirrelObject::setValue(int key, const SquirrelObject &val)
{
	_SETVALUE_INT_BEGIN
		sq_pushobject(SquirrelVM::ms_rootVM, val._o);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, int n)
{
	_SETVALUE_INT_BEGIN
		sq_pushinteger(SquirrelVM::ms_rootVM, n);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, float f)
{
	_SETVALUE_INT_BEGIN
		sq_pushfloat(SquirrelVM::ms_rootVM, f);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, const SQChar *s)
{
	_SETVALUE_INT_BEGIN
		sq_pushstring(SquirrelVM::ms_rootVM, s,-1);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(int key, bool b)
{
	_SETVALUE_INT_BEGIN
		sq_pushbool(SquirrelVM::ms_rootVM, b);
	_SETVALUE_INT_END
}

bool SquirrelObject::setValue(const SquirrelObject &key, const SquirrelObject &val)
{
	bool ret = false;
	int top = sq_gettop(SquirrelVM::ms_rootVM);
	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_pushobject(SquirrelVM::ms_rootVM, key._o);
	sq_pushobject(SquirrelVM::ms_rootVM, val._o);
	if (SQ_SUCCEEDED(sq_rawset(SquirrelVM::ms_rootVM,-3))) {
		ret = true;
	}
	sq_settop(SquirrelVM::ms_rootVM, top);
	return ret;
}

#define _SETVALUE_STR_BEGIN \
	bool ret = false; \
	int top = sq_gettop(SquirrelVM::ms_rootVM); \
	sq_pushobject(SquirrelVM::ms_rootVM, _o); \
	sq_pushstring(SquirrelVM::ms_rootVM, key,-1);

#define _SETVALUE_STR_END \
	if (SQ_SUCCEEDED(sq_rawset(SquirrelVM::ms_rootVM,-3))) { \
	ret = true; \
	} \
	sq_settop(SquirrelVM::ms_rootVM, top); \
	return ret;

bool SquirrelObject::setValue(const SQChar *key, const SquirrelObject &val)
{
	_SETVALUE_STR_BEGIN
	sq_pushobject(SquirrelVM::ms_rootVM, val._o);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, int n)
{
	_SETVALUE_STR_BEGIN
	sq_pushinteger(SquirrelVM::ms_rootVM, n);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, float f)
{
	_SETVALUE_STR_BEGIN
	sq_pushfloat(SquirrelVM::ms_rootVM, f);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, const SQChar *s)
{
	_SETVALUE_STR_BEGIN
	sq_pushstring(SquirrelVM::ms_rootVM, s,-1);
	_SETVALUE_STR_END
}

bool SquirrelObject::setValue(const SQChar *key, bool b)
{
	_SETVALUE_STR_BEGIN
	sq_pushbool(SquirrelVM::ms_rootVM, b);
	_SETVALUE_STR_END
}

// === BEGIN User Pointer, User Data ===

bool SquirrelObject::setUserPointer(const SQChar * key, SQUserPointer up)
{
	_SETVALUE_STR_BEGIN
	sq_pushuserpointer(SquirrelVM::ms_rootVM, up);
	_SETVALUE_STR_END
}

SQUserPointer SquirrelObject::getUserPointer(const SQChar * key)
{
	SQUserPointer ret = NULL;
	if (getSlot(key)) {
		sq_getuserpointer(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	} // if
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

bool SquirrelObject::setUserPointer(int key, SQUserPointer up)
{
	_SETVALUE_INT_BEGIN
		sq_pushuserpointer(SquirrelVM::ms_rootVM, up);
	_SETVALUE_INT_END
}

SQUserPointer SquirrelObject::getUserPointer(int key)
{
	SQUserPointer ret = NULL;
	if (getSlot(key)) {
		sq_getuserpointer(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	} // if
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

// === User Data ===

bool SquirrelObject::newUserData(const SQChar * key, int size, SQUserPointer * typetag)
{
	_SETVALUE_STR_BEGIN
		sq_newuserdata(SquirrelVM::ms_rootVM, size);
	if (typetag) {
		sq_settypetag(SquirrelVM::ms_rootVM,-1, typetag);
	} // if
	_SETVALUE_STR_END
}

bool SquirrelObject::getUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
{
	bool ret = false;
	if (getSlot(key)) {
		sq_getuserdata(SquirrelVM::ms_rootVM,-1, data, typetag);
		sq_pop(SquirrelVM::ms_rootVM, 1);
		ret = true;
	} // if
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

bool SquirrelObject::rawGetUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag)
{
	bool ret = false;
	if (rawGetSlot(key)) {
		sq_getuserdata(SquirrelVM::ms_rootVM,-1, data, typetag);
		sq_pop(SquirrelVM::ms_rootVM, 1);
		ret = true;
	} // if
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

// === END User Pointer ===

// === BEGIN Arrays ===

bool SquirrelObject::arrayResize(int newSize)
{
	//  int top = sq_gettop(SquirrelVM::ms_rootVM);
	sq_pushobject(SquirrelVM::ms_rootVM, getObjectHandle());
	bool res = sq_arrayresize(SquirrelVM::ms_rootVM,-1, newSize) == SQ_OK;
	sq_pop(SquirrelVM::ms_rootVM, 1);
	//  sq_settop(SquirrelVM::ms_rootVM, top);
	return res;
}

bool SquirrelObject::arrayExtend(int amount)
{
	int newLen = len()+amount;
	return arrayResize(newLen);
}

bool SquirrelObject::arrayReverse(void)
{
	sq_pushobject(SquirrelVM::ms_rootVM, getObjectHandle());
	bool res = sq_arrayreverse(SquirrelVM::ms_rootVM,-1) == SQ_OK;
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return res;
}

SquirrelObject SquirrelObject::arrayPop(SQBool returnPoppedVal)
{
	SquirrelObject ret;
	int top = sq_gettop(SquirrelVM::ms_rootVM);
	sq_pushobject(SquirrelVM::ms_rootVM, getObjectHandle());

	if (sq_arraypop(SquirrelVM::ms_rootVM,-1, returnPoppedVal) == SQ_OK) {
		if (returnPoppedVal) {
			ret.attachToStackObject(-1);
		} // if
	} // if

	sq_settop(SquirrelVM::ms_rootVM, top);
	return ret;
}

// === END Arrays ===

SQObjectType SquirrelObject::getType()
{
	return _o._type;
}

bool SquirrelObject::getSlot(int key) const
{
	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_pushinteger(SquirrelVM::ms_rootVM, key);

	if (SQ_SUCCEEDED(sq_get(SquirrelVM::ms_rootVM,-2))) {
		return true;
	}

	return false;
}


SquirrelObject SquirrelObject::getValue(int key)const
{
	SquirrelObject ret;

	if (getSlot(key)) {
		ret.attachToStackObject(-1);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

float SquirrelObject::getFloat(int key) const
{
	float ret = 0.0f;

	if (getSlot(key)) {
		sq_getfloat(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

int SquirrelObject::getInt(int key) const
{
	int ret = 0;

	if (getSlot(key)) {
		sq_getinteger(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

const SQChar *SquirrelObject::getString(int key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

bool SquirrelObject::getBool(int key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret ? true : false;
}

bool SquirrelObject::exists(const SQChar *key) const
{
	if (getSlot(key)) {
		sq_pop(SquirrelVM::ms_rootVM, 2);
		return true;
	} else {
		sq_pop(SquirrelVM::ms_rootVM, 1);
		return false;
	}
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool SquirrelObject::getSlot(const SQChar *name) const
{
	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_pushstring(SquirrelVM::ms_rootVM, name,-1);
	if (SQ_SUCCEEDED(sq_get(SquirrelVM::ms_rootVM,-2))) {
		return true;
	}

	return false;
}

bool SquirrelObject::rawGetSlot(const SQChar *name) const {
	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_pushstring(SquirrelVM::ms_rootVM, name,-1);
	if (SQ_SUCCEEDED(sq_rawget(SquirrelVM::ms_rootVM,-2))) {
		return true;
	}
	return false;
} // SquirrelObject::rawGetSlot

SquirrelObject SquirrelObject::getValue(const SQChar *key)const
{
	SquirrelObject ret;
	if (getSlot(key)) {
		ret.attachToStackObject(-1);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

float SquirrelObject::getFloat(const SQChar *key) const
{
	float ret = 0.0f;
	if (getSlot(key)) {
		sq_getfloat(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

int SquirrelObject::getInt(const SQChar *key) const
{
	int ret = 0;
	if (getSlot(key)) {
		sq_getinteger(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

const SQChar *SquirrelObject::getString(const SQChar *key) const
{
	const SQChar *ret = NULL;

	if (getSlot(key)) {
		sq_getstring(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret;
}

bool SquirrelObject::getBool(const SQChar *key) const
{
	SQBool ret = false;

	if (getSlot(key)) {
		sq_getbool(SquirrelVM::ms_rootVM,-1,&ret);
		sq_pop(SquirrelVM::ms_rootVM, 1);
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return ret?true:false;
}

SQUserPointer SquirrelObject::getInstanceUP(SQUserPointer tag) const
{
	SQUserPointer up;
	sq_pushobject(SquirrelVM::ms_rootVM, _o);

	if (SQ_FAILED(sq_getinstanceup(SquirrelVM::ms_rootVM,-1,(SQUserPointer*)&up, tag))) {
		sq_reseterror(SquirrelVM::ms_rootVM);
		up = NULL;
	}

	sq_pop(SquirrelVM::ms_rootVM, 1);
	return up;
}

bool SquirrelObject::setInstanceUP(SQUserPointer up)
{
	if (!sq_isinstance(_o))
		return false;

	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_setinstanceup(SquirrelVM::ms_rootVM,-1, up);
	sq_pop(SquirrelVM::ms_rootVM, 1);
	return true;
}

SquirrelObject SquirrelObject::getAttributes(const SQChar *key)
{
	SquirrelObject ret;
	int top = sq_gettop(SquirrelVM::ms_rootVM);
	sq_pushobject(SquirrelVM::ms_rootVM, _o);

	if (key)
		sq_pushstring(SquirrelVM::ms_rootVM, key,-1);
	else
		sq_pushnull(SquirrelVM::ms_rootVM);

	if (SQ_SUCCEEDED(sq_getattributes(SquirrelVM::ms_rootVM,-2))) {
		ret.attachToStackObject(-1);
	}

	sq_settop(SquirrelVM::ms_rootVM, top);
	return ret;
}

bool SquirrelObject::beginIteration()
{
	if (!sq_istable(_o) && !sq_isarray(_o) && !sq_isclass(_o))
		return false;

	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_pushnull(SquirrelVM::ms_rootVM);
	return true;
}

bool SquirrelObject::next(SquirrelObject &key, SquirrelObject &val)
{
	if (SQ_SUCCEEDED(sq_next(SquirrelVM::ms_rootVM,-2))) {
		key.attachToStackObject(-2);
		val.attachToStackObject(-1);
		sq_pop(SquirrelVM::ms_rootVM, 2);
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
	sq_pushobject(SquirrelVM::ms_rootVM, _o);
	sq_getbase(SquirrelVM::ms_rootVM,-1);
	ret.attachToStackObject(-1);
	sq_pop(SquirrelVM::ms_rootVM, 2);

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
	sq_pop(SquirrelVM::ms_rootVM, 2);
}

void StackHandler::getRawData( int idx, Value &result)
{
	// make sure result is emtpy first
	AX_ASSERT(result.getTypeId() == Variant::kVoid);

	HSQOBJECT t;
	sq_getstackobj(SquirrelVM::ms_rootVM, idx, &t);

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
				result.init(Variant::kVector3, userdata);
			} else if (typetag == &__Color3_decl) {
				result.init(Variant::kColor3, userdata);
			} else if (typetag == &__Point_decl) {
				result.init(Variant::kPoint, userdata);
			} else if (typetag == &__Rect_decl) {
				result.init(Variant::kRect, userdata);
			} else if (typetag == &__Matrix_decl) {
				result.init(Variant::kMatrix, userdata);
			} else if (typetag == &__Object_c_decl) {
				result.init(Variant::kObject, userdata);
			}
		}
		return;
	}
	return;
}


AX_END_NAMESPACE
