#include "nsrbookmarkslistview.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/MultiSelectHandler>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/GroupDataModel>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemDialog>

#include <QVariantList>

using namespace bb::cascades;
using namespace bb::system;

NSRBookmarksListView::NSRBookmarksListView (bb::cascades::Container *parent) :
	ListView (parent)
{
	setMultiSelectAction (MultiSelectActionItem::create ());
	multiSelectHandler()->addAction (DeleteActionItem::create().onTriggered (this, SLOT (onRemoveActionTriggered ())));

	bool ok = connect (this, SIGNAL (selectionChanged (QVariantList, bool)),
			   this, SLOT (onSelectionChanged ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      this, SLOT (onSelectionChanged ()));
	Q_ASSERT (ok);
}

NSRBookmarksListView::~NSRBookmarksListView ()
{
}

void
NSRBookmarksListView::onEditActionTriggered ()
{
	if (selectionList().count () == 0)
		return;

	GroupDataModel *model = static_cast < GroupDataModel * > (dataModel ());
	SystemPrompt *prompt = new SystemPrompt (this);

	prompt->setTitle (trUtf8 ("Enter Bookmark"));
	prompt->inputField()->setEmptyText (trUtf8 ("Enter bookmark title"));
	prompt->inputField()->setDefaultText (model->data(selectionList().first().toList ()).toMap()["title"].toString ());
	prompt->setDismissAutomatically (false);
	prompt->setProperty ("data", selectionList().first().toList ());

	bool res = connect (prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			    this, SLOT (onEditDialogFinished (bb::system::SystemUiResult::Type)));

	if (res)
		prompt->show ();
	else {
		prompt->deleteLater ();
		prompt = NULL;
	}
}

void
NSRBookmarksListView::onRemoveActionTriggered ()
{
	int count = selectionList().count ();

	if (count == 0)
		return;

	SystemDialog *dialog = new SystemDialog (this);
	dialog->setTitle (trUtf8 ("Delete"));
	dialog->setBody (trUtf8("Delete selected bookmarks: %1?").arg (count));
	dialog->confirmButton()->setLabel (trUtf8 ("Delete"));
	dialog->cancelButton()->setLabel (trUtf8 ("Cancel"));
	dialog->setProperty ("undo-data", selectionList ());

	bool ok = connect (dialog, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onRemoveDialogFinished (bb::system::SystemUiResult::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	dialog->show ();
}

void
NSRBookmarksListView::onEditDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (sender () == NULL)
		return;

	SystemPrompt *prompt = static_cast<SystemPrompt *> (sender ());
	GroupDataModel *model = static_cast < GroupDataModel * > (dataModel ());

	if (res == SystemUiResult::ConfirmButtonSelection) {
		QVariantList list = prompt->property("data").toList ();
		QVariantMap map = model->data(list).toMap ();

		map["title"] = prompt->inputFieldTextEntry ();
		model->updateItem (list, map);

		emit modelUpdated ();
	}

	prompt->deleteLater ();
}

void
NSRBookmarksListView::onRemoveDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (sender () == NULL)
		return;

	SystemDialog *dialog = static_cast<SystemDialog *> (sender ());
	GroupDataModel *model = static_cast < GroupDataModel * > (dataModel ());

	if (res == SystemUiResult::ConfirmButtonSelection) {
		QVariantList list = dialog->property("undo-data").toList ();
		int count = list.count ();

		for (int i = count - 1; i >= 0; --i) {
			int page = model->data(list.at(i).toList ()).toMap()["page-number"].toInt ();
			model->removeAt (list.at(i).toList ());
			emit bookmarkChanged (page, true);
		}

		emit modelUpdated ();
	}

	dialog->deleteLater ();
}

void
NSRBookmarksListView::onSelectionChanged ()
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
