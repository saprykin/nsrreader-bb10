#include "nsrlastdocitemfactory.h"
#include "nsrlastdocitem.h"

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
	Q_UNUSED (list);
	Q_UNUSED (type);

	return new NSRLastDocItem ();
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

	QVariantMap map = data.value<QVariantMap>();
	NSRLastDocItem *item = static_cast<NSRLastDocItem *> (listItem);

	item->updateItem (map["title"].toString (), map["image"].toString ());
}


