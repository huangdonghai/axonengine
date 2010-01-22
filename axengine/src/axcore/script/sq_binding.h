#ifndef AX_SQBINDING_H
#define AX_SQBINDING_H

AX_BEGIN_NAMESPACE

struct ScriptClassMemberDecl  {
	const SQChar *name;
	SQFUNCTION func;
	int params;
	const SQChar *typemask;
};

struct SquirrelClassDecl  {
	const Variant::TypeId typeId;
	const SQChar *name;
	const SQChar *base;
	const ScriptClassMemberDecl *members;
};

struct ScriptConstantDecl  {
	const SQChar *name;
	SQObjectType type;
	union value {
		value(float v){ f = v; }
		value(int v){ i = v; }
		value(long int v){ li = v; }
		value(const SQChar *v){ s = v; }
		float f;
		int i;
		long int li;
		const SQChar *s;
	} val;
};

struct ScriptNamespaceDecl  {
	const SQChar *name;
	const ScriptClassMemberDecl *members;
	const ScriptConstantDecl *constants;
	const ScriptClassMemberDecl *delegate;
};

#define _BEGIN_CLASS(classname)  \
	int __##classname##__typeof(HSQUIRRELVM v) \
	{ \
		sq_pushstring(v, _SC(#classname),-1); \
		return 1; \
	} \
	struct ScriptClassMemberDecl __##classname##_members[] = { \
	{_SC("_typeof"), __##classname##__typeof, 1, NULL},

#define _BEGIN_NAMESPACE(xnamespace) struct ScriptClassMemberDecl __##xnamespace##_members[] = {
#define _BEGIN_NAMESPACE_CONSTANTS(xnamespace) {NULL, NULL, 0, NULL}}; \
	struct ScriptConstantDecl __##xnamespace##_constants[] = {

#define _BEGIN_DELEGATE(xnamespace) struct ScriptClassMemberDecl __##xnamespace##_delegate[] = {
#define _DELEGATE(xnamespace) __##xnamespace##_delegate
#define _END_DELEGATE(classname) {NULL, NULL, NULL, NULL}};

#define _CONSTANT(name, type, val) {_SC(#name), type, val},
#define _CONSTANT_IMPL(name, type) {_SC(#name), type, name},

#define _MEMBER_FUNCTION(classname, name, nparams, typemask) \
	{_SC(#name), __##classname##_##name, nparams, typemask},

#define _END_NAMESPACE(classname, delegate) {NULL, OT_NULL, 0}}; \
struct ScriptNamespaceDecl __##classname##_decl = {   \
	_SC(#classname), __##classname##_members, __##classname##_constants, delegate };

#define _END_CLASS(classname) {NULL, NULL, 0, NULL}}; \
struct SquirrelClassDecl __##classname##_decl = {  \
	classname##_getId(), _SC(#classname), NULL, __##classname##_members };


#define _END_CLASS_INHERITANCE(classname, base) {NULL, NULL, NULL, NULL}}; \
struct SquirrelClassDecl __##classname##_decl = {  \
	_SC(#classname), _SC(#base), __##classname##_members };

#define _MEMBER_FUNCTION_IMPL(classname, name) \
	int __##classname##_##name(HSQUIRRELVM v)

#define _INIT_STATIC_NAMESPACE(classname) CreateStaticNamespace(sqVM::getVM(),&__##classname##_decl);
#define _INIT_CLASS(classname) CreateClass(VM,&__##classname##_decl);

#define _DECL_STATIC_NAMESPACE(xnamespace) extern struct ScriptNamespaceDecl __##xnamespace##_decl;
#define _DECL_CLASS(classname) extern struct SquirrelClassDecl __##classname##_decl;

#define _CHECK_SELF(cppclass, scriptclass) \
	cppclass *self = NULL; \
	if (SQ_FAILED(sq_getinstanceup(v, 1, (SQUserPointer*)&self, (SQUserPointer)&__##scriptclass##_decl))) { \
		return sq_throwerror(v, _SC("invalid instance type"));\
	}

#define _CHECK_SELF_OBJ() \
	ObjectStar self = NULL; \
	if (SQ_FAILED(sq_getinstanceup(v, 1, (SQUserPointer*)&self, (SQUserPointer)&__##Object_c##_decl))) { \
		return sq_throwerror(v, _SC("invalid instance type"));\
	}

#define _CHECK_INST_PARAM(pname, idx, cppclass, scriptclass)  \
	cppclass *pname = NULL; \
	if (SQ_FAILED(sq_getinstanceup(v, idx,(SQUserPointer*)&pname,(SQUserPointer)&__##scriptclass##_decl))) { \
		return sq_throwerror(v, _SC("invalid instance type"));\
	} \

#define _CHECK_INST_PARAM_BREAK(pname, idx, cppclass, scriptclass)  \
	cppclass *pname = NULL; \
	if (SQ_FAILED(sq_getinstanceup(v, idx,(SQUserPointer*)&pname,(SQUserPointer)&__##scriptclass##_decl))) { \
		break; \
	} \

#define _CLASS_TAG(classname) ((unsigned int)&__##classname##_decl)


#define _DECL_NATIVE_CONSTRUCTION(classname, cppclass) \
	bool push_##classname(HSQUIRRELVM v, const cppclass &quat); \
	sqObject new_##classname(HSQUIRRELVM v, const cppclass &quat);

#define _IMPL_NATIVE_CONSTRUCTION(classname, cppclass) \
static Variant::TypeId classname##_getId() \
{ \
	return GetVariantType_<cppclass>(); \
} \
bool push_##classname(HSQUIRRELVM v, const cppclass &quat) \
{ \
	if (!CreateNativeClassInstance(v, _SC(#classname), &quat)) { \
		return FALSE; \
	} \
	return TRUE; \
} \
sqObject new_##classname(HSQUIRRELVM v, const cppclass &quat) \
{ \
	sqObject ret; \
	if (push_##classname(v, quat)) { \
		ret.attachToStackObject(v, -1); \
		sq_pop(v, 1); \
	} \
	return ret; \
} \


bool CreateStaticNamespace(HSQUIRRELVM v, ScriptNamespaceDecl *sn);
bool CreateClass(HSQUIRRELVM v, SquirrelClassDecl *cd);
bool CreateNativeClassInstance(HSQUIRRELVM v, const SQChar *classname, const void *ud);

AX_END_NAMESPACE

#endif // AX_SQBINDING_H
