#include "nsrsession.h"

NSRSession::NSRSession () :
	_file (QString()),
	_pos (QPointF (0, 0)),
	_textPos (QPointF (0, 0)),
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
			const QPointF& pos,
			const QPointF& textPos,
			double angle) :
	_file (file),
	_pos (pos),
	_textPos (_textPos),
	_zoomGraphic (zoomGraphic),
	_angle (angle),
	_page (page),
	_zoomText (zoomText),
	_isFitToWidth (isFitToWidth)
{
}
