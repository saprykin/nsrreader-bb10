#ifndef NSRLASTDOCSPAGE_H_
#define NSRLASTDOCSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/GridListLayout>
#include <bb/cascades/OrientationSupport>

#include "nsrlastdocslistview.h"

class NSRLastDocsPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRLastDocsPage (QObject *parent = 0);
	virtual ~NSRLastDocsPage ();

	void finishToast ();

Q_SIGNALS:
	void requestDocument (const QString& path);
	void documentToBeDeleted (const QString& path);

private Q_SLOTS:
	void onOrientationAboutToChange (bb::cascades::UIOrientation::Type type);
	void onListItemTriggered (QVariantList indexPath);
	void onModelUpdated (bool isEmpty);

private:
	void loadData ();

	NSRLastDocsListView		*_listView;
	bb::cascades::GridListLayout	*_listLayout;
	bb::cascades::Label		*_emptyLabel;
};

#endif /* NSRLASTDOCSPAGE_H_ */
