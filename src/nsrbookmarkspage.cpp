#include "nsrbookmarkspage.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"

#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRBookmarksPage::NSRBookmarksPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
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
				      QString ("Image of bookmarks"));
#  endif
#endif

	bool ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
			   _translator, SLOT (translate ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	_listView->setVisible (false);
	_emptyContainer->setVisible (true);
}

NSRBookmarksPage::~NSRBookmarksPage ()
{
}

