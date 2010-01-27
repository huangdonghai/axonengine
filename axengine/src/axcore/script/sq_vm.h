#ifndef AX_SQUIRRELVM_H
#define AX_SQUIRRELVM_H

AX_BEGIN_NAMESPACE

class sqVM
{
public:
	sqVM();
	sqVM(HSQUIRRELVM vm);
	~sqVM();

	sqObject compileFile(const SQChar *filename);
	sqObject compileBuffer(const SQChar *buf, const SQChar *debugInfo=_SC("console_buffer"));

	sqObject runBytecode(const sqObject &bytecode, sqObject *_this = NULL);

	sqObject runFile(const SQChar *s, sqObject *_this = NULL);
	sqObject runBuffer(const SQChar *s, sqObject *_this = NULL);

	sqObject getScoped(const char *name);
	sqObject getScoped(const sqObject &obj, const char *name);

	static sqObject createClosure(const SQChar *name, SQFUNCTION f, SQInteger nparamscheck, const SQChar *typemask);

	static void pushMeta(HSQUIRRELVM v, const ConstRef &ref);
	static void popMeta(HSQUIRRELVM v, Variant &val);
	static void getMeta(HSQUIRRELVM v, int idx, Variant &val);

	static int allocThread();
	static void freeThread(int threadId);

protected:
	void reportError();
	static void createObjectClosure();

	// replace print
	static void printFunc(HSQUIRRELVM v, const SQChar* s,...);

public:
	enum { INIT_THREADPOOL_SIZE = 256 };
	static sqObject ms_getClosure;
	static sqObject ms_setClosure;
	static Sequence<sqObject> ms_threadPool;
	static List<int> ms_freeThreads;

	typedef IntrusiveList<Object, &Object::m_threadLink> ObjectThreadList;
	static ObjectThreadList ms_objThreadList;

	HSQUIRRELVM m_vm;
};

AX_END_NAMESPACE

#endif // AX_SQUIRRELVM_H
