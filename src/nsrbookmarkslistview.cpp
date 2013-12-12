#include "nsrbookmarkslistview.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/DeleteActionItem>
#include <bb/cascades/MultiSelectHandler>
#include <bb/cascades/MultiSelectActionItem>
#include <bb/cascades/GroupDataModel>

#include <QVariantList>

using namespace bb::cascades;
using namespace bb::system;

NSRBookmarksListView::NSRBookmarksListView (bb::cascades::Container *parent) :
	ListView (parent),
	_toast (NULL),
	_prompt (NULL)
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
	finishToast ();
}

void
NSRBookmarksListView::onEditActionTriggered ()
{
	finishToast ();

	if (_prompt != NULL)
		return;

	if (selectionList().count () == 0)
		return;

	GroupDataModel *model = static_cast < GroupDataModel * > (dataModel ());

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter Bookmark"));
	_prompt->inputField()->setEmptyText (trUtf8 ("Enter bookmark title"));
	_prompt->inputField()->setDefaultText (model->data(selectionList().first().toList ()).toMap()["title"].toString ());
	_prompt->setDismissAutomatically (false);
	_prompt->setProperty ("data", selectionList().first().toList ());

	bool res = connect (_prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			    this, SLOT (onEditDialogFinished (bb::system::SystemUiResult::Type)));

	if (res)
		_prompt->show ();
	else {
		_prompt->deleteLater ();
		_prompt = NULL;
	}
}

void
NSRBookmarksListView::onRemoveActionTriggered ()
{
	GroupDataModel *	model = static_cast < GroupDataModel * > (dataModel ());
	QVariantList 		list;

	int count = selectionList().count ();

	if (count == 0)
		return;

	finishToast ();

	for (int i = count - 1; i >= 0; --i) {
		emit bookmarkChanged (model->data(selectionList().at(i).toList ()).toMap()["page-number"].toInt (), true);
		list.append (model->data (selectionList().at(i).toList ()));
		model->removeAt (selectionList().at(i).toList ());
	}

	_toast = new SystemToast (this);
	_toast->setBody (trUtf8("Bookmarks deleted: %1").arg (count));
	_toast->button()->setLabel (trUtf8 ("Undo"));
	_toast->setPosition (SystemUiPosition::BottomCenter);
	_toast->setProperty ("undo-data", list);

	bool ok = connect (_toast, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			   this, SLOT (onToastFinished (bb::system::SystemUiResult::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	_toast->show ();

	emit modelUpdated ();
}

void
NSRBookmarksListView::onEditDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (_prompt == NULL)
		return;

	GroupDataModel *model = static_cast < GroupDataModel * > (dataModel ());

	if (res == SystemUiResult::ConfirmButtonSelection) {
		QVariantList list = _prompt->property("data").toList ();
		QVariantMap map = model->data(list).toMap ();

		map["title"] = _prompt->inputFieldTextEntry ();
		model->updateItem (list, map);

		emit modelUpdated ();
	}

	_prompt->deleteLater ();
	_prompt = NULL;
}

void
NSRBookmarksListView::finishToast ()
{
	if (_toast != NULL) {
		_toast->cancel ();
		onToastFinished (SystemUiResult::TimeOut);
	}
}

void
NSRBookmarksListView::onToastFinished (bb::system::SystemUiResult::Type result)
{
	if (_toast == NULL)
		return;

	if (result == SystemUiResult::ButtonSelection) {
		QVariantList	list = _toast->property("undo-data").toList ();
		int		count = list.count ();
		GroupDataModel	*model = static_cast < GroupDataModel * > (dataModel ());

		for (int i = 0; i < count; ++i) {
			model->insert (list.at(i).toMap ());
			emit bookmarkChanged (list.at(i).toMap()["page-number"].toInt (), false);
		}

		emit modelUpdated ();
	}

	_toast->deleteLater ();
	_toast = NULL;
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
