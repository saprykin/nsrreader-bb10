#include "nsrsession.h"

NSRSession::NSRSession () :
	_file (QString()),
	_pos (QPoint (0, 0)),
	_zoomGraphic (100.0),
	_angle (0.0),
	_page (0),
	_zoomText (100),
	_zoomScreenWidth (720),
	_isFitToWidth (false)
{

}

NSRSession::NSRSession (const QString& file,
			int page,
			int zoomText,
			double zoomGraphic,
			bool isFitToWidth,
			const QPoint& pos,
			double angle) :
	_file (file),
	_pos (pos),
	_zoomGraphic (zoomGraphic),
	_angle (angle),
	_page (page),
	_zoomText (zoomText),
	_isFitToWidth (isFitToWidth)
{
}
