#include "nsrbpseventhandler.h"

#include <bps/virtualkeyboard.h>

NSRBpsEventHandler::NSRBpsEventHandler (QObject *parent) :
	QObject (parent),
	_vkbVisible (false)
{
	bps_initialize ();
	subscribe (virtualkeyboard_get_domain ());
	virtualkeyboard_request_events (0);
}

NSRBpsEventHandler::~NSRBpsEventHandler ()
{
	bps_shutdown ();
}

void
NSRBpsEventHandler::event (bps_event_t* event)
{
	if (bps_event_get_domain (event) == virtualkeyboard_get_domain ()) {
		switch (bps_event_get_code (event)) {
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
	}
}
