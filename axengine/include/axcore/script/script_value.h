#ifndef AX_HSQOBJECT_H
#define AX_HSQOBJECT_H

AX_BEGIN_NAMESPACE

class SquirrelObject;

// a wrapper for SquirrelObject, so we don't public inlcude squirrel's header files
class ScriptValue
{
public:
	ScriptValue();
	ScriptValue(const ScriptValue &rhs);
	ScriptValue(const SquirrelObject &sobj);
	~ScriptValue();

	ScriptValue& operator=(const ScriptValue &rhs);

//	SquirrelObject& getSquirrelObject() { return *m_d; }
	SquirrelObject& getSquirrelObject() const { return *m_d; }

	bool rawCast(Variant::TypeId toType, void *toData) const;
	static Variant::TypeHandler *getTypeHandler();

private:
	SquirrelObject *m_d;
};

AX_END_NAMESPACE

#endif // AX_HSQOBJECT_H
