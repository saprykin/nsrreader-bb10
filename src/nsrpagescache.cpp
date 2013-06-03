#include "nsrpagescache.h"

/* Maximum cache size in bytes (~100 MB by default) */
#define NSR_PAGES_CACHE_MAX_STORAGE 104857600

NSRPagesCache::NSRPagesCache (QObject *parent) :
		QObject (parent),
		_usedMemory (0)
{
}

NSRPagesCache::~NSRPagesCache ()
{
	_hash.clear ();
	_pages.clear ();
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
	qint64		newSize;
	int		deqPage;

	if (!page.isValid ())
		return;

	newSize = page.getSize().width () * page.getSize().height () * 4;

	if (newSize > NSR_PAGES_CACHE_MAX_STORAGE)
		return;

	_hash.take (page.getNumber ());
	_pages.removeAll (page.getNumber ());

	while (_usedMemory + newSize > NSR_PAGES_CACHE_MAX_STORAGE &&
	       !_pages.isEmpty ()) {
		if (qAbs (page.getNumber () - _pages.first ()) <
		    qAbs (page.getNumber () - _pages.last ()))
			deqPage = _pages.takeLast ();
		else
			deqPage = _pages.takeFirst ();

		NSRRenderedPage rpage = _hash.take (deqPage);

		_usedMemory -= (rpage.getSize().width () * rpage.getSize().height () * 4);
	}

	_pages.append (page.getNumber ());
	qSort (_pages);

	_hash.insert (page.getNumber (), page);
	_usedMemory += newSize;
}

void NSRPagesCache::clearStorage ()
{
	_hash.clear ();
	_pages.clear ();
	_usedMemory = 0;
}
