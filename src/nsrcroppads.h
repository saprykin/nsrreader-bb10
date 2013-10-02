#ifndef NSRCROPPADS_H_
#define NSRCROPPADS_H_

class NSRCropPads
{
public:
	NSRCropPads ();
	~NSRCropPads ();

	inline double getScale () const {
		return _scale;
	}

	inline int getLeft () const {
		return _left;
	}

	inline int getRight () const {
		return _right;
	}

	inline int getTop () const {
		return _top;
	}

	inline int getBottom () const {
		return _bottom;
	}

	void setScale (double scale);

	inline void setLeft (int left) {
		_left = left * _scale;
	}

	inline void setRight (int right) {
		_right = right * _scale;
	}

	inline void setTop (int top) {
		_top = top * _scale;
	}

	inline void setBottom (int bottom) {
		_bottom = bottom * _scale;
	}

	void rotateLeft ();
	void rotateRight ();

private:
	double	_scale;
	int	_left;
	int	_right;
	int	_top;
	int	_bottom;
};

#endif /* NSRCROPPADS_H_ */
