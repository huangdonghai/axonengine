#ifndef AX_ATOMICINT_H
#define AX_ATOMICINT_H

struct AtomicInt {
public:
	AtomicInt() : m_value(0) {}
	AtomicInt(int val) : m_value(val) {}
	~AtomicInt() {}

	int incref() { return InterlockedIncrement(&m_value); }
	int decref() { return InterlockedDecrement(&m_value); }
	int getref() const { return m_value; }
	bool operator==(int rhs) const { return m_value == rhs; }
	bool operator!=(int rhs) const { return m_value != rhs; }
	bool operator!() const { return m_value == 0; }

private:
	volatile long m_value;
};

#endif // AX_ATOMICINT_H
