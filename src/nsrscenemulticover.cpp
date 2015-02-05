#include "nsrscenemulticover.h"

using namespace bb::cascades;

NSRSceneMultiCover::NSRSceneMultiCover (QObject *parent) :
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	MultiCover (parent),
#else
	QObject (parent),
#endif
	_fullCover (NULL),
	_compactCover (NULL)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover = new NSRSceneCover (NSRSceneCover::NSR_COVER_MODE_FULL, this);
	_compactCover = new NSRSceneCover (NSRSceneCover::NSR_COVER_MODE_COMPACT, this);

	add (_fullCover, CoverDetailLevel::High);
	add (_compactCover, CoverDetailLevel::Medium);
#endif
}

NSRSceneMultiCover::~NSRSceneMultiCover ()
{
}

void
NSRSceneMultiCover::setPageData (const NSRRenderedPage&	page,
                		 const QString& 	title,
                		 int 			pagesTotal)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->setPageData (page, title, pagesTotal);
	_compactCover->setPageData (page, title, pagesTotal);
#else
	Q_UNUSED (page);
	Q_UNUSED (title);
	Q_UNUSED (pagesTotal);
#endif
}

void
NSRSceneMultiCover::resetPageData ()
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->resetPageData ();
	_compactCover->resetPageData ();
#endif
}

void
NSRSceneMultiCover::updateState (bool isStatic)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->updateState (isStatic);
	_compactCover->updateState (isStatic);
#else
	Q_UNUSED (isStatic);
#endif
}

void
NSRSceneMultiCover::setTextOnly (bool textOnly)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->setTextOnly (textOnly);
	_compactCover->setTextOnly (textOnly);
#else
	Q_UNUSED (textOnly);
#endif
}

void
NSRSceneMultiCover::setInvertedColors (bool invertedColors)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->setInvertedColors (invertedColors);
	_compactCover->setInvertedColors (invertedColors);
#else
	Q_UNUSED (invertedColors);
#endif
}

void
NSRSceneMultiCover::setTextTheme (NSRReadingTheme::Type type)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_fullCover->setTextTheme (type);
	_compactCover->setTextTheme (type);
#else
	Q_UNUSED (type);
#endif
}
