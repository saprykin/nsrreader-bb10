#ifndef NSRLASTDOCSLISTVIEW_H_
#define NSRLASTDOCSLISTVIEW_H_

#include "nsrtranslator.h"

#include <bb/cascades/ListView>
#include <bb/cascades/Container>
#include <bb/system/SystemUiResult>
#include <bb/system/SystemToast>

#include <QObject>

class NSRLastDocsListView : public bb::cascades::ListView
{
	Q_OBJECT
public:
	NSRLastDocsListView (bb::cascades::Container *parent = 0);
	virtual ~NSRLastDocsListView ();

Q_SIGNALS:
	void modelUpdated (bool isEmpty);
	void documentToBeDeleted (const QString& path);

public Q_SLOTS:
	void onRemoveActionTriggered ();
	void onShareActionTriggered ();
	void onHideActionTriggered ();
	void finishToast ();

private Q_SLOTS:
	void onToastFinished (bb::system::SystemUiResult::Type result);
	void onSystemDialogFinished (bb::system::SystemUiResult::Type result);
	void onSelectionChanged ();

private:
	NSRTranslator *			_translator;
	bb::system::SystemToast *	_toast;
};

#endif /* NSRLASTDOCSLISTVIEW_H_ */
