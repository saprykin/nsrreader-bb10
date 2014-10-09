#include "nsrtranslator.h"
#include "nsrreader.h"

#include <bb/cascades/Label>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Button>
#include <bb/cascades/ActionSet>
#include <bb/cascades/Option>
#include <bb/cascades/MultiSelectHandler>
#include <bb/cascades/TitleBar>
#include <bb/cascades/Tab>
#include <bb/cascades/Header>
#include <bb/cascades/DropDown>

using namespace bb::cascades;

NSRTranslator::NSRTranslator (QObject *parent) :
	QObject (parent)
{
}

NSRTranslator::~NSRTranslator ()
{
	QList<NSRTranslatorInfo *> list;

	while (!list.isEmpty ())
		delete list.takeFirst ();
}

void
NSRTranslator::addTranslatable (bb::cascades::UIObject*	obj,
				NSRTranslatorType 	type,
				const QString&		context,
				const QString& 		text,
				const QString&		disamb)
{
	NSRTranslatorInfo *info;

	if (obj == NULL)
		return;

	removeTranslatable (obj);

	info = new NSRTranslatorInfo ();
	info->type = type;
	info->context = context;
	info->text = text;
	info->disamb = disamb;

	_hash.insert (obj, info);
}

void
NSRTranslator::removeTranslatable (bb::cascades::UIObject* obj)
{
	if (obj == NULL)
		return;

	NSRTranslatorInfo *info = _hash.take (obj);

	if (info != NULL)
		delete info;
}

QString
NSRTranslator::translatePath (const QString& path)
{
	QString ret = path;

	ret.replace (QRegExp ("^/accounts/[^/]*/shared"), trUtf8 ("Device"));
	ret.replace (QRegExp ("^/accounts/[^/]*/removable/sdcard"), trUtf8 ("Media Card"));

	return ret;
}

void
NSRTranslator::translate ()
{
	QList<UIObject *> list = _hash.keys ();
	int count = list.count ();

	for (int i = 0; i < count; ++i) {
		NSRTranslatorInfo *info = _hash.value (list.at (i));
		QString translated = QCoreApplication::translate (info->context.toAscii().data (),
								  info->text.toAscii().data (),
								  info->disamb.toAscii().data ());

		switch (info->type) {
		case NSR_TRANSLATOR_TYPE_LABEL:
		{
			Label *label = dynamic_cast<Label *> (list.at (i));

			if (label != NULL)
				label->setText (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_BUTTON:
		{
			Button *button = dynamic_cast<Button *> (list.at (i));

			if (button != NULL)
				button->setText (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_ACTION:
		{
			ActionItem *action = dynamic_cast<ActionItem *> (list.at (i));

			if (action != NULL)
				action->setTitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_ACTIONSET_TITLE:
		{
			ActionSet *actionSet = dynamic_cast<ActionSet *> (list.at (i));

			if (actionSet != NULL)
				actionSet->setTitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_ACTIONSET_SUBTITLE:
		{
			ActionSet *actionSet = dynamic_cast<ActionSet *> (list.at (i));

			if (actionSet != NULL)
				actionSet->setSubtitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_OPTION:
		{
			Option *option = dynamic_cast<Option *> (list.at (i));

			if (option != NULL)
				option->setText (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_A11Y:
		{
#if BBNDK_VERSION_AT_LEAST(10,2,0)
			AbstractA11yObject *a11y = dynamic_cast<AbstractA11yObject *> (list.at (i));

			if (a11y != NULL)
				a11y->setName (translated);
#endif
		}
		break;
		case NSR_TRANSLATOR_TYPE_SELECTHDL:
		{
			MultiSelectHandler *hdl = dynamic_cast<MultiSelectHandler *> (list.at (i));

			if (hdl != NULL)
				hdl->setStatus (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_TITLEBAR:
		{
			TitleBar *bar = dynamic_cast<TitleBar *> (list.at (i));

			if (bar != NULL)
				bar->setTitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_TAB:
		{
			Tab *tab = dynamic_cast<Tab *> (list.at (i));

			if (tab != NULL)
				tab->setTitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_HEADER:
		{
			Header *header = dynamic_cast<Header *> (list.at (i));

			if (header != NULL)
				header->setTitle (translated);
		}
		break;
		case NSR_TRANSLATOR_TYPE_DROPDOWN_TITLE:
		{
			DropDown *dropDown = dynamic_cast<DropDown *> (list.at (i));

			if (dropDown != NULL)
				dropDown->setTitle (translated);
		}
		break;
		default:
			break;
		}
	}
}
