#ifndef NSRHARDWAREINFO_H_
#define NSRHARDWAREINFO_H_

class NSRHardwareInfo
{
public:
	static NSRHardwareInfo * instance ();
	static void release ();

	inline bool isTrackpad () const {
		return _isTrackpad;
	}

	inline bool isOLED () const {
		return _isOLED;
	}

private:
	NSRHardwareInfo ();
	~NSRHardwareInfo ();

	static NSRHardwareInfo *	_instance;
	bool				_isTrackpad;
	bool				_isOLED;
};

#endif /* NSRHARDWAREINFO_H_ */
