#ifndef NSRBOOKMARKSPAGE_H_
#define NSRBOOKMARKSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/GroupDataModel>
#include <bb/system/SystemToast>

#include "nsrbookmarkslistview.h"
#include "nsrtranslator.h"

class NSRBookmarksPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRBookmarksPage (QObject *parent = 0);
	virtual ~NSRBookmarksPage ();
	bool hasBookmark (int page, QString *title = 0);

Q_SIGNALS:
	void bookmarkChanged (int page, bool removed);

public Q_SLOTS:
	void onDocumentOpened (const QString& file);
	void onDocumentClosed ();
	void onDocumentToBeDeleted (const QString& file);
	void addBookmark (const QString& title, int page);
	void removeBookmark (int page);

private Q_SLOTS:
	void retranslateUi ();
	void onToastFinished (bb::system::SystemUiResult::Type result);
	void saveData ();
	void updateUi ();

private:
	void loadData (const QString& file);
	void unloadData ();
	void finishToast ();

	NSRTranslator *			_translator;
	NSRBookmarksListView *		_listView;
	bb::cascades::GroupDataModel *	_model;
	bb::cascades::Container *	_emptyContainer;
	bb::cascades::Label		*_noBookmarksLabel;
	bb::cascades::Label		*_noFileLabel;
	bb::system::SystemToast		*_toast;
	QString				_openedFile;
};

#endif /* NSRBOOKMARKSPAGE_H_ */
