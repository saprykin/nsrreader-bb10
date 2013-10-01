#ifndef NSRPAGECROPPER_H_
#define NSRPAGECROPPER_H_

#include "nsrcroppads.h"

class NSRPageCropper
{
public:
	enum NSRPixelOrder {
		NSR_PIXEL_ORDER_RGB	= 0,
		NSR_PIXEL_ORDER_ARGB	= 1,
		NSR_PIXEL_ORDER_BGR	= 2,
		NSR_PIXEL_ORDER_BGRA	= 3,
		NSR_PIXEL_ORDER_RGBA	= 4
	};

	static NSRCropPads findCropPads (unsigned char *data, NSRPixelOrder order, int width, int height, int stride);

private:
	NSRPageCropper ();
};

#endif /* NSRPAGECROPPER_H_ */
