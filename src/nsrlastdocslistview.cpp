#include "nsrlastdocslistview.h"
#include "nsrlastdocitem.h"
#include "nsrsettings.h"
#include "nsrfilesharer.h"
#include "nsrthumbnailer.h"

#include <bb/cascades/QListDataModel>

#include <QFile>

using namespace bb::cascades;

NSRLastDocsListView::NSRLastDocsListView (bb::cascades::Container *parent) :
	ListView (parent)
{
}

NSRLastDocsListView::~NSRLastDocsListView ()
{
}

void
NSRLastDocsListView::onRemoveActionTriggered ()
{
	if (sender()->userData (0) == NULL)
		return;

	NSRLastDocItem		*item = (NSRLastDocItem *) (sender()->userData (0));
	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());

	int count = model->size ();
	for (int i = 0; i < count; ++i)
		if (model->value(i).toMap()["path"].toString () == item->getDocumentPath ()) {
			NSRSettings().removeLastDocument (item->getDocumentPath ());
			QFile::remove (NSRThumbnailer::getThumnailPath (item->getDocumentPath ()));
			model->removeAt (i);
			break;
		}
}

void
NSRLastDocsListView::onOpenActionTriggered ()
{
	if (sender()->userData (0) == NULL)
		return;

	NSRLastDocItem *item = (NSRLastDocItem *) (sender()->userData (0));

	emit documentRequested (item->getDocumentPath ());
}

void
NSRLastDocsListView::onShareActionTriggered ()
{
	if (sender()->userData (0) == NULL)
		return;

	NSRLastDocItem	*item = (NSRLastDocItem *) (sender()->userData (0));
	NSRFileSharer::getInstance()->shareFile (item->getDocumentPath ());
}
