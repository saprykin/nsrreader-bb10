#include "nsrtocpage.h"
#include "nsrglobalnotifier.h"
#include "nsrtocitemfactory.h"
#include "nsrtocitem.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRTocPage::NSRTocPage (bb::cascades::NavigationPane *naviPane, QObject *parent) :
	Page (parent),
	_translator (NULL),
	_toc (NULL),
	_naviPane (naviPane),
	_listView (NULL),
	_model (NULL),
	_emptyContainer (NULL),
	_noTocLabel (NULL),
	_noFileLabel (NULL)
{
	_translator = new NSRTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	_listView = new ListView ();
	_listView->setVerticalAlignment (VerticalAlignment::Fill);
	_listView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_listView->setListItemProvider (new NSRTocItemFactory ());

	bool ok = connect (_listView, SIGNAL (triggered (QVariantList)), this, SLOT (onListItemTriggered (QVariantList)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	Label *emptyLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("No contents", "List of contents related to opened file"));
	emptyLabel->textStyle()->setFontSize (FontSize::Large);
	emptyLabel->textStyle()->setTextAlign (TextAlign::Center);

	_noTocLabel = Label::create().horizontal(HorizontalAlignment::Center)
				     .vertical(VerticalAlignment::Center)
				     .multiline(true)
				     .text(trUtf8 ("No contents to display"));
	_noTocLabel->textStyle()->setFontSize (FontSize::Medium);
	_noTocLabel->textStyle()->setTextAlign (TextAlign::Center);

	_noFileLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("Open file to display contents"));
	_noFileLabel->textStyle()->setFontSize (FontSize::Medium);
	_noFileLabel->textStyle()->setTextAlign (TextAlign::Center);

	ImageView *emptyImage = ImageView::create().horizontal(HorizontalAlignment::Center)
						   .vertical(VerticalAlignment::Center)
						   .imageSource(QUrl ("asset:///contents-area.png"));

#if BBNDK_VERSION_AT_LEAST(10,2,0)
	emptyImage->accessibility()->setName (trUtf8 ("Image of contents"));
#endif

	_emptyContainer = Container::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Center)
					     .layout(StackLayout::create ());

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_emptyContainer->setLeftPadding (ui()->sddu (2));
	_emptyContainer->setRightPadding (ui()->sddu (2));
#elif BBNDK_VERSION_AT_LEAST(10,3,0)
	_emptyContainer->setLeftPadding (ui()->sdu (2));
	_emptyContainer->setRightPadding (ui()->sdu (2));
#else
	_emptyContainer->setLeftPadding (20);
	_emptyContainer->setRightPadding (20);
#endif

	_emptyContainer->add (emptyImage);
	_emptyContainer->add (emptyLabel);
	_emptyContainer->add (_noTocLabel);
	_emptyContainer->add (_noFileLabel);

	rootContainer->add (_listView);
	rootContainer->add (_emptyContainer);
	rootContainer->setBackground (NSRThemeSupport::instance()->getBackground ());

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Contents", "Title for window with contents")));

	_translator->addTranslatable ((UIObject *) emptyLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRTocPage"),
				      QString ("No contents"));
	_translator->addTranslatable ((UIObject *) _noTocLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRTocPage"),
				      QString ("No contents to display"));
	_translator->addTranslatable ((UIObject *) _noFileLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRTocPage"),
				      QString ("Open file to display contents"));
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRTocPage"),
				      QString ("Contents"));

#if BBNDK_VERSION_AT_LEAST(10,2,0)
	_translator->addTranslatable ((UIObject *) emptyImage->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRTocPage"),
				      QString ("Image of contents"));
#endif

#if BBNDK_VERSION_AT_LEAST(10,1,0)
	_listView->setScrollRole (ScrollRole::Main);
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      _translator, SLOT (translate ()));
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	ok = connect (ui (), SIGNAL (dduFactorChanged (float)),
		      this, SLOT (onDynamicDUFactorChanged (float)));
	Q_ASSERT (ok);
#endif

	_model = new GroupDataModel ();
	_model->setGrouping (ItemGrouping::None);
	_model->setSortingKeys (QStringList ("page-number"));

	_listView->setDataModel (_model);

	/* Show empty label and image */
	unloadData ();
}

NSRTocPage::~NSRTocPage ()
{
	_model->clear ();

	if (_toc != NULL) {
		delete _toc;
		_toc = NULL;
	}
}

void
NSRTocPage::setToc (NSRTocEntry *toc)
{
	_model->clear ();

	if (_toc != NULL) {
		delete _toc;
		_toc = NULL;
	}

	_toc = toc;

	if (_toc == NULL) {
		updateUi ();
		return;
	}

	QList<const NSRTocEntry *> items = _toc->getChildren ();

	foreach (const NSRTocEntry *item, items) {
		QVariantMap map;

		map["page-number"] = item->getPage ();
		map["entry"] = qVariantFromValue ((void *) item);

		_model->insert (map);
	}

	updateUi ();
}

void
NSRTocPage::onDocumentOpened (const QString& file)
{
	Q_UNUSED (file);
}

void
NSRTocPage::onDocumentClosed ()
{
	unloadData ();
}

void
NSRTocPage::onSubtreeRequested (const NSRTocEntry *toc)
{
	if (toc == NULL)
		return;

	NSRTocEntry *clonedToc = toc->clone ();

	if (clonedToc != NULL && _naviPane != NULL) {
		NSRTocPage *tocPage = new NSRTocPage (_naviPane);
		tocPage->setToc (clonedToc);

		bool ok = connect (tocPage, SIGNAL (subtreeRequested (const NSRTocEntry *)),
				   this, SLOT (onSubtreeRequested (const NSRTocEntry *)));
		Q_UNUSED (ok);
		Q_ASSERT (ok);

		ok = connect (tocPage, SIGNAL (pageRequested (int)), this, SIGNAL (pageRequested (int)));
		Q_ASSERT (ok);

		_naviPane->push (tocPage);
	}
}

void
NSRTocPage::onPopTransitionEnded (bb::cascades::Page *page)
{
	if (page != NULL)
		delete page;
}

void
NSRTocPage::unloadData ()
{
	_model->clear ();

	if (_toc != NULL) {
		delete _toc;
		_toc = NULL;
	}

	_listView->setVisible (false);
	_emptyContainer->setVisible (true);
	_noFileLabel->setVisible (true);
	_noTocLabel->setVisible (false);
}

void
NSRTocPage::updateUi ()
{
	_listView->setVisible (_model->size () > 0);
	_emptyContainer->setVisible (_model->size () == 0);
	_noFileLabel->setVisible (false);
	_noTocLabel->setVisible (_model->size () == 0);
}

void
NSRTocPage::onListItemTriggered (QVariantList indexPath)
{
	Q_UNUSED (indexPath);
	QVariantMap map = _listView->dataModel()->data(indexPath).toMap ();

	const NSRTocEntry *entry = (const NSRTocEntry *) (map["entry"].value<void *> ());

	if (entry == NULL)
		return;

	if (!_listView->property(NSR_TOC_ITEM_INNER_TAP_PROP).toBool ()) {
		int page = map["page-number"].toInt ();

		if (!entry->isExternal () && page > 0)
			emit pageRequested (page);
	} else
		emit subtreeRequested (entry);
}

void
NSRTocPage::onDynamicDUFactorChanged (float dduFactor)
{
	Q_UNUSED (dduFactor);

#if BBNDK_VERSION_AT_LEAST(10,3,1)
	_emptyContainer->setLeftPadding (ui()->sddu (2));
	_emptyContainer->setRightPadding (ui()->sddu (2));
#endif
}
