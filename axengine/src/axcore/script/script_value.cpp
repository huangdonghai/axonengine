#include "script_p.h"

AX_BEGIN_NAMESPACE

namespace {

	class Handler : public Variant::TypeHandler
	{
	public:
		Handler()
		{
			m_dataSize = sizeof(ScriptValue);
		}

		virtual bool canCast(Variant::TypeId toType)
		{
			return true;
		}

		virtual bool rawCast(const void *fromData, Variant::TypeId toType, void *toData)
		{
			const ScriptValue &from = *reinterpret_cast<const ScriptValue*>(fromData);
			return from.rawCast(toType, toData);
		}

		virtual void construct(void *ptr, const void *copyfrom)
		{
			new (ptr) ScriptValue(*reinterpret_cast<const ScriptValue*>(copyfrom));
		}
		virtual void construct(void *ptr)
		{
			new (ptr) ScriptValue();
		}

		virtual void destruct(void *ptr)
		{
			reinterpret_cast<ScriptValue*>(ptr)->~ScriptValue();
		}
	};

	Handler s_scriptValueHandler;
}

ScriptValue::ScriptValue() : m_d(new SquirrelObject())
{}

ScriptValue::ScriptValue(const SquirrelObject &sobj) : m_d(new SquirrelObject(sobj))
{}

ScriptValue::ScriptValue(const ScriptValue &rhs) : m_d(new SquirrelObject(*rhs.m_d))
{}

ScriptValue::~ScriptValue()
{
	SafeDelete(m_d);
}

bool ScriptValue::rawCast(Variant::TypeId toType, void *toData) const
{
	switch (toType) {
	case Variant::kVoid:
		return false;
	case Variant::kBool:
		*(reinterpret_cast<bool *>(toData)) = m_d->toBool();
		return true;
	case Variant::kInt:
		*(reinterpret_cast<int *>(toData)) = m_d->toInteger();
		return true;
	case Variant::kFloat:
		*(reinterpret_cast<float *>(toData)) = m_d->toFloat();
		return true;
	case Variant::kString:
		*(reinterpret_cast<String *>(toData)) = m_d->toString();
		return true;
	case Variant::kObject:
	case Variant::kVector3:
	case Variant::kColor3:
	case Variant::kPoint:
	case Variant::kRect:
	case Variant::kMatrix:
	case Variant::kScriptValue:
	default:
		return false;
	}

	return false;
}

Variant::TypeHandler * ScriptValue::getTypeHandler()
{
	return &s_scriptValueHandler;
}

ScriptValue& ScriptValue::operator=(const ScriptValue &rhs)
{
	*m_d = *rhs.m_d;
	return *this;
}

AX_END_NAMESPACE
