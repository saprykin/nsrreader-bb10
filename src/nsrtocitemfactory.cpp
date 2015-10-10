#include "nsrtocitemfactory.h"
#include "nsrtocitem.h"

NSRTocItemFactory::NSRTocItemFactory ()
{
}

NSRTocItemFactory::~NSRTocItemFactory ()
{
}

bb::cascades::VisualNode *
NSRTocItemFactory::createItem (bb::cascades::ListView* list, const QString& type)
{
	Q_UNUSED (list);
	Q_UNUSED (type);

	return new NSRTocItem ();
}

void
NSRTocItemFactory::updateItem (bb::cascades::ListView * 	list,
			       bb::cascades::VisualNode * 	listItem,
			       const QString& 			type,
			       const QVariantList& 		indexPath,
			       const QVariant& 			data)
{
	Q_UNUSED (list);
	Q_UNUSED (type);
	Q_UNUSED (indexPath);

	QVariantMap map = data.value<QVariantMap> ();
	NSRTocItem *item = static_cast < NSRTocItem * > (listItem);

	item->setTocEntry ((const NSRTocEntry *) (map["entry"].value<void *> ()));
	item->setView (list);
}
