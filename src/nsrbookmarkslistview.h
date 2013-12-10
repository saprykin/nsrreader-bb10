#ifndef NSRBOOKMARKSLISTVIEW_H_
#define NSRBOOKMARKSLISTVIEW_H_

#include <bb/cascades/ListView>
#include <bb/cascades/Container>
#include <bb/system/SystemToast>

#include <QObject>

class NSRBookmarksListView : public bb::cascades::ListView
{
	Q_OBJECT
public:
	NSRBookmarksListView (bb::cascades::Container *parent = 0);
	virtual ~NSRBookmarksListView ();

Q_SIGNALS:
	void bookmarkChanged (int page, bool removed);
	void modelUpdated ();

public Q_SLOTS:
	void onEditActionTriggered ();
	void onRemoveActionTriggered ();
	void finishToast ();

private Q_SLOTS:
	void onToastFinished (bb::system::SystemUiResult::Type result);
	void onSelectionChanged ();

private:
	bb::system::SystemToast *	_toast;
};

#endif /* NSRBOOKMARKSLISTVIEW_H_ */
