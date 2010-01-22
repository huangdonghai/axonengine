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
	StackHandler sa(v);
	const char *name = sa.getString(2);

	g_scriptSystem->registerSqClass(name);

	return SQ_OK;
}

sqObject sqVM::ms_getClosure;
sqObject sqVM::ms_setClosure;

static SQRegFunction ax_funcs[] = {
	{ "loadfile", &loadfile, -2, _SC(".sb") },
	{ "dofile", &dofile, -2, _SC(".sb") },
	{ "AX_REGISTER_CLASS", &registerClass, -2, _SC(".s") },
	{ 0, 0 }
};


sqVM::sqVM()
{
	if (VM) {
		Errorf("Already create root vm");
	}

	VM = sq_open(1024);
	AX_ASSERT(VM);
	sq_setprintfunc(VM, sqVM::printFunc);
	sq_pushroottable(VM);
	sqstd_register_iolib(VM);
	sqstd_register_bloblib(VM);
	sqstd_register_mathlib(VM);
	sqstd_register_stringlib(VM);
#ifdef SQPLUS_SQUIRRELVM_WITH_SYSTEMLIB        
	sqstd_register_systemlib(VM);
#endif        
	sqstd_seterrorhandlers(VM);
	//TODO error handler, compiler error handler
	sq_pop(VM, 1);

	// register or replace functions
	int top = sq_gettop(VM);
	sq_pushroottable(VM);

	int i = 0;
	while (ax_funcs[i].name != 0) {
		SQRegFunction &f = ax_funcs[i];
		sq_pushstring(VM, f.name, -1);
		sq_newclosure(VM, f.f, 0);
		sq_setparamscheck(VM, f.nparamscheck, f.typemask);
		sq_setnativeclosurename(VM, -1, f.name);
		sq_createslot(VM, -3);
		i++;
	}
	sq_settop(VM, top);

	createObjectClosure();

	m_vm = VM;
}

sqVM::sqVM(HSQUIRRELVM vm)
{

}

sqVM::~sqVM()
{

}

void sqVM::printFunc(HSQUIRRELVM v, const SQChar* s, ...)
{
	static char temp[2048];
	va_list vl;
	va_start(vl, s);
	StringUtil::vsnprintf(temp, ArraySize(temp), s, vl);
	Printf("%s\n", temp);
	va_end(vl);
}

sqObject sqVM::compileFile(const SQChar *filename)
{
	sqObject ret;
	if (SQ_SUCCEEDED(_loadfile(m_vm, filename, 1))) {
		ret.attachToStackObject(m_vm, -1);
		sq_pop(m_vm, 1);
		return ret;
	}
	reportError();
	return ret;
//	throw SquirrelError(this);
}

sqObject sqVM::compileBuffer(const SQChar *s, const SQChar *debugInfo/*=_SC("console_buffer")*/)
{
	sqObject ret;
	if (SQ_SUCCEEDED(sq_compilebuffer(m_vm, s, (int)scstrlen(s)*sizeof(SQChar), debugInfo, 1))) {
		ret.attachToStackObject(m_vm, -1);
		sq_pop(m_vm, 1);
		return ret;
	}

	reportError();
	return ret;
//	throw SquirrelError(this);
}

sqObject sqVM::runBytecode(const sqObject &bytecode, sqObject *_this /*= NULL*/)
{
	sqObject ret;
	sq_pushobject(m_vm, bytecode.m_obj);
	if (_this) {
		sq_pushobject(m_vm, _this->m_obj);
	} else {
		sq_pushroottable(m_vm);
	}
	if (SQ_SUCCEEDED(sq_call(m_vm, 1, SQTrue, SQTrue))) {
		ret.attachToStackObject(m_vm, -1);
		sq_pop(m_vm, 2);
		return ret;
	}
	sq_pop(m_vm, 1);

	reportError();
	return ret;
//	throw SquirrelError(this);
}

sqObject sqVM::runFile(const SQChar *s, sqObject *_this /*= NULL*/)
{
	sqObject bytecode = compileFile(s);
	return runBytecode(bytecode, _this);
}

sqObject sqVM::runBuffer(const SQChar *s, sqObject *_this /*= NULL*/)
{
	sqObject bytecode = compileBuffer(s);
	return runBytecode(bytecode, _this);
}

sqObject sqVM::createClosure(const SQChar *name, SQFUNCTION f, SQInteger nparamscheck, const SQChar *typemask)
{
	sq_newclosure(VM, f, 0);
	sq_setparamscheck(VM, nparamscheck, typemask);
	sq_setnativeclosurename(VM, -1, name);

	sqObject sobj;
	sobj.attachToStackObject(VM, -1);
	sq_pop(VM, 1);
	return sobj;
}

sqObject sqVM::getScoped(const char *name)
{
	sq_pushroottable(m_vm);
	sqObject so;
	so.attachToStackObject(m_vm, -1);
	sq_pop(m_vm, 1);

	return getScoped(so, name);
}

sqObject sqVM::getScoped(const sqObject &obj, const char *name)
{
	ScopedStack scopedStack(m_vm);

	char buf[64];
	int num = 0;
	sqObject result;

	if (!name || !name[0]) {
		return result;
	}

	sq_pushobject(m_vm, obj);

	while (1) {
		int c = *name++;

		if (isalpha(c) || c == '_' || (isdigit(c) && num != 0)) {
			buf[num++] = c;
			AX_ASSERT(num < s2i(ArraySize(buf)));
			continue;
		}

		if (c == '.' || c == 0) {
			if (num) {
				buf[num] = 0;
				sq_pushstring(m_vm, buf, num);
				if (SQ_SUCCEEDED(sq_get(m_vm, -2))) {
					sq_remove(m_vm, -2);
				} else {
					return result;
				}
			} else {
				return result;
			}

			if (c == 0) {
				break;
			}

			num = 0;
			continue;
		}

		// not known char, error
		return result;
	}

	result.attachToStackObject(m_vm, -1);
	return result;
}

void sqVM::reportError()
{
	const SQChar *s = 0;
	const SQChar *desc = 0;
	sq_getlasterror(m_vm);
	sq_getstring(m_vm, -1, &s);
	if (s) {
		desc = s;
	} else {
		desc = _SC("unknown error");
	}
	sq_reseterror(m_vm);

	Errorf("script error: %s", desc);
}

AX_END_NAMESPACE
