#include "nsrcroppads.h"

#include <qmath.h>
#include <float.h>

NSRCropPads::NSRCropPads () :
	_scale (1.0),
	_left (0),
	_right (0),
	_top (0),
	_bottom (0)
{
}

NSRCropPads::~NSRCropPads ()
{
}

void
NSRCropPads::setScale (double scale)
{
	if (scale < 0)
		return;

	if (qAbs (scale) <= DBL_EPSILON) {
		_left	= 0.0;
		_right	= 0.0;
		_top	= 0.0;
		_bottom	= 0.0;
	} else {
		double ratio = qAbs (_scale) <= DBL_EPSILON ? scale : scale / _scale;

		_left	*= ratio;
		_right	*= ratio;
		_top	*= ratio;
		_bottom	*= ratio;
	}

	_scale = scale;
}
