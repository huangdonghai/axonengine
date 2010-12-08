#ifndef AX_HSQOBJECT_H
#define AX_HSQOBJECT_H

AX_BEGIN_NAMESPACE

class sqObject;

// a wrapper for sqObject, so we don't public inlcude squirrel's header files
class ScriptValue
{
public:
	ScriptValue();
	ScriptValue(const ScriptValue &rhs);
	ScriptValue(const sqObject &sobj);
	~ScriptValue();

	void reset() const;

	ScriptValue& operator=(const ScriptValue &rhs);

	bool isNull() const;
	bool isArray() const;
	bool isTable() const;
	bool isInstance() const;

	ScriptValue getValue(const std::string &name) const;

	sqObject& getSqObject() { return *m_d; }
	const sqObject& getSqObject() const { return *m_d; }

	bool rawCast(Variant::TypeId toType, void *toData) const;

	// internal use
	static Variant::TypeHandler *getTypeHandler();

private:
	sqObject *m_d;
};

class ScriptThread : public ScriptValue
{
public:
	enum ThreadState {
		TS_Idle, TS_Running, TS_Suspended
	};

	ScriptThread() : ScriptValue(), m_id(-1) {}
	ScriptThread(const ScriptValue &sv, int id) : ScriptValue(sv), m_id(id) {}
	ScriptThread(const ScriptThread &rhs) : ScriptValue(rhs), m_id(rhs.m_id) {}
	~ScriptThread() { reset(); }

	void reset();

	// thread
	ThreadState callThread(const ScriptValue &closure, Object *this_) const;
	ThreadState resumeThread();
	bool isSuspended() const;
	bool isRunning() const;
	bool isIdle() const;

private:
	int m_id;
};

AX_END_NAMESPACE

#endif // AX_HSQOBJECT_H
