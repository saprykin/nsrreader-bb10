#ifndef NSRLASTDOCSPAGE_H_
#define NSRLASTDOCSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/GridListLayout>
#include <bb/cascades/OrientationSupport>

class NSRLastDocsPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRLastDocsPage (QObject *parent = 0);
	virtual ~NSRLastDocsPage ();

Q_SIGNALS:
	void requestDocument (const QString& path);

private Q_SLOTS:
	void onOrientationAboutToChange (bb::cascades::UIOrientation::Type type);
	void onListItemTriggered (QVariantList indexPath);

private:
	void loadData ();

	bb::cascades::ListView		*_listView;
	bb::cascades::GridListLayout	*_listLayout;
};

#endif /* NSRLASTDOCSPAGE_H_ */
