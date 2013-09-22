#ifndef NSRTEXTDOCUMENT_H
#define NSRTEXTDOCUMENT_H

#include "nsrabstractdocument.h"

#include <QMap>

class NSRTextDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRTextDocument(const QString& file, QObject *parent = 0);
	~NSRTextDocument();

	int getNumberOfPages () const;
	void renderPage (int page);
	bb::ImageData getCurrentPage ();
	bool isValid ()	const;
	double getMaxZoom ();
	double getMinZoom ();
	void setTextOnly (bool textOnly);
	QString getText ();
	bool isEncodingUsed () const;
	bool isAutoCrop () const;

private:
	QString			_text;
	int			_pagesCount;
};

#endif // NSRTEXTDOCUMENT_H
