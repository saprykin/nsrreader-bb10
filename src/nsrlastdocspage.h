#ifndef NSRLASTDOCSPAGE_H_
#define NSRLASTDOCSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/GridListLayout>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/Container>

#include "nsrlastdocslistview.h"
#include "nsrtranslator.h"

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

public Q_SLOTS:
	void onDocumentOpened (const QString& file);
	void onDocumentPageRendered ();

private Q_SLOTS:
	void onOrientationAboutToChange (bb::cascades::UIOrientation::Type type);
	void onListItemTriggered (QVariantList indexPath);
	void onModelUpdated (bool isEmpty);

private:
	void loadData ();
	QVariant createModelItem (const QString& file);

	QString				_lastOpenedFile;
	NSRTranslator			*_translator;
	NSRLastDocsListView		*_listView;
	bb::cascades::GridListLayout	*_listLayout;
	bb::cascades::Container		*_emptyContainer;
	bool				_prepareForUpdate;
};

#endif /* NSRLASTDOCSPAGE_H_ */
