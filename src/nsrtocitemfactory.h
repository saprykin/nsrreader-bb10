#ifndef NSRTOCITEMFACTORY_H_
#define NSRTOCITEMFACTORY_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>

#include <QObject>

class NSRTocItemFactory : public bb::cascades::ListItemProvider
{
	Q_OBJECT
public:
	NSRTocItemFactory ();
	virtual ~NSRTocItemFactory ();

	bb::cascades::VisualNode * createItem (bb::cascades::ListView*	list,
					       const QString&		type);
	void updateItem (bb::cascades::ListView*	list,
			 bb::cascades::VisualNode*	listItem,
			 const QString&			type,
			 const QVariantList&		indexPath,
			 const QVariant&		data);
};

#endif /* NSRTOCITEMFACTORY_H_ */
