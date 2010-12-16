
#ifndef AX_CORE_EVENT_H
#define AX_CORE_EVENT_H

AX_BEGIN_NAMESPACE

class EventHandler;
class Event
{
public:
	enum Type {
		None = 0,                               // invalid event
		KeyDown, KeyUp, MouseDown, MouseUp, MouseMove, CharInput, MouseWheel, XboxAxis,

		AsioCompleted,

		User = 1000,                            // first user event id
		MaxUser = 65535                         // last user event id
	};

	Event(Type type);
	virtual ~Event();

	inline Type type() const { return m_type; }
	inline bool spontaneous() const { return m_spont; }

	inline void setAccepted(bool accepted) { m_accept = accepted; }
	inline bool isAccepted() const { return m_accept; }

	inline void accept() { m_accept = true; }
	inline void ignore() { m_accept = false; }

	static int registerEventType(int hint = -1);
	static void postEvent(EventHandler *receiver, Event *e); // thread safe
	static bool sendEvent(EventHandler *receiver, Event *e);

protected:
	static void processEvents(); // thread safe

protected:
	Type m_type;

private:
	ushort_t m_posted : 1;
	ushort_t m_spont : 1;
	ushort_t m_accept : 1;
	ushort_t m_reserved : 13;
};

class EventHandler
{
public:
	friend class Event;

	EventHandler();
	virtual ~EventHandler();

	virtual bool event(Event *e);
	virtual bool eventFilter(EventHandler *watched, Event *e);
	void installEventFilter(EventHandler *watcher);
	void removeEventFilter(EventHandler *watcher);

protected:
	std::list<EventHandler *> m_eventFilters;
};

AX_END_NAMESPACE

#endif // AX_CORE_EVENT_H
