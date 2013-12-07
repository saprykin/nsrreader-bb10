#include "nsrbookmarkitemfactory.h"

#include <bb/cascades/StandardListItem>

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
	Q_UNUSED (list);

	return new StandardListItem ();
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
	StandardListItem *item = static_cast<StandardListItem *> (listItem);

	item->setTitle (map["title"].toString ());
	item->setDescription (map["description"].toString ());
}
