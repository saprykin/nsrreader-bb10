#include "nsrabstractdocument.h"

#include <QTextCodec>

NSRAbstractDocument::NSRAbstractDocument(const QString& file, QObject *parent) :
	QObject (parent),
	_docPath (file),
	_zoom (100.0),
	_screenWidth (360),
	_zoomToWidth (false),
	_textOnly (false),
	_invertedColors (false),
	_lastError (NSR_DOCUMENT_ERROR_NO),
	_encoding ("UTF-8"),
	_rotation (0),
	_maxPageSize (4000, 4000)
{
}

NSRAbstractDocument::~NSRAbstractDocument()
{
}

void NSRAbstractDocument::zoomToWidth(int screenWidth)
{
	if (screenWidth < 0)
		_zoomToWidth = false;
	else {
		_screenWidth = screenWidth;
		_zoomToWidth = true;
	}
}

void NSRAbstractDocument::rotateLeft()
{
	_rotation -= 90;

	if (_rotation == -360)
		_rotation = 0;
}

void NSRAbstractDocument::rotateRight()
{
	_rotation += 90;

	if (_rotation == 360)
		_rotation = 0;
}

void NSRAbstractDocument::setRotation(int angle)
{
	_rotation = angle < 0 ? -((-angle) % 360) : angle % 360;
}

void NSRAbstractDocument::setEncoding(const QString &enc)
{
	if (QTextCodec::codecForName(enc.toAscii()) == NULL)
		return;
	else
		_encoding = enc;
}

void NSRAbstractDocument::setZoom(double zoom)
{
	if (zoom > getMaxZoom())
		zoom = getMaxZoom();
	else if (zoom < getMinZoom())
		zoom = getMinZoom();

	_zoom = zoom;
	_zoomToWidth = false;
}

QString NSRAbstractDocument::processText(const QString &text)
{
	int		size;
	QString		buf;
	QRegExp		endSigns;

	size = text.length();
	endSigns = QRegExp("[\\.\\;\\.{3}\\)]");

	for (int i = 0; i < size; ++i) {
		/* Check for previous spaces */
		if (text.at(i) == ' ') {
			if (i > 0 && buf.size() > 0 &&
			    (buf.at(buf.size() - 1) == ' ' || buf.at(buf.size() - 1) == '\n'))
				continue;

		}

		/* Check for extra new lines */
		if (text.at(i) == '\n' && i < size - 1 && buf.size() > 0) {
			int nextIdx = i + 1;

			/* Skip foreward spaces */
			while (nextIdx < (size - 1) && text.at(nextIdx) == ' ')
				++nextIdx;

			if ((endSigns.indexIn(buf.at(buf.size() - 1)) == -1 || buf.at(buf.size() - 1).isSpace()) &&
			     text.at(nextIdx).isLower()) {

				if (buf.at(buf.size() - 1) != ' ')
					buf += ' ';

				continue;
			}
		}

		buf += text.at(i);

		if (text.at(i) == '\n')
			buf += "   ";
	}

	return buf;
}

double
NSRAbstractDocument::validateMaxZoom (const QSize& pageSize, double zoom) const
{
	QSize maxSize = getMaximumPageSize ();

	if (pageSize.width () * zoom / 100.0 <= maxSize.width () &&
	    pageSize.height () * zoom / 100.0 <= maxSize.height ())
		return zoom;

	double scale = qMin (maxSize.width () / (double) pageSize.width (),
			     maxSize.height () / (double) pageSize.height ());

	return scale * 100.0;
}

