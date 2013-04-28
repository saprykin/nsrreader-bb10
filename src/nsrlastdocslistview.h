#ifndef NSRLASTDOCSLISTVIEW_H_
#define NSRLASTDOCSLISTVIEW_H_

#include <bb/cascades/ListView>
#include <bb/cascades/Container>

#include <bb/system/InvokeTargetReply>

#include <QObject>

class NSRLastDocsListView : public bb::cascades::ListView
{
	Q_OBJECT
public:
	NSRLastDocsListView (bb::cascades::Container *parent = 0);
	virtual ~NSRLastDocsListView ();

Q_SIGNALS:
	void documentRequested (const QString& path);

public Q_SLOTS:
	void onRemoveActionTriggered ();
	void onOpenActionTriggered ();
	void onShareActionTriggered ();

private:
	bb::system::InvokeTargetReply	*_invokeTargetReply;
};

#endif /* NSRLASTDOCSLISTVIEW_H_ */
