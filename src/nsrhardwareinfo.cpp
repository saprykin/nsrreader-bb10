#include "nsrhardwareinfo.h"
#include "nsrreader.h"

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

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_isTrackpad = hwInfo.isTrackpadDevice ();
#endif

#if BBNDK_VERSION_AT_LEAST(10,1,0)
	_isOLED = dInfo.displayTechnology () == DisplayTechnology::Oled;
#endif
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
