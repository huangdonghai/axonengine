#ifndef AX_NONCOPYABLE_H
#define AX_NONCOPYABLE_H

AX_BEGIN_NAMESPACE

class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}
private:  // emphasize the following members are private
	Noncopyable(const Noncopyable&);
	const Noncopyable& operator=(const Noncopyable&);
};


template <int v>
class Int_ {};

AX_END_NAMESPACE

#endif // AX_NONCOPYABLE_H
