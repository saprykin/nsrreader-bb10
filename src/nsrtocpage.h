#ifndef NSRTOCPAGE_H_
#define NSRTOCPAGE_H_

#include "nsrtranslator.h"
#include "nsrtocentry.h"

#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/GroupDataModel>

#include <QObject>

class NSRTocPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRTocPage (bb::cascades::NavigationPane *naviPane, QObject *parent = 0);
	virtual ~NSRTocPage ();

	void setToc (NSRTocEntry *toc);

Q_SIGNALS:
	void pageRequested (int page);
	void subtreeRequested (const NSRTocEntry *toc);

public Q_SLOTS:
	void onDocumentOpened (const QString& file);
	void onDocumentClosed ();
	void onSubtreeRequested (const NSRTocEntry *toc);

private Q_SLOTS:
	void updateUi ();
	void onListItemTriggered (QVariantList indexPath);
	void onDynamicDUFactorChanged (float dduFactor);

private:
	void unloadData ();

	NSRTranslator *			_translator;
	NSRTocEntry *			_toc;
	bb::cascades::NavigationPane *  _naviPane;
	bb::cascades::ListView *	_listView;
	bb::cascades::GroupDataModel *	_model;
	bb::cascades::Container *	_emptyContainer;
	bb::cascades::Label		*_noTocLabel;
	bb::cascades::Label		*_noFileLabel;
};

#endif /* NSRTOCPAGE_H_ */
