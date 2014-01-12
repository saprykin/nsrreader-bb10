#include "nsrbookmarkitemfactory.h"
#include "nsrbookmarkslistview.h"
#include "nsrbookmarkitem.h"
#include "nsrreader.h"

#include <bb/cascades/StandardListItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ActionSet>

using namespace bb::cascades;

NSRBookmarkItemFactory::NSRBookmarkItemFactory ()
{
}

NSRBookmarkItemFactory::~NSRBookmarkItemFactory ()
{
}

bb::cascades::VisualNode *
NSRBookmarkItemFactory::createItem (bb::cascades::ListView* list, const QString& type)
{
	Q_UNUSED (type);

	NSRBookmarkItem *item = new NSRBookmarkItem ();
	NSRBookmarksListView *listView = static_cast < NSRBookmarksListView * > (list);
	NSRTranslator *translator = item->getTranslator ();

	ActionSet *actionSet = ActionSet::create ();

	ActionItem *editAction = ActionItem::create().title (trUtf8 ("Edit Bookmark"));
	DeleteActionItem *removeAction = DeleteActionItem::create ();

	editAction->setImageSource (QUrl ("asset:///bookmark-edit.png"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	editAction->accessibility()->setName (trUtf8 ("Edit bookmark title"));

	translator->addTranslatable ((UIObject *) editAction->accessibility (),
				     NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				     QString ("NSRBookmarkItemFactory"),
				     QString ("Edit bookmark title"));
#  endif
#endif

	bool ok = connect (editAction, SIGNAL (triggered ()), listView, SLOT (onEditActionTriggered ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (removeAction, SIGNAL (triggered ()), listView, SLOT (onRemoveActionTriggered ()));
	Q_ASSERT (ok);

	actionSet->add (editAction);
	actionSet->add (removeAction);

	item->addActionSet (actionSet);

	translator->addTranslatable ((UIObject *) editAction,
				     NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				     QString ("NSRBookmarkItemFactory"),
				     QString ("Edit Bookmark"));
	translator->addTranslatable ((UIObject *) actionSet,
				     NSRTranslator::NSR_TRANSLATOR_TYPE_ACTIONSET_SUBTITLE,
				     QString ("NSRBookmarkItemFactory"),
				     QString ("Bookmark"));

	return item;
}

void
NSRBookmarkItemFactory::updateItem (bb::cascades::ListView * 	list,
				    bb::cascades::VisualNode * 	listItem,
				    const QString& 		type,
				    const QVariantList& 	indexPath,
				    const QVariant& 		data)
{
	Q_UNUSED (list);
	Q_UNUSED (type);
	Q_UNUSED (indexPath);

	QVariantMap map = data.value<QVariantMap> ();
	NSRBookmarkItem *item = static_cast < NSRBookmarkItem * > (listItem);

	item->setPageTitle (map["title"].toString ());
	item->setPageNumber (map["page-number"].toInt ());
}
