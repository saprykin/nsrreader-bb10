#include "nsrbookmarkspage.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"
#include "nsrbookmarkitemfactory.h"
#include "nsrbookmarksstorage.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/TitleBar>

#include <QtXml/QDomDocument>

using namespace bb::cascades;

NSRBookmarksPage::NSRBookmarksPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
	_model (NULL),
	_listView (NULL),
	_emptyContainer (NULL),
	_noBookmarksLabel (NULL),
	_noFileLabel (NULL)
{
	_translator = new NSRTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	_listView = new ListView ();
	_listView->setVerticalAlignment (VerticalAlignment::Fill);
	_listView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_listView->setListItemProvider (new NSRBookmarkItemFactory ());

	Label *emptyLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("No bookmarks", "List bookmarks related to opened file"));
	emptyLabel->textStyle()->setFontSize (FontSize::Large);
	emptyLabel->textStyle()->setTextAlign (TextAlign::Center);

	_noBookmarksLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("Add bookmarks to display them here"))
					   .visible(false);
	_noBookmarksLabel->textStyle()->setFontSize (FontSize::Medium);
	_noBookmarksLabel->textStyle()->setTextAlign (TextAlign::Center);

	_noFileLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("Open file to display bookmarks"));
	_noFileLabel->textStyle()->setFontSize (FontSize::Medium);
	_noFileLabel->textStyle()->setTextAlign (TextAlign::Center);

	ImageView *emptyImage = ImageView::create().horizontal(HorizontalAlignment::Center)
						   .vertical(VerticalAlignment::Center)
						   .imageSource(QUrl ("asset:///bookmarks-area.png"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	emptyImage->accessibility()->setName (trUtf8 ("Image of bookmark"));
#  endif
#endif

	_emptyContainer = Container::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Center)
					     .layout(StackLayout::create ())
					     .visible(false);
	_emptyContainer->setLeftPadding (20);
	_emptyContainer->setRightPadding (20);
	_emptyContainer->add (emptyImage);
	_emptyContainer->add (emptyLabel);
	_emptyContainer->add (_noBookmarksLabel);
	_emptyContainer->add (_noFileLabel);

	rootContainer->add (_listView);
	rootContainer->add (_emptyContainer);
	rootContainer->setBackground (Color::Black);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Bookmarks", "Title for window with bookmarks")));

	_translator->addTranslatable ((UIObject *) emptyLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRBookmarksPage"),
				      QString ("No bookmarks"));
	_translator->addTranslatable ((UIObject *) _noBookmarksLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRBookmarksPage"),
				      QString ("Add bookmarks to display them here"));
	_translator->addTranslatable ((UIObject *) _noFileLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRBookmarksPage"),
				      QString ("Open file to display bookmarks"));
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRBookmarksPage"),
				      QString ("Bookmarks"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	_translator->addTranslatable ((UIObject *) emptyImage->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRBookmarksPage"),
				      QString ("Image of bookmark"));
#  endif
#endif

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			   this, SLOT (retranslateUi ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	_model = new GroupDataModel ();
	_model->setGrouping (ItemGrouping::None);
	_model->setSortingKeys (QStringList ("page-number"));

	_listView->setDataModel (_model);

	unloadData ();
}

NSRBookmarksPage::~NSRBookmarksPage ()
{
	_model->clear ();
}

bool
NSRBookmarksPage::hasBookmark (int page)
{
	QVariantList query;
	query.append (QVariant (page));

	QVariantList result = _model->find (query);

	return !result.isEmpty ();
}

void
NSRBookmarksPage::onDocumentOpened (const QString& file)
{
	loadData (file);
	_openedFile = file;
}

void
NSRBookmarksPage::onDocumentClosed ()
{
	unloadData ();
	_openedFile = QString ();
}

void
NSRBookmarksPage::onDocumentToBeDeleted (const QString& file)
{
	if (_openedFile == file)
		unloadData ();

	NSRBookmarksStorage::instance()->removeBookmarks (file);
}

void
NSRBookmarksPage::loadData (const QString& file)
{
	if (!QFile::exists (file))
		return;

	_model->clear ();

	QDomDocument doc ("NSRBookmarksDocument");

	if (!doc.setContent (NSRBookmarksStorage::instance()->getBookmarks (file))) {
		_listView->setVisible (false);
		_emptyContainer->setVisible (true);
		_noFileLabel->setVisible (false);
		_noBookmarksLabel->setVisible (true);

		return;
	}

	QDomElement docElem = doc.documentElement ();
	QDomNode n = docElem.firstChild ();

	while (!n.isNull()) {
		QDomElement e = n.toElement ();

		if (!e.isNull () && e.tagName () == "bookmark") {
			QString title = e.attribute ("title");
			QString page = e.attribute("page");

			if (!title.isEmpty () && !page.isEmpty ()) {
				QVariantMap map;

				map["title"] = title;
				map["description"] = trUtf8("Page %1").arg (page);
				map["page-number"] = page.toInt ();

				_model->insert (map);
			}
		}

		n = n.nextSibling ();
	}

	_listView->setVisible (_model->size () > 0);
	_emptyContainer->setVisible (_model->size () == 0);
	_noFileLabel->setVisible (false);
	_noBookmarksLabel->setVisible (_model->size () == 0);
}

void
NSRBookmarksPage::addBookmark (const QString& title, int page)
{
	QVariantList query, result;
	QVariantMap val;

	query.append (QVariant (page));
	result = _model->find (query);

	if (!result.isEmpty ()) {
		val = _model->data(result).toMap ();
		val["title"] = title;
		_model->updateItem (query, val);
	} else {
		val["title"] = title;
		val["description"] = trUtf8("Page %1").arg (page);
		val["page-number"] = page;
		_model->insert (val);
	}

	saveData ();

	_listView->setVisible (_model->size () > 0);
	_emptyContainer->setVisible (_model->size () == 0);
	_noFileLabel->setVisible (false);
	_noBookmarksLabel->setVisible (_model->size () == 0);
}

void
NSRBookmarksPage::retranslateUi ()
{
	_translator->translate ();

	for (int i = 0; i < _model->size (); ++i) {
		QVariantList query;
		query.append (QVariant (i));
		QVariantMap val = _model->data(query).toMap ();

		val["description"] = trUtf8("Page %1").arg (val["page-number"].toInt ());

		_model->updateItem (query, val);
	}
}

void
NSRBookmarksPage::saveData ()
{
	if (_openedFile.isEmpty ())
		return;

	QDomDocument doc ("NSRBookmarksDocument");
	QDomElement root = doc.createElement ("NSRBookmarksDocument");
	doc.appendChild (root);

	for (int i = 0; i < _model->size (); ++i) {
		QVariantList query;
		query.append (QVariant (i));
		QVariantMap val = _model->data(query).toMap ();

		QDomElement tag = doc.createElement ("bookmark");
		tag.setAttribute ("title", val["title"].toString ());
		tag.setAttribute ("page", val["page-number"].toString ());
		root.appendChild (tag);
	}

	NSRBookmarksStorage::instance()->saveBookmarks (_openedFile, doc.toString ());
}

void NSRBookmarksPage::unloadData ()
{
	_model->clear ();
	_listView->setVisible (false);
	_emptyContainer->setVisible (true);
	_noFileLabel->setVisible (true);
	_noBookmarksLabel->setVisible (false);
}
