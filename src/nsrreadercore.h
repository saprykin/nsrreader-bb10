#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include <bb/cascades/Image>
#include <bb/ImageData>

#include "nsrabstractdocument.h"

class NSRReaderCore: public QObject
{
	Q_OBJECT

public:
	NSRReaderCore (QObject *parent = 0);
	virtual ~NSRReaderCore ();

	void openDocument (const QString &path);
	bool isDocumentOpened () const;
	void closeDocument ();
	bb::cascades::Image getCurrentPage ();

private:
	NSRAbstractDocument	*_doc;
	bb::cascades::Image	_currentPage;

};

#endif /* NSRREADERCORE_H_ */
