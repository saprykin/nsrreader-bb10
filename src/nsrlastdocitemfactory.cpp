#include "nsrlastdocitemfactory.h"
#include "nsrlastdocitem.h"
#include "nsrlastdocslistview.h"
#include "nsrfilesharer.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/ActionSet>

using namespace bb::cascades;

NSRLastDocItemFactory::NSRLastDocItemFactory ()
{
}

NSRLastDocItemFactory::~NSRLastDocItemFactory ()
{
}

bb::cascades::VisualNode*
NSRLastDocItemFactory::createItem (bb::cascades::ListView*	list,
				   const QString&		type)
{
	Q_UNUSED (type);

	NSRLastDocsListView *listView = static_cast<NSRLastDocsListView *> (list);
	NSRLastDocItem *item = new NSRLastDocItem ();

	ActionSet *actionSet = ActionSet::create().subtitle (trUtf8 ("File"));

	ActionItem *openAction = ActionItem::create().title (trUtf8 ("Open"));
	ActionItem *shareAction = ActionItem::create().title (trUtf8 ("Share"));
	ActionItem *removeAction = ActionItem::create().title (trUtf8 ("Remove from List"));

	openAction->setImageSource (QUrl ("asset:///open.png"));
	shareAction->setImageSource (QUrl ("asset:///share.png"));
	removeAction->setImageSource (QUrl ("asset:///delete.png"));

	connect (openAction, SIGNAL (triggered ()), listView, SLOT (onOpenActionTriggered ()));
	connect (shareAction, SIGNAL (triggered ()), listView, SLOT (onShareActionTriggered ()));
	connect (removeAction, SIGNAL (triggered ()), listView, SLOT (onRemoveActionTriggered ()));

	actionSet->add (openAction);
	actionSet->add (shareAction);
	actionSet->add (removeAction);

	item->addActionSet (actionSet);

	return item;
}

void
NSRLastDocItemFactory::updateItem (bb::cascades::ListView*	list,
                		   bb::cascades::VisualNode*	listItem,
                		   const QString&		type,
                		   const QVariantList&		indexPath,
                		   const QVariant&		data)
{
	Q_UNUSED (list);
	Q_UNUSED (type);
	Q_UNUSED (indexPath);

	QString docPath;
	QVariantMap map = data.value<QVariantMap> ();
	NSRLastDocItem *item = static_cast<NSRLastDocItem *> (listItem);

	docPath = map["path"].toString ();

	if (item->actionSetCount () > 0) {
		item->actionSetAt(0)->setTitle (map["title"].toString ());
		item->actionSetAt(0)->at(1)->setEnabled (NSRFileSharer::isSharable (docPath));

		item->actionSetAt(0)->at(0)->setProperty ("document-path", docPath);
		item->actionSetAt(0)->at(1)->setProperty ("document-path", docPath);
		item->actionSetAt(0)->at(2)->setProperty ("document-path", docPath);
	}

	item->updateItem (map["title"].toString (),
			  map["image"].toString (),
			  map["text"].toString (),
			  map["path"].toString (),
			  map["encrypted"].toBool ());
}


