#include "nsrhardwareinfo.h"

#include <bb/device/HardwareInfo>
#include <bb/device/DisplayInfo>

using namespace bb::device;

NSRHardwareInfo * NSRHardwareInfo::_instance = NULL;

NSRHardwareInfo::NSRHardwareInfo () :
	_isTrackpad (false),
	_isOLED (false)
{
	HardwareInfo	hwInfo;
	DisplayInfo	dInfo;

	_isTrackpad = hwInfo.isTrackpadDevice ();
	_isOLED = dInfo.displayTechnology () == DisplayTechnology::Oled;
}

NSRHardwareInfo::~NSRHardwareInfo ()
{
}

NSRHardwareInfo *
NSRHardwareInfo::instance ()
{
	if (_instance == NULL)
		_instance = new NSRHardwareInfo ();

	return _instance;
}

void
NSRHardwareInfo::release ()
{
	if (_instance != NULL) {
		delete _instance;
		_instance = NULL;
	}
}
