#ifndef AX_SQHELPER_H
#define AX_SQHELPER_H

AX_BEGIN_NAMESPACE

class SquirrelObject;
class SquirrelVM;

class SquirrelError
{
public:
	SquirrelError(SquirrelVM *vm);
	SquirrelError(const SQChar* s):desc(s){}
	const SQChar *desc;
};

class SquirrelVM
{
public:
	SquirrelVM();
	SquirrelVM(HSQUIRRELVM vm);
	~SquirrelVM();

	SquirrelObject compileFile(const SQChar *filename);
	SquirrelObject compileBuffer(const SQChar *buf, const SQChar *debugInfo=_SC("console_buffer"));

	SquirrelObject runBytecode(const SquirrelObject &bytecode, SquirrelObject *_this = NULL);

	SquirrelObject runFile(const SQChar *s, SquirrelObject *_this = NULL);
	SquirrelObject runBuffer(const SQChar *s, SquirrelObject *_this = NULL);

protected:
	// replace print
	static void printFunc(HSQUIRRELVM v, const SQChar* s,...);

public:
	static HSQUIRRELVM ms_rootVM;
	HSQUIRRELVM m_vm;
};

class SquirrelObject
{
	friend class SquirrelVM;

public:
	SquirrelObject();
	~SquirrelObject();
	SquirrelObject(const SquirrelObject &o);
	SquirrelObject(HSQOBJECT o);

#if 1
	template <typename _ty>
	SquirrelObject(const _ty & val) { sq_resetobject(&_o); set((_ty &)val); } // Cast away const to avoid compiler SqPlus::Push() match issue.
	template <typename _ty>
	SquirrelObject(_ty & val) { sq_resetobject(&_o); set(val); }
	template <typename _ty>
	SquirrelObject(_ty * val) { sq_resetobject(&_o); setByValue(val); } // set() would also be OK here. setByValue() to save potential compiler overhead.
#endif

	SquirrelObject& operator = (HSQOBJECT ho);
	SquirrelObject& operator = (const SquirrelObject &o);
	SquirrelObject& operator = (int n);
	SquirrelObject& operator = (HSQUIRRELVM v);

	operator HSQOBJECT& (){ return _o; } 
	bool operator == (const SquirrelObject& o);
	bool compareUserPointer(const SquirrelObject& o);

	void attachToStackObject(int idx);
	void reset(void); // Release (any) reference and reset _o.
	SquirrelObject clone();
	bool setValue(const SquirrelObject &key, const SquirrelObject &val);

	bool setValue(SQInteger key, const SquirrelObject &val);
	bool setValue(int key, bool b); // Compiler treats SQBool as int.
	bool setValue(int key, int n);
	bool setValue(int key, float f);
	bool setValue(int key, const SQChar *s);

	bool setValue(const SQChar *key, const SquirrelObject &val);
	bool setValue(const SQChar *key, bool b);
	bool setValue(const SQChar *key, int n);
	bool setValue(const SQChar *key, float f);
	bool setValue(const SQChar *key, const SQChar *s);

	bool setUserPointer(const SQChar * key, SQUserPointer up);
	SQUserPointer getUserPointer(const SQChar * key);
	bool setUserPointer(int key, SQUserPointer up);
	SQUserPointer getUserPointer(int key);

	bool newUserData(const SQChar * key, int size, SQUserPointer * typetag=0);
	bool getUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag=0);
	bool rawGetUserData(const SQChar * key, SQUserPointer * data, SQUserPointer * typetag=0);

	// === BEGIN Arrays ===
	bool arrayResize(int newSize);
	bool arrayExtend(int amount);
	bool arrayReverse(void);
	SquirrelObject arrayPop(SQBool returnPoppedVal=SQTrue);

	void arrayAppend(const SquirrelObject &o);

	template<typename T>
	bool arrayAppend(T item);
	// === END Arrays ===

	bool setInstanceUP(SQUserPointer up);
	bool isNull() const;
	bool isNumeric() const;
	int len() const;
	bool setDelegate(SquirrelObject &obj);
	SquirrelObject getDelegate();
	const SQChar* toString();
	bool toBool();
	SQInteger toInteger();
	SQFloat toFloat();
	SQUserPointer getInstanceUP(SQUserPointer tag) const;
	SquirrelObject getValue(const SQChar *key) const;
	bool exists(const SQChar *key) const;
	float getFloat(const SQChar *key) const;
	int getInt(const SQChar *key) const;
	const SQChar *getString(const SQChar *key) const;
	bool getBool(const SQChar *key) const;
	SquirrelObject getValue(int key) const;
	float getFloat(int key) const;
	int getInt(int key) const;
	const SQChar *getString(int key) const;
	bool getBool(int key) const;
	SquirrelObject getAttributes(const SQChar *key = NULL);
	SQObjectType getType();
	HSQOBJECT & getObjectHandle() const {return *(HSQOBJECT*)&_o;}
	bool beginIteration();
	bool next(SquirrelObject &key, SquirrelObject &value);
	void endIteration();

	bool getTypeTag(SQUserPointer * typeTag);

	// === get the type name of item/object through string key in a table or class. Returns NULL if the type name is not set (not an SqPlus registered type).
	const SQChar * getTypeName(const SQChar * key);
	// === get the type name of item/object through int key in a table or class. Returns NULL if the type name is not set (not an SqPlus registered type).
	const SQChar * getTypeName(int key);
	// === get the type name of this object, else return NULL if not an SqPlus registered type.
	const SQChar * getTypeName(void);

	// === Return base class of object using sq_getbase() === 
	SquirrelObject getBase();

	// === BEGIN code suggestion from the Wiki ===
	// get any bound type from this SquirrelObject. Note that Squirrel's handling of references and pointers still holds here.
	template<typename _ty>
	_ty get(void);

	// set any bound type to this SquirrelObject. Note that Squirrel's handling of references and pointers still holds here.
	template<typename _ty>
	SquirrelObject setByValue(_ty val); // classes/structs should be passed by ref (below) to avoid an extra copy.

	// set any bound type to this SquirrelObject. Note that Squirrel's handling of references and pointers still holds here.
	template<typename _ty>
	SquirrelObject &set(_ty & val);

	// === END code suggestion from the Wiki ===
	Ref tryToReturnArgument();

private:
	bool getSlot(const SQChar *name) const;
	bool rawGetSlot(const SQChar *name) const;
	bool getSlot(int key) const;
	HSQOBJECT _o;
};

struct StackHandler
{
	StackHandler(HSQUIRRELVM v)
	{
		_top = sq_gettop(v);
		this->v = v;
	}

	SQFloat getFloat(int idx)
	{
		SQFloat x = 0.0f;
		if(idx > 0 && idx <= _top) {
			sq_getfloat(v, idx, &x);
		}
		return x;
	}

	SQInteger getInt(int idx)
	{
		SQInteger x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getinteger(v, idx, &x);
		}
		return x;
	}

	HSQOBJECT getObjectHandle(int idx)
	{
		HSQOBJECT x;
		if(idx > 0 && idx <= _top) {
			sq_resetobject(&x);
			sq_getstackobj(v, idx, &x);
		}
		return x;
	}

	const SQChar *getString(int idx)
	{
		const SQChar *x = NULL;
		if(idx > 0 && idx <= _top) {
			sq_getstring(v, idx, &x);
		}
		return x;
	}

	SQUserPointer getUserPointer(int idx)
	{
		SQUserPointer x = 0;
		if(idx > 0 && idx <= _top) {
			sq_getuserpointer(v, idx, &x);
		}
		return x;
	}

	SQUserPointer getInstanceUp(int idx, SQUserPointer tag)
	{
		SQUserPointer self;
		if(SQ_FAILED(sq_getinstanceup(v, idx,(SQUserPointer*)&self, tag)))
			return NULL;
		return self;
	}

	SQUserPointer getUserData(int idx, SQUserPointer tag=0)
	{
		SQUserPointer otag;
		SQUserPointer up;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getuserdata(v, idx, &up,&otag))) {
				if(tag == otag)
					return up;
			}
		}
		return NULL;
	}

	bool getBool(int idx)
	{
		SQBool ret;
		if(idx > 0 && idx <= _top) {
			if(SQ_SUCCEEDED(sq_getbool(v, idx,&ret)))
				return ret;
		}
		return FALSE;
	}

	int getType(int idx)
	{
		if(idx > 0 && idx <= _top) {
			return sq_gettype(v, idx);
		}
		return -1;
	}

	int getParamCount() {
		return _top;
	}

	void getRawData(int idx, Value &result);

	int retRawData(const ConstRef &arg)
	{
		// TODO
		return 0;
	}

	int Return(const SQChar *s)
	{
		sq_pushstring(v, s,-1);
		return 1;
	}

	int Return(float f)
	{
		sq_pushfloat(v, f);
		return 1;
	}

	int Return(int i)
	{
		sq_pushinteger(v, i);
		return 1;
	}

	int Return(bool b)
	{
		sq_pushbool(v, b);
		return 1;
	}

	int Return(SQUserPointer p) {
		sq_pushuserpointer(v, p);
		return 1;
	}

	int Return(SquirrelObject &o)
	{
		sq_pushobject(v, o.getObjectHandle());
		return 1;
	}

	int Return() { return 0; }

	int throwError(const SQChar *error) {
		return sq_throwerror(v, error);
	}

	HSQUIRRELVM getVM() { return v; }

private:
	int _top;
	HSQUIRRELVM v;
};

AX_END_NAMESPACE

#endif // AX_SQHELPER_H
