#include "nsrpagecropper.h"

#include <qmath.h>

#define NSR_CROP_PIXEL_THRESHOLD	2
#define NSR_CROP_MAX_PAD_PERCENT	0.15
#define NSR_CROP_AGGRESSIVE_VALUE	0.15

NSRPageCropper::NSRPageCropper ()
{
}

NSRCropPads
NSRPageCropper::findCropPads (unsigned char *data, NSRPixelOrder order, int width, int height, int stride)
{
	NSRCropPads	pads;
	unsigned char *	dataPtr = data;
	double		rDelta, gDelta, bDelta;
	int		badCount = 0;
	int		px;			/* Bytes in pixel 					*/
	int		pxShift;		/* Shift in bytes to the first color component in pixel	*/

	if (data == NULL || stride < width * 3 || width < 3 || height < 3)
		return pads;

	int maxWCrop = (int) (width * NSR_CROP_MAX_PAD_PERCENT);
	int maxHCrop = (int) (height * NSR_CROP_MAX_PAD_PERCENT);

	if (order == NSR_PIXEL_ORDER_RGB || order == NSR_PIXEL_ORDER_BGR) {
		px = 3;
		pxShift = 0;
	} else {
		px = 4;
		pxShift = 1;
	}

	/* Use mean of two corner pixels as background */
	unsigned char red = (dataPtr[pxShift] +
			     dataPtr[(width - 1) * px + pxShift] +
			     dataPtr[stride * (height - 1) + pxShift] +
			     dataPtr[stride * (height - 1) + (width - 1) * px + pxShift]) / 4;
	unsigned char green = (dataPtr[pxShift + 1] +
			       dataPtr[(width - 1) * px + pxShift + 1] +
			       dataPtr[stride * (height - 1) + pxShift + 1] +
			       dataPtr[stride * (height - 1) + (width - 1) * px + pxShift + 1]) / 4;
	unsigned char blue = (dataPtr[pxShift + 2] +
			      dataPtr[(width - 1) * px + pxShift + 2] +
			      dataPtr[stride * (height - 1) + pxShift + 2] +
			      dataPtr[stride * (height - 1) + (width - 1) * px + pxShift + 2]) / 4;

	/* First, find top crop pad */
	for (int row = 0; row < maxHCrop; ++row) {
		for (int col = 0; col < width; ++col) {
			rDelta = ((double) red - dataPtr[col * px + pxShift]) / red;
			gDelta = ((double) green - dataPtr[col * px + pxShift + 1]) / green;
			bDelta = ((double) blue - dataPtr[col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CROP_PIXEL_THRESHOLD) {
			pads.topPad = row + 1;
			break;
		} else if (row == maxHCrop - 1)
			pads.topPad = row + 1;

		dataPtr += stride;
	}

	/* Next, find bottom crop pad */

	dataPtr = data + stride * (height - 1);
	badCount = 0;

	for (int row = height - 1; row >= height - maxHCrop; --row) {
		for (int col = 0; col < width; ++col) {
			rDelta = ((double) red - dataPtr[col * px + pxShift]) / red;
			gDelta = ((double) green - dataPtr[col * px + pxShift + 1]) / green;
			bDelta = ((double) blue - dataPtr[col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CROP_PIXEL_THRESHOLD) {
			pads.bottomPad = height - row - 1;
			break;
		} else if (row == height - maxHCrop)
			pads.bottomPad = height - row - 1;

		dataPtr -= stride;
	}

	/* Next, find left crop pad */

	dataPtr = data;
	badCount = 0;

	for (int col = 0; col < maxWCrop; ++col) {
		for (int row = 0; row < height; ++row) {
			rDelta = ((double) red - dataPtr[row * stride + col * px + pxShift]) / red;
			gDelta = ((double) green - dataPtr[row * stride + col * px + pxShift + 1]) / green;
			bDelta = ((double) blue - dataPtr[row * stride + col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CROP_AGGRESSIVE_VALUE))
				++badCount;		}

		if (badCount > NSR_CROP_PIXEL_THRESHOLD) {
			pads.leftPad = col + 1;
			break;
		} else if (col == maxWCrop - 1)
			pads.leftPad = col + 1;
	}

	/* Finally, find right crop pad */

	dataPtr = data;
	badCount = 0;

	for (int col = width - 1; col >= width - maxWCrop; --col) {
		for (int row = 0; row < height; ++row) {
			rDelta = ((double) red - dataPtr[row * stride + col * px + pxShift]) / red;
			gDelta = ((double) green - dataPtr[row * stride + col * px + pxShift + 1]) / green;
			bDelta = ((double) blue - dataPtr[row * stride + col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CROP_PIXEL_THRESHOLD) {
			pads.rightPad = width - col - 1;
			break;
		} else if (col == width - maxWCrop)
			pads.rightPad = width - col - 1;
	}

	pads.topPad = qMax (pads.topPad - 5, 0);
	pads.rightPad = qMax (pads.rightPad - 5, 0);
	pads.bottomPad = qMax (pads.bottomPad - 5, 0);
	pads.leftPad = qMax (pads.leftPad - 5, 0);

	return pads;
}
