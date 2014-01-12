#include "nsrbookmarkitem.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/ActionSet>

using namespace bb::cascades;

NSRBookmarkItem::NSRBookmarkItem (bb::cascades::Container *parent) :
	StandardListItem (parent),
	_translator (NULL),
	_page (0)
{
	_translator = new NSRTranslator (this);

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			  this, SLOT (retranslateUi ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);
}

NSRBookmarkItem::~NSRBookmarkItem ()
{
}

void
NSRBookmarkItem::setPageTitle (const QString& title)
{
	setTitle (title);

	if (actionSetCount () > 0)
		actionSetAt(0)->setTitle (title);
}

void
NSRBookmarkItem::setPageNumber (int page)
{
	_page = page;

	retranslateSubtitle ();
}

void
NSRBookmarkItem::retranslateUi ()
{
	retranslateSubtitle ();

	_translator->translate ();
}

void
NSRBookmarkItem::retranslateSubtitle ()
{
	setDescription (trUtf8("Page %1").arg (_page));

	if (actionSetCount () > 0)
		actionSetAt(0)->setSubtitle (description ());
}
