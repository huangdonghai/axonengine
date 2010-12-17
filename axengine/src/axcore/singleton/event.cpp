#include "../private.h"

AX_BEGIN_NAMESPACE

namespace {
	struct PostedEvent {
		IEventHandler *handler;
		Event *event;
	};

	std::list<PostedEvent> s_postedEvents;
	SyncMutex s_peMutex;
} // namespace

Event::Event(Type type) 
	: m_type(type), m_posted(false), m_spont(false), m_accept(true)
{}


Event::~Event()
{
}

int Event::registerEventType( int hint /*= -1*/ )
{
	return 0;
}

void Event::postEvent(IEventHandler *receiver, Event *e)
{
	PostedEvent pe;
	pe.handler = receiver;
	pe.event = e;

	ScopedLocker locker(s_peMutex);
	s_postedEvents.push_back(pe);
}

bool Event::sendEvent(IEventHandler *receiver, Event *e)
{
	AX_ASSERT(Thread::isInMainThread());

	std::list<IEventHandler *>::const_iterator it = receiver->m_eventFilters.begin();

	for (; it != receiver->m_eventFilters.end(); ++it) {
		if ((*it)->eventFilter(receiver, e))
			return true;
	}

	return receiver->event(e);
}

void Event::processEvents()
{
	ScopedLocker locker(s_peMutex);

	std::list<PostedEvent>::iterator it = s_postedEvents.begin();

	for (; it != s_postedEvents.end(); ++it) {
		PostedEvent &pe = *it;
		sendEvent(pe.handler, pe.event);

		// delete event object
		delete pe.event;
		pe.event = 0;
	}

	s_postedEvents.clear();
}



IEventHandler::IEventHandler()
{
	if (!Thread::isInMainThread())
		Errorf("EventHandler can only be owned by main thread");
}

IEventHandler::~IEventHandler()
{

}

bool IEventHandler::event(Event *e)
{
	return false;
}

bool IEventHandler::eventFilter(IEventHandler *watched, Event *e)
{
	return false;
}

void IEventHandler::installEventFilter(IEventHandler *watcher)
{
	m_eventFilters.push_back(watcher);
}

void IEventHandler::removeEventFilter( IEventHandler *watcher )
{
	m_eventFilters.remove(watcher);
}

AX_END_NAMESPACE
