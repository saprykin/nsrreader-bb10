#include "nsrlastdocslistview.h"
#include "nsrlastdocitem.h"
#include "nsrsettings.h"
#include "nsrfilesharer.h"
#include "nsrthumbnailer.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/QListDataModel>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/MultiSelectHandler>
#include <bb/cascades/ActionItem>
#include <bb/cascades/DeleteActionItem>
#include <bb/system/SystemDialog>

#include <QFile>
#include <QVariant>

using namespace bb::cascades;
using namespace bb::system;

NSRLastDocsListView::NSRLastDocsListView (bb::cascades::Container *parent) :
	ListView (parent),
	_translator (NULL),
	_toast (NULL)
{
	_translator = new NSRTranslator (this);

	setMultiSelectAction (MultiSelectActionItem::create ());
	multiSelectHandler()->addAction (ActionItem::create().title(trUtf8 ("Clear Recent", "Clear recent files"))
							     .imageSource(QUrl ("asset:///list-remove.png"))
							     .onTriggered (this, SLOT (onHideActionTriggered ())));
	multiSelectHandler()->addAction (ActionItem::create().title(trUtf8 ("Share"))
							     .imageSource(QUrl ("asset:///share.png"))
							     .onTriggered (this, SLOT (onShareActionTriggered ())));
	multiSelectHandler()->addAction (DeleteActionItem::create().onTriggered (this, SLOT (onRemoveActionTriggered ())));

	bool ok = connect (this, SIGNAL (selectionChanged (QVariantList, bool)),
			   this, SLOT (onSelectionChanged ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	_translator->addTranslatable ((UIObject *) multiSelectHandler()->actionAt (0),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRLastDocsListView"),
				      QString ("Clear Recent"));
	_translator->addTranslatable ((UIObject *) multiSelectHandler()->actionAt (1),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRLastDocsListView"),
				      QString ("Share"));

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      _translator, SLOT (translate ()));
	Q_ASSERT (ok);

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      this, SLOT (onSelectionChanged ()));
	Q_ASSERT (ok);
}

NSRLastDocsListView::~NSRLastDocsListView ()
{
	finishToast ();
}

void
NSRLastDocsListView::onRemoveActionTriggered ()
{
	QVariantListDataModel *	model = static_cast < QVariantListDataModel * > (dataModel ());
	QVariantList 		list;

	int count = selectionList().count ();

	if (count == 0)
		return;

	for (int i = 0; i < count; ++i) {
		QMap<QString, QVariant> map;

		map["path"] = model->data(selectionList().at(i).toList ()).toMap()["path"].toString ();
		map["index"] = selectionList().at(i).toList().first().toInt ();

		list.append (QVariant (map));
	}

	SystemDialog *dialog = new SystemDialog (this);
	dialog->setTitle (trUtf8 ("Delete"));
	dialog->setBody (trUtf8("Delete selected files: %1?").arg (count));
	dialog->confirmButton()->setLabel (trUtf8 ("Delete"));
	dialog->cancelButton()->setLabel (trUtf8 ("Cancel"));
	dialog->setProperty ("undo-data", list);

	bool ok = connect (dialog, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onSystemDialogFinished (bb::system::SystemUiResult::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	dialog->show ();
}

void
NSRLastDocsListView::onShareActionTriggered ()
{
	QVariantListDataModel	*model = static_cast < QVariantListDataModel * > (dataModel ());
	QStringList		files;

	int count = selectionList().count ();

	if (count == 0)
		return;

	for (int i = 0; i < count; ++i)
		files.append (model->data(selectionList().at(i).toList ()).toMap()["path"].toString ());

	NSRFileSharer::getInstance()->shareFiles (files);
}

void
NSRLastDocsListView::onHideActionTriggered ()
{
	QVariantListDataModel *	model = static_cast < QVariantListDataModel * > (dataModel ());
	QVariantList 		list;

	int count = selectionList().count ();

	if (count == 0)
		return;

	if (_toast != NULL)
		finishToast ();

	for (int i = count - 1; i >= 0; --i) {
		QMap<QString, QVariant>	map;
		int			index = selectionList().at(i).toList().first().toInt ();

		map["path"] = model->data(selectionList().at(i).toList ()).toMap()["path"].toString ();
		map["index"] = index;
		map["record"] = model->value (index);

		model->removeAt (index);
		list.append (QVariant (map));
	}

	_toast = new SystemToast (this);
	_toast->setBody (trUtf8("Recent cleared: %1").arg (count));
	_toast->button()->setLabel (trUtf8 ("Undo"));
	_toast->setPosition (SystemUiPosition::BottomCenter);
	_toast->setProperty ("undo-data", list);

	bool ok = connect (_toast, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onToastFinished (bb::system::SystemUiResult::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

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

	QVariantList list = _toast->property("undo-data").toList ();

	if (result == SystemUiResult::ButtonSelection) {
		QVariantListDataModel *model = static_cast < QVariantListDataModel * > (dataModel ());

		for (int i = list.count () - 1; i >= 0; --i)
			model->insert (list.at(i).toMap()["index"].toInt (),
				       list.at(i).toMap()["record"]);

		emit modelUpdated (false);
	} else {
		for (int i = list.count () - 1; i >= 0; --i) {
			QString docPath = list.at(i).toMap()["path"].toString ();
			NSRSettings::instance()->removeLastDocument (docPath);
			NSRThumbnailer::instance()->removeThumbnail (docPath);
		}
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
		QVariantListDataModel *	model = static_cast < QVariantListDataModel * > (dataModel ());
		QVariantList 		list = dialog->property("undo-data").toList ();

		for (int i = list.count () - 1; i >= 0; --i) {
			QString docPath = list.at(i).toMap()["path"].toString ();
			model->removeAt (list.at(i).toMap()["index"].toInt ());
			NSRSettings::instance()->removeLastDocument (docPath);
			NSRThumbnailer::instance()->removeThumbnail (docPath);
			emit documentToBeDeleted (docPath);
			QFile::remove (docPath);
		}

		if (model->size () == 0)
			emit modelUpdated (true);
	}

	dialog->deleteLater ();
}

void
NSRLastDocsListView::onSelectionChanged ()
{
	int selectCount	= selectionList().count ();
	int actionCount	= multiSelectHandler()->actionCount ();

	for (int i = 0; i < actionCount; ++i)
		multiSelectHandler()->actionAt(i)->setEnabled (selectCount > 0);

	if (selectCount >= 1)
		multiSelectHandler()->setStatus (trUtf8 ("Selected: %1", "Items selected")
						 .arg (selectCount));
	else
		multiSelectHandler()->setStatus (trUtf8 ("No items selected"));
}
