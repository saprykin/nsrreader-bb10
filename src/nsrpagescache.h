#ifndef NSRPAGESCACHE_H_
#define NSRPAGESCACHE_H_

#include "nsrrenderedpage.h"

#include <QObject>
#include <QHash>
#include <QList>

class NSRPagesCache : public QObject
{
	Q_OBJECT
public:
	NSRPagesCache (QObject *parent = 0);
	virtual ~NSRPagesCache ();

	bool isPageExists (int number);
	NSRRenderedPage getPage (int number);
	void addPage (NSRRenderedPage &page);
	void clearStorage ();

private:
	QHash<int, NSRRenderedPage>	_hash;
	QList<int>			_pages;
	qint64				_usedMemory;
};

#endif /* NSRPAGESCACHE_H_ */
