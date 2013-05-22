#ifndef NSRSESSION_H
#define NSRSESSION_H

#include <QtCore>

class NSRSession
{
public:
	NSRSession ();
	NSRSession (const QString& file,
		    int page,
		    int zoomText,
		    double zoomGraphic,
		    bool isFitToWidth,
		    const QPointF& pos,
		    const QPointF& textPos,
		    double angle);

	inline QString getFile () const {return _file;}
	inline int getPage () const {return _page;}
	inline int getZoomText () const {return _zoomText;}
	inline double getZoomGraphic () const {return _zoomGraphic;}
	inline int getZoomScreenWidth () const {return _zoomScreenWidth;}
	inline bool isFitToWidth () const {return _isFitToWidth;}
	inline QPointF getPosition () const {return _pos;}
	inline QPointF getTextPosition () const {return _textPos;}
	inline double getRotation () const {return _angle;}

	inline void setFile (const QString& file) {_file = file;}
	inline void setPage (int page) {_page = page;}
	inline void setZoomText (int zoom) {_zoomText = zoom;}
	inline void setZoomGraphic (double zoom) {_zoomGraphic = zoom;}
	inline void setZoomScreenWidth (int width) {_zoomScreenWidth = width;}
	inline void setFitToWidth (bool fit) {_isFitToWidth = fit;}
	inline void setPosition (const QPointF& pos) {_pos = pos;}
	inline void setTextPosition (const QPointF& pos) {_textPos = pos;}
	inline void setRotation (double angle) {_angle = angle;}

private:
	QString _file;
	QPointF	_pos;
	QPointF	_textPos;
	double	_zoomGraphic;
	double	_angle;
	int	_page;
	int	_zoomText;
	int	_zoomScreenWidth;
	bool	_isFitToWidth;
};

#endif // NSRSESSION_H
