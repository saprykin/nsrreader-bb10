#ifndef NSRPAGECROPPER_H_
#define NSRPAGECROPPER_H_

struct NSRCropPads
{
	NSRCropPads () :
		topPad (0),
		rightPad (0),
		bottomPad (0),
		leftPad (0)
	{}
	int topPad;
	int rightPad;
	int bottomPad;
	int leftPad;
};

class NSRPageCropper
{
public:
	enum NSRPixelOrder {
		NSR_PIXEL_ORDER_RGB	= 0,
		NSR_PIXEL_ORDER_ARGB	= 1,
		NSR_PIXEL_ORDER_BGR	= 2
	};

	static NSRCropPads findCropPads (unsigned char *data, NSRPixelOrder order, int width, int height, int stride);

private:
	NSRPageCropper ();
};

#endif /* NSRPAGECROPPER_H_ */
