#ifndef NSRLASTDOCITEMFACTORY_H_
#define NSRLASTDOCITEMFACTORY_H_

#include <bb/cascades/ListItemProvider>
#include <bb/cascades/VisualNode>
#include <bb/cascades/ListView>

#include <QObject>

class NSRLastDocItemFactory : public bb::cascades::ListItemProvider
{
	Q_OBJECT
public:
	NSRLastDocItemFactory ();
	virtual ~NSRLastDocItemFactory ();

	bb::cascades::VisualNode * createItem (bb::cascades::ListView*	list,
					       const QString&		type);
	void updateItem (bb::cascades::ListView*	list,
			 bb::cascades::VisualNode*	listItem,
			 const QString&			type,
			 const QVariantList&		indexPath,
			 const QVariant&		data);
};

#endif /* NSRLASTDOCITEMFACTORY_H_ */
