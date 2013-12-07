#ifndef NSRBOOKMARKITEMFACTORY_H_
#define NSRBOOKMARKITEMFACTORY_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>

class NSRBookmarkItemFactory : public bb::cascades::ListItemProvider
{
	Q_OBJECT
public:
	NSRBookmarkItemFactory ();
	virtual ~NSRBookmarkItemFactory ();

	bb::cascades::VisualNode * createItem (bb::cascades::ListView*	list,
					       const QString&		type);
	void updateItem (bb::cascades::ListView*	list,
			 bb::cascades::VisualNode*	listItem,
			 const QString&			type,
			 const QVariantList&		indexPath,
			 const QVariant&		data);
};

#endif /* NSRBOOKMARKITEMFACTORY_H_ */
