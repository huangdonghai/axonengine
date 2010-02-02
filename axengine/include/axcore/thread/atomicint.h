#ifndef AX_ATOMICINT_H
#define AX_ATOMICINT_H

struct AtomicInt {
public:
	AtomicInt() : m_value(0) {}
	~AtomicInt() {}

	int incref()
	{
		return InterlockedIncrement(&m_value);
	}

	int decref()
	{
		return InterlockedDecrement(&m_value);
	}

private:
	volatile long m_value;
};
#endif // AX_ATOMICINT_H
