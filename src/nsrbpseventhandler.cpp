#include "nsrbpseventhandler.h"

#include <bps/virtualkeyboard.h>
#include <bps/navigator.h>

NSRBpsEventHandler::NSRBpsEventHandler (QObject *parent) :
	QObject (parent),
	_vkbVisible (false)
{
	bps_initialize ();
	subscribe (virtualkeyboard_get_domain ());
	subscribe (navigator_get_domain ());
	virtualkeyboard_request_events (0);
	navigator_request_events (0);
}

NSRBpsEventHandler::~NSRBpsEventHandler ()
{
	bps_shutdown ();
}

void
NSRBpsEventHandler::event (bps_event_t* event)
{
	int eventDomain = bps_event_get_domain (event);
	int eventCode = bps_event_get_code (event);

	if (eventDomain == virtualkeyboard_get_domain ()) {
		switch (eventCode) {
		case VIRTUALKEYBOARD_EVENT_VISIBLE:
			_vkbVisible = true;
			emit vkbVisibilityChanged (true);
			break;
		case VIRTUALKEYBOARD_EVENT_HIDDEN:
			_vkbVisible = false;
			emit vkbVisibilityChanged (false);
			break;
		default:
			break;
		}
	} else if (eventDomain == navigator_get_domain ()) {
		switch (eventCode) {
		case NAVIGATOR_WINDOW_ACTIVE:
			emit windowActiveChanged (true);
			break;
		case NAVIGATOR_WINDOW_INACTIVE:
			emit windowActiveChanged (false);
			break;
		default:
			break;
		}
	}
}
