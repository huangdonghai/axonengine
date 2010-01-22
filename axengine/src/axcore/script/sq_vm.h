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

	static sqObject createClosure(const SQChar *name, SQFUNCTION f, 	SQInteger nparamscheck, const SQChar *typemask);

protected:
	void reportError();
	static void createObjectClosure();

	// replace print
	static void printFunc(HSQUIRRELVM v, const SQChar* s,...);

public:
	static sqObject ms_getClosure;
	static sqObject ms_setClosure;
	HSQUIRRELVM m_vm;
};

AX_END_NAMESPACE

#endif // AX_SQUIRRELVM_H
