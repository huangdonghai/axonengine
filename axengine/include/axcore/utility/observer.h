/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#ifndef AX_OBSERVER_H
#define AX_OBSERVER_H

// see GoF Design Patterns 5.7

AX_BEGIN_NAMESPACE

class IObservable;

class IObserver {
public:
	virtual void doNotify(IObservable *subject, int arg) = 0;
};

class AX_API IObservable {
public:
	virtual ~IObservable();

	void attachObserver(IObserver *observer);
	void detachObserver(IObserver *observer);
	void notify(int arg);

private:
	List<IObserver*>	m_observers;
};

AX_END_NAMESPACE


#endif // AX_OBSERVER_H
