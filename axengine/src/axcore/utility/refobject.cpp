#include "../private.h"

AX_BEGIN_NAMESPACE

namespace {
	std::list<RefObject *> s_deferredDeleteObjectList;
} // namespace


void RefObject::incref()
{
	m_ref.incref();
}

void RefObject::decref()
{
	if (m_ref.decref() != 0)
		return;

	onDestroy();

	if (canBeDeletedNow()) {
		delete this;
		return;
	} else {
		s_deferredDeleteObjectList.push_back(this);
	}
}


void RefObject::onDestroy()
{
}

void RefObject::checkDeferredDeleteObject()
{
	std::list<RefObject *>::iterator it = s_deferredDeleteObjectList.begin();

	while (it != s_deferredDeleteObjectList.end()) {
		RefObject *obj = *it;
		if (obj->canBeDeletedNow()) {
			it = s_deferredDeleteObjectList.erase(it);
			delete obj;
			continue;
		} else {
			++it;
		}
	}
}


AX_END_NAMESPACE
