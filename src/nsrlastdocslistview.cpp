#include "nsrlastdocslistview.h"
#include "nsrlastdocitem.h"
#include "nsrsettings.h"
#include "nsrfilesharer.h"
#include "nsrthumbnailer.h"

#include <bb/cascades/QListDataModel>
#include <bb/system/SystemDialog>

#include <QFile>

using namespace bb::cascades;
using namespace bb::system;

NSRLastDocsListView::NSRLastDocsListView (bb::cascades::Container *parent) :
	ListView (parent),
	_toast (NULL)
{
}

NSRLastDocsListView::~NSRLastDocsListView ()
{
	finishToast ();
}

void
NSRLastDocsListView::onRemoveActionTriggered ()
{
	if (sender () == NULL)
		return;

	QString			docPath = sender()->property("document-path").toString ();
	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());

	if (docPath.isEmpty ())
		return;

	SystemDialog *dialog = new SystemDialog (this);
	dialog->setTitle (trUtf8 ("Delete"));
	dialog->setBody (trUtf8("Delete selected files: %1?").arg (1));
	dialog->confirmButton()->setLabel (trUtf8 ("Delete"));
	dialog->cancelButton()->setLabel (trUtf8 ("Cancel"));
	dialog->setProperty ("document-path", docPath);


	Q_ASSERT (connect (dialog, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onSystemDialogFinished (bb::system::SystemUiResult::Type))));

	int count = model->size ();
	for (int i = 0; i < count; ++i)
		if (model->value(i).toMap()["path"].toString () == docPath) {
			dialog->setProperty ("model-index", i);
			break;
		}

	dialog->exec ();
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

void
NSRLastDocsListView::onHideActionTriggered ()
{
	if (sender () == NULL)
		return;

	QString			docPath = sender()->property("document-path").toString ();
	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());

	if (docPath.isEmpty ())
		return;

	if (_toast != NULL)
		finishToast ();

	_toast = new SystemToast (this);
	_toast->setBody (trUtf8("Recent cleared: %1").arg (1));
	_toast->button()->setLabel (trUtf8 ("Undo"));
	_toast->setPosition (SystemUiPosition::BottomCenter);
	_toast->setProperty ("document-path", docPath);

	Q_ASSERT (connect (_toast, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onToastFinished (bb::system::SystemUiResult::Type))));

	int count = model->size ();
	for (int i = 0; i < count; ++i)
		if (model->value(i).toMap()["path"].toString () == docPath) {
			_toast->setProperty ("model-record", model->value (i));
			_toast->setProperty ("model-index", i);
			model->removeAt (i);
			break;
		}

	if (model->size () == 0)
		emit modelUpdated (true);

	_toast->show ();
}

void
NSRLastDocsListView::finishToast ()
{
	if (_toast != NULL) {
		_toast->cancel ();
		onToastFinished (SystemUiResult::TimeOut);
	}
}

void
NSRLastDocsListView::onToastFinished (bb::system::SystemUiResult::Type result)
{
	if (_toast == NULL)
		return;

	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());
	QString			docPath = _toast->property("document-path").toString ();

	if (result == SystemUiResult::ButtonSelection) {
		model->insert (_toast->property("model-index").toInt (),
			       _toast->property("model-record"));
		emit modelUpdated (false);
	} else {
		NSRSettings().removeLastDocument (docPath);
		NSRThumbnailer::removeThumbnail (docPath);
	}

	_toast->deleteLater ();
	_toast = NULL;
}

void
NSRLastDocsListView::onSystemDialogFinished (bb::system::SystemUiResult::Type result)
{
	if (sender () == NULL)
		return;

	SystemDialog *dialog = static_cast<SystemDialog *> (sender ());

	if (result == SystemUiResult::ConfirmButtonSelection) {
		QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());
		QString			docPath = dialog->property("document-path").toString ();

		model->removeAt (dialog->property("model-index").toInt ());
		NSRSettings().removeLastDocument (docPath);
		NSRThumbnailer::removeThumbnail (docPath);
		emit documentToBeDeleted (docPath);
		QFile::remove (docPath);

		if (model->size () == 0)
			emit modelUpdated (true);
	}

	dialog->deleteLater ();
}
