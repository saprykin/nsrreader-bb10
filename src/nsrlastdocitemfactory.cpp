#include "nsrlastdocitemfactory.h"
#include "nsrlastdocitem.h"
#include "nsrlastdocslistview.h"
#include "nsrfilesharer.h"

#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/ActionSet>

#include <bbndk.h>

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
	NSRTranslator *translator = item->getTranslator ();

	ActionSet *actionSet = ActionSet::create().subtitle (trUtf8 ("File"));

	ActionItem *shareAction = ActionItem::create().title (trUtf8 ("Share"));
	ActionItem *hideAction = ActionItem::create().title (trUtf8 ("Clear Recent", "Clear recent files"));
	DeleteActionItem *removeAction = DeleteActionItem::create ();

	shareAction->setImageSource (QUrl ("asset:///share.png"));
	hideAction->setImageSource (QUrl ("asset:///list-remove.png"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	shareAction->accessibility()->setName (trUtf8 ("Share file with others"));
	hideAction->accessibility()->setName (trUtf8 ("Remove file from the recent list only"));

	translator->addTranslatable ((UIObject *) shareAction->accessibility (),
				     NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				     QString ("NSRLastDocItemFactory"),
				     QString ("Share file with others"));
	translator->addTranslatable ((UIObject *) hideAction->accessibility (),
				     NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				     QString ("NSRLastDocItemFactory"),
				     QString ("Remove file from the recent list only"));
#  endif
#endif

	bool ok = connect (shareAction, SIGNAL (triggered ()), listView, SLOT (onShareActionTriggered ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (removeAction, SIGNAL (triggered ()), listView, SLOT (onRemoveActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (hideAction, SIGNAL (triggered ()), listView, SLOT (onHideActionTriggered ()));
	Q_ASSERT (ok);

	actionSet->add (shareAction);
	actionSet->add (hideAction);
	actionSet->add (removeAction);

	item->addActionSet (actionSet);

	translator->addTranslatable ((UIObject *) shareAction,
				     NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				     QString ("NSRLastDocItemFactory"),
				     QString ("Share"));
	translator->addTranslatable ((UIObject *) hideAction,
				     NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				     QString ("NSRLastDocItemFactory"),
				     QString ("Clear Recent"));
	translator->addTranslatable ((UIObject *) actionSet,
				     NSRTranslator::NSR_TRANSLATOR_TYPE_ACTIONSET_SUBTITLE,
				     QString ("NSRLastDocItemFactory"),
				     QString ("File"));

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


