#include "nsrlastdocslistview.h"
#include "nsrlastdocitem.h"
#include "nsrsettings.h"

#include <bb/cascades/QListDataModel>

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>

#include <QFileInfo>

using namespace bb::cascades;
using namespace bb::system;

NSRLastDocsListView::NSRLastDocsListView (bb::cascades::Container *parent) :
	ListView (parent),
	_invokeTargetReply (NULL)
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
	QVariantListDataModel	*model = static_cast<QVariantListDataModel *> (dataModel ());

	int count = model->size ();
	for (int i = 0; i < count; ++i)
		if (model->value(i).toMap()["path"].toString () == item->getDocumentPath ()) {
			NSRSettings().removeLastDocument (item->getDocumentPath ());
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
	QString		extension = QFileInfo(item->getDocumentPath ()).suffix().toLower ();
	QString		mimeType;
	InvokeManager	invokeManager;
	InvokeRequest	invokeRequest;

	if (extension == "pdf")
		mimeType = "application/pdf";
	else if (extension == "djvu" || extension == "djv")
		mimeType = "image/vnd.djvu";
	else if (extension == "tiff" || extension == "tif")
		mimeType = "image/tiff";
	else
		mimeType = "text/plain";

	invokeRequest.setMimeType (mimeType);
	invokeRequest.setUri (QUrl::fromLocalFile (item->getDocumentPath ()));
	invokeRequest.setAction ("bb.action.SHARE");

	_invokeTargetReply = invokeManager.invoke (invokeRequest);

	if (_invokeTargetReply != NULL) {
		_invokeTargetReply->setParent (this);
		connect (_invokeTargetReply, SIGNAL (finished ()), _invokeTargetReply, SLOT (deleteLater ()));
	}
}
