#include "nsrlastdocitemfactory.h"
#include "nsrlastdocitem.h"
#include "nsrlastdocslistview.h"
#include "nsrfilesharer.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
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

	ActionItem *shareAction = ActionItem::create().title (trUtf8 ("Share"));
	ActionItem *hideAction = ActionItem::create().title (trUtf8 ("Clear Recent", "Clear recent files"));
	DeleteActionItem *removeAction = DeleteActionItem::create ();

	shareAction->setImageSource (QUrl ("asset:///share.png"));
	hideAction->setImageSource (QUrl ("asset:///list-remove.png"));

	bool ok = connect (shareAction, SIGNAL (triggered ()), listView, SLOT (onShareActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (removeAction, SIGNAL (triggered ()), listView, SLOT (onRemoveActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (hideAction, SIGNAL (triggered ()), listView, SLOT (onHideActionTriggered ()));
	Q_ASSERT (ok);

	actionSet->add (shareAction);
	actionSet->add (hideAction);
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
		item->actionSetAt(0)->at(0)->setEnabled (NSRFileSharer::isSharable (docPath));
	}

	item->updateItem (map["title"].toString (),
			  map["image"].toString (),
			  map["text"].toString (),
			  map["path"].toString (),
			  map["encrypted"].toBool ());
}


