#include "../private.h"

AX_BEGIN_NAMESPACE

namespace {
	struct PostedEvent {
		EventHandler *handler;
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

void Event::postEvent(EventHandler *receiver, Event *e)
{
	PostedEvent pe;
	pe.handler = receiver;
	pe.event = e;

	ScopedLocker locker(s_peMutex);
	s_postedEvents.push_back(pe);
}

bool Event::sendEvent(EventHandler *receiver, Event *e)
{
	std::list<EventHandler *>::const_iterator it = receiver->m_eventFilters.begin();

	for (; it != receiver->m_eventFilters.end(); ++it) {
		if ((*it)->eventFilter(receiver, e))
			return true;
	}

	return receiver->event(e);
}

void Event::processEvents()
{
	ScopedLocker locker(s_peMutex);

	std::list<PostedEvent>::const_iterator it = s_postedEvents.begin();

	for (; it != s_postedEvents.end(); ++it) {
		const PostedEvent &pe = *it;
		sendEvent(pe.handler, pe.event);
	}

	s_postedEvents.clear();
}



EventHandler::EventHandler()
{
	if (!Thread::isMainThread())
		Errorf("EventHandler can only be owned by main thread");
}

EventHandler::~EventHandler()
{

}

bool EventHandler::event(Event *e)
{
	return false;
}

bool EventHandler::eventFilter(EventHandler *watched, Event *e)
{
	return false;
}

void EventHandler::installEventFilter(EventHandler *watcher)
{
	m_eventFilters.push_back(watcher);
}

void EventHandler::removeEventFilter( EventHandler *watcher )
{
	m_eventFilters.remove(watcher);
}

AX_END_NAMESPACE
