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

	ScriptValue& operator=(const ScriptValue &rhs);

//	sqObject& getSquirrelObject() { return *m_d; }
	sqObject& getSquirrelObject() const { return *m_d; }

	bool rawCast(Variant::TypeId toType, void *toData) const;
	static Variant::TypeHandler *getTypeHandler();

private:
	sqObject *m_d;
};

AX_END_NAMESPACE

#endif // AX_HSQOBJECT_H
