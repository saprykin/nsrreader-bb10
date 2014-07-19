#include "nsrglobalnotifier.h"

NSRGlobalNotifier* NSRGlobalNotifier::_instance = NULL;

NSRGlobalNotifier::NSRGlobalNotifier () :
	QObject ()
{
}

NSRGlobalNotifier::~NSRGlobalNotifier ()
{
}

NSRGlobalNotifier *
NSRGlobalNotifier::instance ()
{
	if (_instance == NULL)
		_instance = new NSRGlobalNotifier ();

	return _instance;
}

void
NSRGlobalNotifier::languageChangedSignal ()
{
	emit languageChanged ();
}
