/*
Copyright (c) 2009 AxonEngine Team

This file is part of the AxonEngine project, and may only be used, modified, 
and distributed under the terms of the AxonEngine project license, license.txt.  
By continuing to use, modify, or distribute this file you indicate that you have
read the license and understand and accept it fully.
*/


#include "../private.h"

namespace Axon {

	IObservable::~IObservable() {}

	void IObservable::attachObserver(IObserver* observer) {
		m_observers.push_back(observer);
	}

	void IObservable::detachObserver(IObserver* observer) {
		List<IObserver*>::iterator it = m_observers.begin();

		for (; it != m_observers.end(); ++it) {
			if (*it == observer) {
				m_observers.erase(it);
				return;
			}
		}

		Errorf("Observable::detachObserver: cann't find observer to detach");
	}

	void IObservable::notify(int arg) {
		List<IObserver*>::iterator it = m_observers.begin();

		for (; it != m_observers.end(); ++it) {
			(*it)->doNotify(this, arg);
		}
	}

} // namespace Axon
