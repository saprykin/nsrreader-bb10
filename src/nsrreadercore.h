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

	Q_INVOKABLE void openDocument (const QString &path);
	Q_INVOKABLE bool isDocumentOpened () const;
	Q_INVOKABLE void closeDocument ();
	Q_INVOKABLE bb::cascades::Image * getCurrentPage ();

private:
	NSRAbstractDocument	*_doc;
	bb::cascades::Image	* _currentPage;
};

#endif /* NSRREADERCORE_H_ */
