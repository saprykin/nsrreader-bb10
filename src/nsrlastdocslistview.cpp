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
	if (sender ()== NULL)
		return;

	QString			docPath = sender()->property("document-path").toString ();
	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());

	if (docPath.isEmpty ())
		return;

	int count = model->size ();
	for (int i = 0; i < count; ++i)
		if (model->value(i).toMap()["path"].toString () == docPath) {
			NSRSettings().removeLastDocument (docPath);
			QFile::remove (NSRThumbnailer::getThumnailPath (docPath));
			model->removeAt (i);
			break;
		}
}

void
NSRLastDocsListView::onOpenActionTriggered ()
{
	if (sender () == NULL)
		return;

	QString docPath = sender()->property("document-path").toString ();

	if (docPath.isEmpty ())
		return;

	emit documentRequested (docPath);
}

void
NSRLastDocsListView::onShareActionTriggered ()
{
	if (sender () == NULL)
		return;

	QString docPath = sender()->property("document-path").toString ();

	if (docPath.isEmpty ())
		return;

	NSRFileSharer::getInstance()->shareFile (docPath);
}
