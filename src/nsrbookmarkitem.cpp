#include "nsrbookmarkitem.h"
#include "nsrglobalnotifier.h"

using namespace bb::cascades;

NSRBookmarkItem::NSRBookmarkItem (bb::cascades::Container *parent) :
	StandardListItem (parent),
	_translator (NULL)
{
	_translator = new NSRTranslator (this);

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			  _translator, SLOT (translate ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);
}

NSRBookmarkItem::~NSRBookmarkItem ()
{
}

