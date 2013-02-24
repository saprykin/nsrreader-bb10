#include "nsrpagescache.h"

#include <bb/cascades/ImageTracker>

/* Maximum cache size in bytes (~100 MB by default) */
#define NSR_PAGES_CACHE_MAX_STORAGE 104857600

using namespace bb::cascades;

NSRPagesCache::NSRPagesCache (QObject *parent) :
		QObject (parent),
		_usedMemory (0)
{
}

NSRPagesCache::~NSRPagesCache ()
{
	_hash.clear ();
	_pagesByTime.clear ();
	_usedMemory = 0;
}

bool
NSRPagesCache::isPageExists (int number)
{
	return _hash.contains (number);
}

NSRRenderedPage
NSRPagesCache::getPage (int number)
{
	return _hash.value (number);
}

void
NSRPagesCache::addPage (NSRRenderedPage& page)
{
	NSRRenderedPage	oldPage;
	qint64		newSize;
	bool		inCache;

	if (!page.isValid ())
		return;

	oldPage = _hash.value (page.getNumber ());
	newSize = page.getSize().width () * page.getSize().height () * 4;
	inCache = oldPage.isValid ();

	while (_usedMemory + newSize > NSR_PAGES_CACHE_MAX_STORAGE &&
	       !_pagesByTime.isEmpty ()) {
		int deq = _pagesByTime.dequeue ();
		NSRRenderedPage rpage = _hash.take (deq);

		if (deq == rpage.getNumber ())
			inCache = false;

		_usedMemory -= (rpage.getSize().width () * rpage.getSize().height () * 4);
	}

	if (!inCache)
		_pagesByTime.enqueue (page.getNumber ());

	_hash.insert (page.getNumber (), page);
	_usedMemory += newSize;
}

void NSRPagesCache::clearStorage ()
{
	_hash.clear ();
	_pagesByTime.clear ();
	_usedMemory = 0;
}



