#include "nsrreaderbb10.h"
#include "nsrsettings.h"
#include "nsrsession.h"
#include "nsrpreferencespage.h"
#include "nsrlastdocspage.h"
#include "nsrfilesharer.h"
#include "nsrscenecover.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"

#include <float.h>

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Page>
#include <bb/cascades/Color>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Label>
#include <bb/cascades/Menu>
#include <bb/cascades/ActionItem>
#include <bb/cascades/NavigationPaneProperties>
#include <bb/cascades/TabbedPane>
#include <bb/cascades/Tab>
#include <bb/cascades/Window>
#include <bb/cascades/ScreenIdleMode>

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,1,0)
#    include <bb/cascades/SystemShortcut>
#    include <bb/cascades/Shortcut>
#  endif
#endif

#include <bb/system/LocaleHandler>

#include <bb/multimedia/MediaKey>

#include <bb/device/DisplayInfo>

using namespace bb::system;
using namespace bb::cascades;
using namespace bb::cascades::pickers;
using namespace bb::device;
using namespace bb::multimedia;

#define NSR_GUI_VERSION				"1.4.0"

#define NSR_GUI_ACTION_BAR_NORMAL_HEIGHT	140
#define NSR_GUI_ACTION_BAR_REDUCED_HEIGHT	100
#define NSR_GUI_MAIN_TAB_INDEX			0
#define NSR_GUI_RECENT_TAB_INDEX		1
#define NSR_GUI_BOOKMARKS_TAB_INDEX		2
#define NSR_GUI_CROP_TO_WIDTH_THRESHOLD		4

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app),
	_core (NULL),
	_pageView (NULL),
	_pageStatus (NULL),
	_welcomeView (NULL),
	_actionAggregator (NULL),
	_slider (NULL),
	_bpsHandler (NULL),
	_translator (NULL),
	_qtranslator (NULL),
	_naviPane (NULL),
	_page (NULL),
	_filePicker (NULL),
	_indicator (NULL),
	_prompt (NULL),
	_toast (NULL),
	_dialog (NULL),
	_invokeManager (NULL),
	_startMode (ApplicationStartupMode::LaunchApplication),
	_isFullscreen (false),
	_isActiveFrame (false),
	_isWaitingForFirstPage (false),
	_wasSliderVisible (false)
{
	memset (_mediaKeys, 0, sizeof (_mediaKeys));

	_invokeManager = new InvokeManager (this);

	bool ok = connect (_invokeManager, SIGNAL (invoked (const bb::system::InvokeRequest&)),
			   this, SLOT (onInvoke (const bb::system::InvokeRequest&)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_invokeManager, SIGNAL (cardPooled (const bb::system::CardDoneMessage&)),
		      this, SLOT (onCardPooled (const bb::system::CardDoneMessage&)));
	Q_ASSERT (ok);

	ok = connect (_invokeManager, SIGNAL (cardResizeRequested (const bb::system::CardResizeMessage&)),
		      this, SLOT (onCardResize (const bb::system::CardResizeMessage&)));
	Q_ASSERT (ok);

	_startMode = _invokeManager->startupMode ();

	initFullUI ();
}

NSRReaderBB10::~NSRReaderBB10 ()
{
}

QString
NSRReaderBB10::getVersion ()
{
	return QString (NSR_GUI_VERSION);
}

void
NSRReaderBB10::initFullUI ()
{
	NSRSettings::instance()->setStarting (true);

	_translator = new NSRTranslator (this);
	_qtranslator = new QTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .background(Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f))
						      .layout(DockLayout::create());

	_toast = new SystemToast (this);

	_pageView = new NSRPageView ();
	_pageView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_pageView->setVerticalAlignment (VerticalAlignment::Fill);
	_pageView->setVisible (false);
	_indicator = ActivityIndicator::create().horizontal(HorizontalAlignment::Fill)
						.vertical(VerticalAlignment::Fill)
						.visible(false);
	_indicator->setTouchPropagationMode (TouchPropagationMode::None);

	bool ok = connect (_indicator, SIGNAL (stopped ()), this, SLOT (onIndicatorStopped ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_pageView, SIGNAL (prevPageRequested ()), this, SLOT (onPrevPageRequested ()));
	Q_ASSERT (ok);

	ok = connect (_pageView, SIGNAL (nextPageRequested ()), this, SLOT (onNextPageRequested ()));
	Q_ASSERT (ok);

	ok = connect (_pageView, SIGNAL (fitToWidthRequested ()), this, SLOT (onFitToWidthRequested ()));
	Q_ASSERT (ok);

	ok = connect (_pageView, SIGNAL (rotateLeftRequested ()), this, SLOT (onRotateLeftRequested ()));
	Q_ASSERT (ok);

	ok = connect (_pageView, SIGNAL (rotateRightRequested ()), this, SLOT (onRotateRightRequested ()));
	Q_ASSERT (ok);

	_pageStatus = new NSRPageStatus ();
	_pageStatus->setAutoHide (false);
	_pageStatus->setHorizontalAlignment (HorizontalAlignment::Left);
	_pageStatus->setVerticalAlignment (VerticalAlignment::Top);
	_pageStatus->setStatusBackground (Color::Gray);
	_pageStatus->setStatusBackgroundOpacity (0.4f);
	_pageStatus->setStatus (0, 0);

	ok = connect (_pageView, SIGNAL (viewTapped ()), this, SLOT (onPageTapped ()));
	Q_ASSERT (ok);

	_welcomeView = new NSRWelcomeView ();
	/* We need welcome view only if not loading file at start up */
	_welcomeView->setCardMode (true);

	ok = connect (_welcomeView, SIGNAL (openDocumentRequested ()), this, SLOT (onOpenActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (_welcomeView, SIGNAL (recentDocumentsRequested ()), this, SLOT (onRecentDocumentsRequested ()));
	Q_ASSERT (ok);

	_slider = new NSRPageSlider ();
	_slider->setBottomSpace (getActionBarHeight ());
	_slider->setVisible (false);

	ok = connect (_slider, SIGNAL (interactionStarted ()), this, SLOT (onPageSliderInteractionStarted ()));
	Q_ASSERT (ok);

	ok = connect (_slider, SIGNAL (interactionEnded ()), this, SLOT (onPageSliderInteractionEnded ()));
	Q_ASSERT (ok);

	ok = connect (_slider, SIGNAL (currentValueChanged (int)), this, SLOT (onPageSliderValueChanged (int)));
	Q_ASSERT (ok);

	rootContainer->add (_pageView);
	rootContainer->add (_welcomeView);
	rootContainer->add (_indicator);
	rootContainer->add (_pageStatus);
	rootContainer->add (_slider);

	_page = Page::create().content (rootContainer);
	_page->setActionBarVisibility (ChromeVisibility::Visible);
	_actionAggregator = new NSRActionAggregator (this);

	_bpsHandler = new NSRBpsEventHandler (this);
	ok = connect (_bpsHandler, SIGNAL (vkbVisibilityChanged (bool)),
		      this, SLOT (onVkbVisibilityChanged (bool)));

	ActionItem *openAction = ActionItem::create().enabled (true);
	openAction->setTitle (trUtf8 ("Open", "Open file"));
	ActionItem *prevPageAction = ActionItem::create().enabled (false);
	prevPageAction->setTitle (trUtf8 ("Previous", "Previous page"));
	ActionItem *nextPageAction = ActionItem::create().enabled (false);
	nextPageAction->setTitle (trUtf8 ("Next", "Next page"));
	ActionItem *gotoAction = ActionItem::create().enabled (false);
	gotoAction->setTitle (trUtf8 ("Go to", "Go to page"));
	ActionItem *reflowAction = ActionItem::create().enabled (false);
	reflowAction->setTitle (trUtf8 ("Text Reflow", "Text mode for a file view"));
	ActionItem *invertAction = ActionItem::create();
	invertAction->setTitle (trUtf8 ("Invert Colors"));
	ActionItem *prefsAction = ActionItem::create().title (trUtf8 ("Settings"));
	ActionItem *helpAction = ActionItem::create().title (trUtf8 ("About", "About a program, window title"));
	ActionItem *shareAction = ActionItem::create().enabled (false);
	shareAction->setTitle (trUtf8 ("Share", "Share file between users"));
	ActionItem *bookmarkAction = ActionItem::create().enabled (false);
#ifdef NSR_CORE_LITE_VERSION
	ActionItem *buyAction = ActionItem::create();
	buyAction->setTitle (trUtf8 ("Buy", "Buy full version of the app in the store"));
#endif

	_translator->addTranslatable ((UIObject *) openAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Open"));
	_translator->addTranslatable ((UIObject *) prevPageAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Previous"));
	_translator->addTranslatable ((UIObject *) nextPageAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Next"));
	_translator->addTranslatable ((UIObject *) gotoAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Go to"));
	_translator->addTranslatable ((UIObject *) reflowAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Text Reflow"));
	_translator->addTranslatable ((UIObject *) invertAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Invert Colors"));
	_translator->addTranslatable ((UIObject *) prefsAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Settings"));
	_translator->addTranslatable ((UIObject *) helpAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("About"));
	_translator->addTranslatable ((UIObject *) shareAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Share"));
#ifdef NSR_CORE_LITE_VERSION
	_translator->addTranslatable ((UIObject *) buyAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Buy"));
#endif

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	openAction->accessibility()->setName (trUtf8 ("Open file"));
	prevPageAction->accessibility()->setName (trUtf8 ("Go to previous page"));
	nextPageAction->accessibility()->setName (trUtf8 ("Go to next page"));
	gotoAction->accessibility()->setName (trUtf8 ("Go to arbitrary page"));
	reflowAction->accessibility()->setName (trUtf8 ("Switch text reflow mode"));
	invertAction->accessibility()->setName (trUtf8 ("Invert page colors"));
	prefsAction->accessibility()->setName (trUtf8 ("Open Settings page"));
	helpAction->accessibility()->setName (trUtf8 ("Open page with information about the app and help sections"));
	shareAction->accessibility()->setName (trUtf8 ("Share file with others"));

	_translator->addTranslatable ((UIObject *) openAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open file"));
	_translator->addTranslatable ((UIObject *) prevPageAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Go to previous page"));
	_translator->addTranslatable ((UIObject *) nextPageAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Go to next page"));
	_translator->addTranslatable ((UIObject *) gotoAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Go to arbitrary page"));
	_translator->addTranslatable ((UIObject *) reflowAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Switch text reflow mode"));
	_translator->addTranslatable ((UIObject *) invertAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Invert page colors"));
	_translator->addTranslatable ((UIObject *) prefsAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open Settings page"));
	_translator->addTranslatable ((UIObject *) helpAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open page with information about the app and help sections"));
	_translator->addTranslatable ((UIObject *) shareAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Share file with others"));
#    ifdef NSR_CORE_LITE_VERSION
	_translator->addTranslatable ((UIObject *) buyAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Buy full version of the app in the store"));
#    endif

#  endif
#endif

	openAction->setImageSource (QUrl ("asset:///open.png"));
	prevPageAction->setImageSource (QUrl ("asset:///previous.png"));
	nextPageAction->setImageSource (QUrl ("asset:///next.png"));
	gotoAction->setImageSource (QUrl ("asset:///goto.png"));
	reflowAction->setImageSource (QUrl ("asset:///text-mode.png"));
	invertAction->setImageSource (QUrl ("asset:///invert.png"));
	prefsAction->setImageSource (QUrl ("asset:///settings.png"));
	helpAction->setImageSource (QUrl ("asset:///about.png"));
	shareAction->setImageSource (QUrl ("asset:///share.png"));
#ifdef NSR_CORE_LITE_VERSION
	buyAction->setImage (QUrl ("asset:///buy.png"));
#endif

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		_page->addAction (shareAction, ActionBarPlacement::OnBar);
		_page->addAction (prevPageAction, ActionBarPlacement::OnBar);
		_page->addAction (nextPageAction, ActionBarPlacement::OnBar);
		_page->addAction (gotoAction, ActionBarPlacement::InOverflow);
		_page->addAction (reflowAction, ActionBarPlacement::InOverflow);
		_page->addAction (invertAction, ActionBarPlacement::InOverflow);

		openAction->setParent (_page);
		bookmarkAction->setParent (_page);
	} else {
		_page->addAction (openAction, ActionBarPlacement::OnBar);
		_page->addAction (prevPageAction, ActionBarPlacement::OnBar);
		_page->addAction (nextPageAction, ActionBarPlacement::OnBar);
		_page->addAction (gotoAction, ActionBarPlacement::InOverflow);
		_page->addAction (bookmarkAction, ActionBarPlacement::InOverflow);
		_page->addAction (reflowAction, ActionBarPlacement::InOverflow);
		_page->addAction (invertAction, ActionBarPlacement::InOverflow);
		_page->addAction (shareAction, ActionBarPlacement::InOverflow);
	}

	_actionAggregator->addAction ("open", openAction);
	_actionAggregator->addAction ("prev", prevPageAction);
	_actionAggregator->addAction ("next", nextPageAction);
	_actionAggregator->addAction ("goto", gotoAction);
	_actionAggregator->addAction ("bookmark", bookmarkAction);
	_actionAggregator->addAction ("reflow", reflowAction);
	_actionAggregator->addAction ("invert", invertAction);
	_actionAggregator->addAction ("share", shareAction);
	_actionAggregator->addAction ("prefs", prefsAction);
	_actionAggregator->addAction ("help", helpAction);
#ifdef NSR_CORE_LITE_VERSION
	_actionAggregator->addAction ("buy", buyAction);
#endif

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,1,0)
	SystemShortcut *prevShortcut = SystemShortcut::create (SystemShortcuts::PreviousSection);
	SystemShortcut *nextShortcut = SystemShortcut::create (SystemShortcuts::NextSection);
	Shortcut *gotoShortcut = Shortcut::create().key ("Ctrl + G");
	Shortcut *reflowShortcut = Shortcut::create().key ("Ctrl + T");
	Shortcut *invertShortcut = Shortcut::create().key ("Ctrl + I");

	prevPageAction->addShortcut (prevShortcut);
	nextPageAction->addShortcut (nextShortcut);
	gotoAction->addShortcut (gotoShortcut);
	reflowAction->addShortcut (reflowShortcut);
	invertAction->addShortcut (invertShortcut);

	if (_startMode != ApplicationStartupMode::InvokeCard) {
		Shortcut *openShortcut = Shortcut::create().key ("Ctrl + O");
		Shortcut *bookmarkShortcut = Shortcut::create().key ("Ctrl + B");

		openAction->addShortcut (openShortcut);
		bookmarkAction->addShortcut (bookmarkShortcut);
	}
#  endif
#endif

	ok = connect (openAction, SIGNAL (triggered ()), this, SLOT (onOpenActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (prevPageAction, SIGNAL (triggered ()), this, SLOT (onPrevPageActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (nextPageAction, SIGNAL (triggered ()), this, SLOT (onNextPageActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (gotoAction, SIGNAL (triggered ()), this, SLOT (onGotoActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (bookmarkAction, SIGNAL (triggered ()), this, SLOT (onBookmarkActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (prefsAction, SIGNAL (triggered ()), this, SLOT (onPrefsActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (helpAction, SIGNAL (triggered ()), this, SLOT (onHelpActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (shareAction, SIGNAL (triggered ()), this, SLOT (onShareActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (reflowAction, SIGNAL (triggered ()), this, SLOT (onReflowActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (invertAction, SIGNAL (triggered ()), this, SLOT (onInvertActionTriggered ()));
	Q_ASSERT (ok);

#ifdef NSR_CORE_LITE_VERSION
	ok = connect (buyAction, SIGNAL (triggered ()), this, SLOT (onBuyActionTriggered ()));
	Q_ASSERT (ok);
#endif

	Menu *menu = new Menu ();
	menu->addAction (helpAction);
	menu->addAction (prefsAction);
#ifdef NSR_CORE_LITE_VERSION
	menu->addAction (buyAction);
#endif
	Application::instance()->setMenu (menu);

	_filePicker = new FilePicker (this);
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);
	_filePicker->setFilter (QStringList ("*.pdf") << "*.djvu" << "*.djv" <<
					     "*.tiff" << "*.tif" << "*.txt");

	_core = new NSRReaderCore (_startMode == ApplicationStartupMode::InvokeCard, this);

	ok = connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		      this, SLOT (onFileSelected (const QStringList&)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (pageRendered (int)), this, SLOT (onPageRendered (int)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (needIndicator (bool)), this, SLOT (onIndicatorRequested (bool)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (errorWhileOpening (NSRAbstractDocument::NSRDocumentError)),
		      this, SLOT (onErrorWhileOpening (NSRAbstractDocument::NSRDocumentError)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (documentOpened (QString)), this, SLOT (onDocumentOpened ()));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (documentClosed (QString)), this, SLOT (onDocumentClosed ()));
	Q_ASSERT (ok);

#ifdef NSR_CORE_LITE_VERSION
	ok = connect (_core, SIGNAL (liteVersionOverPage ()), this, SLOT (onLiteVersionOverPage ()));
	Q_ASSERT (ok);
#endif

	_naviPane = NavigationPane::create().add (_page);

	ok = connect (_naviPane, SIGNAL (popTransitionEnded (bb::cascades::Page *)),
		      this, SLOT (onPopTransitionEnded (bb::cascades::Page *)));
	Q_ASSERT (ok);

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		Application::instance()->setScene (_naviPane);
	} else {
		NSRLastDocsPage *recentPage = new NSRLastDocsPage ();
		NSRBookmarksPage *bookmarksPage = new NSRBookmarksPage ();

		ok = connect (recentPage, SIGNAL (requestDocument (QString)), this, SLOT (onLastDocumentRequested (QString)));
		Q_ASSERT (ok);

		ok = connect (recentPage, SIGNAL (documentToBeDeleted (QString)), this, SLOT (onDocumentToBeDeleted (QString)));
		Q_ASSERT (ok);

		ok = connect (recentPage, SIGNAL (documentToBeDeleted (QString)), bookmarksPage, SLOT (onDocumentToBeDeleted (QString)));
		Q_ASSERT (ok);

		Tab *mainTab = Tab::create().content(_naviPane).title(trUtf8 ("Reading")).imageSource(QUrl ("asset:///main-tab.png"));
		Tab *recentTab = Tab::create().content(recentPage).title(trUtf8 ("Recent")).imageSource(QUrl ("asset:///recent.png"));
		Tab *bookmarksTab = Tab::create().content(bookmarksPage).title(trUtf8 ("Bookmarks")).imageSource(QUrl ("asset:///bookmarks.png"));

		TabbedPane *tabbedPane = TabbedPane::create().add(mainTab).add(recentTab).add(bookmarksTab);
		tabbedPane->setPeekEnabled (false);

		ok = connect (_core, SIGNAL (documentOpened (QString)), recentPage, SLOT (onDocumentOpened (QString)));
		Q_ASSERT (ok);

		ok = connect (_core, SIGNAL (thumbnailRendered ()), recentPage, SLOT (onThumbnailRendered ()));
		Q_ASSERT (ok);

		ok = connect (_core, SIGNAL (documentOpened (QString)), bookmarksPage, SLOT (onDocumentOpened (QString)));
		Q_ASSERT (ok);

		ok = connect (_core, SIGNAL (documentClosed (QString)), bookmarksPage, SLOT (onDocumentClosed ()));
		Q_ASSERT (ok);

		ok = connect (bookmarksPage, SIGNAL (bookmarkChanged (int, bool)), this, SLOT (onBookmarkChanged (int, bool)));
		Q_ASSERT (ok);

		ok = connect (bookmarksPage, SIGNAL (pageRequested (int)), this, SLOT (onBookmarkPageRequested (int)));
		Q_ASSERT (ok);

		_translator->addTranslatable ((UIObject *) mainTab,
				NSRTranslator::NSR_TRANSLATOR_TYPE_TAB,
				QString ("NSRReaderBB10"),
				QString ("Reading"));
		_translator->addTranslatable ((UIObject *) recentTab,
				NSRTranslator::NSR_TRANSLATOR_TYPE_TAB,
				QString ("NSRReaderBB10"),
				QString ("Recent"));
		_translator->addTranslatable ((UIObject *) bookmarksTab,
				NSRTranslator::NSR_TRANSLATOR_TYPE_TAB,
				QString ("NSRReaderBB10"),
				QString ("Bookmarks"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
		mainTab->accessibility()->setName (trUtf8 ("Main file reading page"));
		recentTab->accessibility()->setName (trUtf8 ("Page with recent files"));
		bookmarksTab->accessibility()->setName (trUtf8 ("Page with bookmarks"));

		_translator->addTranslatable ((UIObject *) mainTab->accessibility (),
				NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				QString ("NSRReaderBB10"),
				QString ("Main file reading page"));
		_translator->addTranslatable ((UIObject *) recentTab->accessibility (),
				NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				QString ("NSRReaderBB10"),
				QString ("Page with recent files"));
		_translator->addTranslatable ((UIObject *) bookmarksTab->accessibility (),
				NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				QString ("NSRReaderBB10"),
				QString ("Page with bookmarks"));
#  endif
#endif

		Application::instance()->setScene (tabbedPane);
	}

	_mediaKeys[0] = new MediaKeyWatcher (MediaKey::VolumeUp, this);
	_mediaKeys[1] = new MediaKeyWatcher (MediaKey::VolumeDown, this);

	setVolumeKeysEnabled (true);

	bb::cascades::LocaleHandler *localeHandler = new bb::cascades::LocaleHandler (this);
	ok = connect (localeHandler, SIGNAL (systemLanguageChanged ()),
		      this, SLOT (onSystemLanguageChanged ()));
	Q_ASSERT (ok);

	ok = connect (OrientationSupport::instance (),
		      SIGNAL (orientationAboutToChange (bb::cascades::UIOrientation::Type)),
		      this,
		      SLOT (onOrientationAboutToChange (bb::cascades::UIOrientation::Type)));
	Q_ASSERT (ok);

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		_pageView->setViewMode (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
		_pageView->setInvertedColors (false);
		onFullscreenSwitchRequested (true);
	} else {
		/* We do need it here to not to read settings in card mode */
		_pageView->setInvertedColors (NSRSettings::instance()->isInvertedColors ());
		_pageView->setViewMode (NSRSettings::instance()->isWordWrap () ? NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT
									       : NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
		onFullscreenSwitchRequested (NSRSettings::instance()->isFullscreenMode ());
	}

	ok = connect (_pageView, SIGNAL (zoomChanged (double, NSRRenderRequest::NSRRenderReason)),
		      this, SLOT (onZoomChanged (double, NSRRenderRequest::NSRRenderReason)));
	Q_ASSERT (ok);

	Application::instance()->setCover (new NSRSceneCover ());
	Application::instance()->setAutoExit (false);

	ok = connect (Application::instance (), SIGNAL (thumbnail ()), this, SLOT (onThumbnail ()));
	Q_ASSERT (ok);

	ok = connect (Application::instance (), SIGNAL (fullscreen ()), this, SLOT (onFullscreen ()));
	Q_ASSERT (ok);

	ok = connect (Application::instance (), SIGNAL (manualExit ()), this, SLOT (onManualExit ()));
	Q_ASSERT (ok);

	onSystemLanguageChanged ();

	/* Initial loading logic:
	 *   - if we are launching from invoke request, wait for further request;
	 *   - in case of first start load quick start guide;
	 *   - in other cases try to load previous session. */

	if (_startMode == ApplicationStartupMode::InvokeApplication ||
	    _startMode == ApplicationStartupMode::InvokeCard) {
		updateVisualControls ();
		NSRSettings::instance()->setStarting (false);
		return;
	}

	if (NSRSettings::instance()->isFirstStart ()) {
		NSRSettings::instance()->saveFirstStart ();
		NSRSettings::instance()->saveNewsShown (getVersion ());
		showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_HELP);

		_welcomeView->setCardMode (false);

		updateVisualControls ();
	} else {
		if (!NSRSettings::instance()->isNewsShown (getVersion ())) {
			NSRSettings::instance()->saveNewsShown (getVersion ());
			showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_CHANGES);
		}

		/* Load previously saved session */
		if (QFile::exists (NSRSettings::instance()->getLastSession().getFile ()))
			loadSession ();
		else {
			_welcomeView->setCardMode (false);
			updateVisualControls ();
		}
	}

	NSRSettings::instance()->setStarting (false);
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	if (_core->getDocumentPath () == files.first ()) {
		_toast->setBody (trUtf8 ("Selected file is already opened"));
		_toast->resetButton ();
		_toast->show ();
		return;
	}

	QFileInfo finfo (files.first ());

	/* Save session for opened document */
	NSRSettings::instance()->saveLastOpenDir (finfo.canonicalPath ());

	saveSession ();
	loadSession (finfo.canonicalFilePath ());
}

void
NSRReaderBB10::onOpenActionTriggered ()
{
	_filePicker->setDirectories (QStringList (NSRSettings::instance()->getLastOpenDir ()));
	_filePicker->open ();
}

void
NSRReaderBB10::onPrevPageActionTriggered ()
{
	_core->saveCurrentPagePositions (_pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC),
					 _pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_PREV);
}

void
NSRReaderBB10::onNextPageActionTriggered ()
{
	_core->saveCurrentPagePositions (_pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC),
					 _pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_NEXT);
}

void
NSRReaderBB10::onGotoActionTriggered ()
{
	_slider->setBottomSpace (getActionBarHeight ());
	_slider->setVisible (!_slider->isVisible ());

	if (_slider->isVisible ())
		_pageStatus->setVisible (true);
}

void
NSRReaderBB10::onReflowActionTriggered ()
{
	if (!_core->isTextReflowSwitchSupported ())
		return;

	/* Check whether we have noted user about text mode */
	if (_startMode != ApplicationStartupMode::InvokeCard && !NSRSettings::instance()->isTextModeNoted ()) {
		NSRSettings::instance()->saveTextModeNoted ();
		QString text = trUtf8 ("You are using text reflow the first time. Note that "
				       "file formatting may be differ than in original one, "
				       "no images displayed and page can be empty if there is "
				       "no text in the file. Also text may not be displayed "
				       "properly if appropriate language is not supported by phone.",
				       "Text reflow is a view mode of PDF/DjVu files when "
				       "only text information without images is displayed with "
				       "word wrap feature enabled. Use pinch gesture to adjust "
				       "text size");
		showToast (text, false);
	}

	_core->saveCurrentPagePositions (_pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC),
					 _pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
	_core->switchTextReflow ();

	if (!_core->isPageRendering () && _core->isTextReflow ())
		_pageView->setViewMode (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);

	if (_startMode == ApplicationStartupMode::InvokeCard)
		NSRSettings::instance()->saveWordWrapWithoutSync (_core->isTextReflow ());
	else
		NSRSettings::instance()->saveWordWrap (_core->isTextReflow ());
}

void
NSRReaderBB10::onInvertActionTriggered ()
{
	_pageView->setInvertedColors (!_pageView->isInvertedColors ());
	_core->invertColors ();

	if (_startMode == ApplicationStartupMode::InvokeCard)
		NSRSettings::instance()->saveInvertedColorsWithoutSync (_core->isInvertedColors ());
	else
		NSRSettings::instance()->saveInvertedColors (_core->isInvertedColors ());

}

void
NSRReaderBB10::onPrefsActionTriggered ()
{
	_actionAggregator->setActionEnabled ("prefs", false);

	NSRPreferencesPage *prefsPage = new NSRPreferencesPage ();

	bool ok = connect (prefsPage, SIGNAL (switchFullscreen (bool)),
			   this, SLOT (onFullscreenSwitchRequested (bool)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	if (_core->isDocumentOpened ()) {
		ok = connect (prefsPage, SIGNAL (switchPreventScreenLock (bool)),
				this, SLOT (onPreventScreenLockSwitchRequested (bool)));
		Q_ASSERT (ok);
	}

	_naviPane->push (prefsPage);

	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (pane != NULL) {
		pane->setActiveTab (pane->at(NSR_GUI_MAIN_TAB_INDEX));
		pane->resetSidebarState ();
	}
}

void
NSRReaderBB10::onHelpActionTriggered ()
{
	showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_MAIN);

	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (pane != NULL) {
		pane->setActiveTab (pane->at(NSR_GUI_MAIN_TAB_INDEX));
		pane->resetSidebarState ();
	}

}

void
NSRReaderBB10::onShareActionTriggered ()
{
	if (!_core->isDocumentOpened ())
		return;

	NSRFileSharer::getInstance()->shareFiles (QStringList (_core->getDocumentPath ()));
}

void
NSRReaderBB10::onBookmarkActionTriggered ()
{
	if (_prompt != NULL)
		return;

	QString bookmarkTitle;
	NSRBookmarksPage *bookmarksPage = getBookmarksPage ();

	if (bookmarksPage != NULL)
		bookmarksPage->hasBookmark (_core->getCurrentPage().getNumber (), &bookmarkTitle);

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter Bookmark"));
	_prompt->inputField()->setEmptyText (trUtf8 ("Enter bookmark title"));
	_prompt->inputField()->setDefaultText (bookmarkTitle);
	_prompt->setDismissAutomatically (false);

	bool res = connect (_prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			    this, SLOT (onAddBookmarkDialogFinished (bb::system::SystemUiResult::Type)));

	if (res)
		_prompt->show ();
	else {
		_prompt->deleteLater ();
		_prompt = NULL;
	}
}

void
NSRReaderBB10::onPageRendered (int number)
{
	int pagesCount = _core->getPagesCount ();
	NSRRenderedPage page = _core->getCurrentPage ();

	_pageView->setZoomRange (_core->getMinZoom (), _core->getMaxZoom ());
	_pageView->setPage (page);

	_pageStatus->setStatus (number, pagesCount);
	_slider->setRange (1, pagesCount);
	_slider->setValue (number);

	if (_pageView->getViewMode () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) {
		bool needToFit = _core->isFitToWidth () ?
				qAbs (_pageView->getSize().width () - page.getSize().width ()) > NSR_GUI_CROP_TO_WIDTH_THRESHOLD :
				(_pageView->getSize().width () - page.getSize().width ()) > NSR_GUI_CROP_TO_WIDTH_THRESHOLD;

		if (needToFit && page.getRenderReason () != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH &&
		    qAbs (_core->getMaxZoom () - page.getRenderedZoom ()) > DBL_EPSILON)
			_pageView->fitToWidth (page.isAutoCrop () ? NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH
								  : NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH);
	}

	updateVisualControls ();

	if (_isActiveFrame)
		onThumbnail ();

	setViewMode (_core->isTextReflow () ? NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT
					    : NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
}

void
NSRReaderBB10::updateVisualControls ()
{
	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (_startMode != ApplicationStartupMode::InvokeCard) {
		NSRRenderRequest::NSRRenderReason reason = _core->getCurrentPage().getRenderReason ();

		if (pane != NULL &&
		    reason != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH &&
		    reason != NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH &&
		    reason != NSRRenderRequest::NSR_RENDER_REASON_ZOOM) {
			pane->at(NSR_GUI_RECENT_TAB_INDEX)->setEnabled (true);
			pane->at(NSR_GUI_BOOKMARKS_TAB_INDEX)->setEnabled (true);
			pane->resetSidebarState ();
		}
	}

	bool isDocumentOpened = _core->isDocumentOpened ();
	bool hasBookmark = false;

	ActionItem *bookmarkAction = static_cast < ActionItem *> (_actionAggregator->actionByName ("bookmark"));

	_actionAggregator->setActionEnabled ("open", true);
	_actionAggregator->setActionEnabled ("prefs", true);
	_actionAggregator->setActionEnabled ("share",
				       	     isDocumentOpened && NSRFileSharer::isSharable (_core->getDocumentPath ()));
	_actionAggregator->setActionEnabled ("reflow", _core->isTextReflowSwitchSupported ());
	_actionAggregator->setActionEnabled ("invert", isDocumentOpened);
	_pageView->setVisible (isDocumentOpened);
	_welcomeView->setVisible (!isDocumentOpened);
	_slider->setEnabled (isDocumentOpened);

	if (!isDocumentOpened) {
		_actionAggregator->setActionEnabled ("prev", false);
		_actionAggregator->setActionEnabled ("next", false);
		_actionAggregator->setActionEnabled ("goto", false);
		_page->setActionBarVisibility (ChromeVisibility::Visible);
	} else {
		int totalPages = _core->getPagesCount ();
		int currentPage = _core->getCurrentPage().getNumber ();

		_actionAggregator->setActionEnabled ("prev", totalPages != 1 && currentPage > 1);
		_actionAggregator->setActionEnabled ("next", totalPages != 1 && currentPage != totalPages);
		_actionAggregator->setActionEnabled ("goto", totalPages > 1);

		if (totalPages == 1)
			_slider->setVisible (false);

		if (_isWaitingForFirstPage) {
			_page->setActionBarVisibility (ChromeVisibility::Visible);
			_pageStatus->setVisible (true);
			_isWaitingForFirstPage = false;
		}

		NSRBookmarksPage *bookmarksPage = getBookmarksPage ();

		if (bookmarksPage != NULL)
			hasBookmark = bookmarksPage->hasBookmark (currentPage);
	}

	if (bookmarkAction != NULL) {
		bookmarkAction->setEnabled (isDocumentOpened);
		retranslateBookmarkAction (hasBookmark);
	}
}

void
NSRReaderBB10::disableVisualControls ()
{
	if (_startMode != ApplicationStartupMode::InvokeCard) {
		TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

		if (pane != NULL) {
			pane->at(NSR_GUI_RECENT_TAB_INDEX)->setEnabled (false);
			pane->at(NSR_GUI_BOOKMARKS_TAB_INDEX)->setEnabled (false);
		}
	}

	_actionAggregator->setActionEnabled ("open", false);
	_actionAggregator->setActionEnabled ("prev", false);
	_actionAggregator->setActionEnabled ("next", false);
	_actionAggregator->setActionEnabled ("goto", false);
	_actionAggregator->setActionEnabled ("bookmark", false);
	_actionAggregator->setActionEnabled ("reflow", false);
	_actionAggregator->setActionEnabled ("invert", false);
	_actionAggregator->setActionEnabled ("share", false);
	_actionAggregator->setActionEnabled ("prefs", false);
	_slider->setEnabled (false);
}

void
NSRReaderBB10::loadSession (const QString& path, int page)
{
	NSRSession	session;
	int		width = _pageView->getSize().width ();

	if (!path.isEmpty () && !QFile::exists (path)) {
		if (_dialog != NULL)
			return;

		_dialog = new SystemDialog (trUtf8 ("Change"), trUtf8 ("Cancel"), this);
		_dialog->setTitle (trUtf8 ("Permission required"));
		_dialog->setBody (trUtf8 ("It seems that NSR Reader doesn't have Shared Files "
					  "permission required for proper working. Do you want "
					  "to change the permission for shared files now? You "
					  "have to restart the app after changing permissions."));

		bool res = connect (_dialog, SIGNAL (finished (bb::system::SystemUiResult::Type)),
				    this, SLOT (onPermissionDialogFinished (bb::system::SystemUiResult::Type)));

		if (res)
			_dialog->show ();
		else {
			_dialog->deleteLater ();
			_dialog = NULL;
		}

		return;
	}

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		session.setFile (path);
		session.setPage (1);
	} else {
		if (path.isEmpty ())
			session = NSRSettings::instance()->getLastSession ();
		else
			session = NSRSettings::instance()->getSessionForFile (path);
	}

	if (page != -1)
		session.setPage (page);

	/* Since we can load session when UI is not built properly
	 * we must use device parameters */
	if (width <= 0) {
		QSize displaySize = DisplayInfo().pixelSize ();

		if (OrientationSupport::instance()->orientation () == UIOrientation::Portrait)
			width = displaySize.width ();
		else
			width = displaySize.height ();
	}

	session.setZoomScreenWidth (width);

	if (_startMode == ApplicationStartupMode::InvokeCard)
		session.setFitToWidth (true);
	else {
		_pageView->setTextZoom (session.getZoomText ());
		_pageView->setScrollPositionOnLoad (session.getPosition (),
						    NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
		_pageView->setScrollPositionOnLoad (session.getTextPosition (),
						    NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
	}

	if (_core->isPasswordProtected (session.getFile ())) {
		if (_prompt != NULL)
			return;

		_prompt = new SystemPrompt (this);

		_prompt->setTitle (trUtf8 ("Enter Password"));
		_prompt->inputField()->setEmptyText (trUtf8 ("Enter file password"));
		_prompt->inputField()->setInputMode (SystemUiInputMode::Password);
		_prompt->setDismissAutomatically (false);

		bool res = connect (_prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
				    this, SLOT (onPasswordDialogFinished (bb::system::SystemUiResult::Type)));

		if (res) {
			NSRSession *newSession = new NSRSession (session);
			_prompt->setProperty ("session", qVariantFromValue (reinterpret_cast<void *> (newSession)));
			_prompt->show ();
		} else {
			_prompt->deleteLater ();
			_prompt = NULL;
		}
	} else
		_core->loadSession (&session);
}

void
NSRReaderBB10::saveSession ()
{
	NSRSession	session;

	if (_startMode == ApplicationStartupMode::InvokeCard)
		return;

	if (!_core->isDocumentOpened ())
		return;

	if (!_core->isDestructing () && _core->isPageRendering ())
		return;

	NSRRenderedPage page = _core->getCurrentPage ();

	if (!page.isValid ())
		return;

	/* Save session */
	session.setFile (_core->getDocumentPath ());
	session.setPage (page.getNumber ());
	session.setFitToWidth (page.isZoomToWidth ());
	session.setZoomGraphic (page.getRenderedZoom ());
	session.setRotation (page.getRotation ());
	session.setZoomText (_pageView->getTextZoom ());
	session.setPosition (_pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC));
	session.setTextPosition (_pageView->getScrollPosition (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
	NSRSettings::instance()->saveSession (&session);
}

void
NSRReaderBB10::onIndicatorRequested (bool enabled)
{
	if (_indicator->isRunning () == enabled)
		return;

	_pageView->setZoomEnabled (!enabled);
	_pageView->setActionsEnabled (!enabled);
	_pageView->setGesturesEnabled (!enabled);

	_indicator->setTouchPropagationMode (_pageView->isVisible () ? TouchPropagationMode::None
								     : TouchPropagationMode::Full);

	if (enabled) {
		disableVisualControls ();
		_indicator->setVisible (true);
		_indicator->start ();
	} else
		_indicator->stop ();
}

void
NSRReaderBB10::onIndicatorStopped ()
{
	_indicator->setVisible (false);
}

void
NSRReaderBB10::onRecentDocumentsRequested ()
{
	if (_startMode != ApplicationStartupMode::InvokeCard) {
		TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

		if (pane != NULL)
			pane->setActiveTab (pane->at (NSR_GUI_RECENT_TAB_INDEX));
	}
}

void
NSRReaderBB10::onPasswordDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (res == SystemUiResult::ConfirmButtonSelection) {
		NSRSession *session = reinterpret_cast < NSRSession * > (_prompt->property("session").value<void *> ());

		if (session != NULL) {
			session->setPassword (_prompt->inputFieldTextEntry ());
			_core->loadSession (session);
			delete session;
		}
	}

	_prompt->deleteLater ();
	_prompt = NULL;
}

void
NSRReaderBB10::onAddBookmarkDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (res == SystemUiResult::ConfirmButtonSelection) {
		NSRBookmarksPage *bookmarksPage = getBookmarksPage ();

		if (bookmarksPage != NULL)
			bookmarksPage->addBookmark (_prompt->inputFieldTextEntry (), _core->getCurrentPage().getNumber ());
	}

	_prompt->deleteLater ();
	_prompt = NULL;
}

void
NSRReaderBB10::onPermissionDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (_dialog == NULL)
		return;

	if (_dialog != reinterpret_cast < SystemDialog * > (sender ()))
		return;

	if (res == SystemUiResult::ConfirmButtonSelection)
		NSRFileSharer::getInstance()->invokeUri ("settings://permissions", "sys.settings.card", "bb.action.OPEN");

	_dialog->deleteLater ();
	_dialog = NULL;
}

void
NSRReaderBB10::onErrorWhileOpening (NSRAbstractDocument::NSRDocumentError error)
{
	QString errorStr;

	_isWaitingForFirstPage = false;

	if (error == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		errorStr = trUtf8 ("Seems that entered password is wrong or "
				   "encryption algorithm is not supported.\n"
				   "Please do not blame hard NSR Reader for "
				   "that because it is only a piece of "
				   "software :)");
	else
		errorStr = trUtf8 ("Unknown error! Maybe file is broken.\n"
				   "NSR Reader tried to open this file, but it can't :( "
				   "Please check this file on desktop computer.");

	/* Reset flag to prevent slider visible after password entering */
	_wasSliderVisible = false;

	showToast (errorStr, true);
}

void
NSRReaderBB10::onSystemLanguageChanged ()
{
	QString 	locale_string = QLocale().name ();
#ifdef NSR_CORE_LITE_VERSION
	QString		filename = QString("nsrreader_bb10_lite_%1").arg (locale_string);
#else
	QString		filename = QString("nsrreader_bb10_%1").arg (locale_string);
#endif

	QCoreApplication::instance()->removeTranslator (_qtranslator);

	if (_qtranslator->load (filename, "app/native/qm"))
		QCoreApplication::installTranslator (_qtranslator);

	retranslateUi ();
	NSRGlobalNotifier::instance()->languageChangedSignal ();
}

void
NSRReaderBB10::onPageTapped ()
{
	if (!_core->isDocumentOpened ())
		return;

	if (!_isFullscreen) {
		_pageStatus->setVisible (!_pageStatus->isVisible ());
		return;
	}

	if (_page->actionBarVisibility () == ChromeVisibility::Hidden)
		_page->setActionBarVisibility (ChromeVisibility::Overlay);
	else
		_page->setActionBarVisibility (ChromeVisibility::Hidden);

	_pageStatus->setVisible (_page->actionBarVisibility () == ChromeVisibility::Overlay);
	_slider->setBottomSpace (getActionBarHeight ());
}

void
NSRReaderBB10::onPopTransitionEnded (bb::cascades::Page *page)
{
	if (dynamic_cast<NSRPreferencesPage *> (page) != NULL) {
		NSRPreferencesPage *prefsPage = dynamic_cast < NSRPreferencesPage * > (page);
		prefsPage->saveSettings ();
		_core->reloadSettings ();
		_actionAggregator->setActionEnabled ("prefs", true);
	} else if (dynamic_cast < NSRAboutPage * > (page) != NULL)
		_actionAggregator->setActionEnabled ("help", true);

	if (page != NULL)
		delete page;
}

void
NSRReaderBB10::onLastDocumentRequested (const QString& path)
{
	onFileSelected (QStringList (path));

	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (pane != NULL)
		pane->setActiveTab (pane->at (NSR_GUI_MAIN_TAB_INDEX));
}

void
NSRReaderBB10::onDocumentToBeDeleted (const QString& path)
{
	if (!_core->isDocumentOpened () || _core->getDocumentPath () != path)
		return;

	_core->closeDocument ();

	resetState ();
	updateVisualControls ();
}

void
NSRReaderBB10::onZoomChanged (double zoom, NSRRenderRequest::NSRRenderReason reason)
{
	if (reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
	    reason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_core->setScreenWidth (_pageView->getSize().width ());

	_core->setZoom (zoom, reason);
}

void
NSRReaderBB10::onManualExit ()
{
	_core->prepareForDestruction ();

	saveSession ();

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		CardDoneMessage doneMessage;

		doneMessage.setReason ("Exit");
		_invokeManager->sendCardDone (doneMessage);
	}

	Application::instance()->quit ();
}

void
NSRReaderBB10::onPrevPageRequested ()
{
	if (_actionAggregator->isActionEnabled ("prev"))
		onPrevPageActionTriggered ();
}

void
NSRReaderBB10::onNextPageRequested ()
{
	if (_actionAggregator->isActionEnabled ("next"))
		onNextPageActionTriggered ();
}

void
NSRReaderBB10::onFitToWidthRequested ()
{
	_pageView->fitToWidth (NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH);
}

void
NSRReaderBB10::onInvoke (const bb::system::InvokeRequest& req)
{
	QString	target = req.target ();
	QString	file = req.uri().toLocalFile ();
	bool	ok;
	int	page = req.data().toInt (&ok);

	if (!ok)
		page = -1;

#ifdef NSR_CORE_LITE_VERSION
	if (target == "com.gmail.lite.reader.nsr") {
#else
	if (target == "com.gmail.reader.nsr") {
#endif
		saveSession ();
		loadSession (file, page);
#ifdef NSR_CORE_LITE_VERSION
	} else if (target == "com.gmail.lite.reader.nsr.viewer")
#else
	} else if (target == "com.gmail.reader.nsr.viewer")
#endif
		loadSession (file, page);

	if (!_core->isDocumentOpened ())
		_welcomeView->setCardMode (_startMode == ApplicationStartupMode::InvokeCard);
}

void
NSRReaderBB10::onRotateLeftRequested ()
{
	if (_core->isDocumentOpened ())
		_core->rotate (NSRReaderCore::ROTATE_DIRECTION_LEFT);
}

void
NSRReaderBB10::onRotateRightRequested ()
{
	if (_core->isDocumentOpened ())
		_core->rotate (NSRReaderCore::ROTATE_DIRECTION_RIGHT);
}

void
NSRReaderBB10::onFullscreenSwitchRequested (bool isFullscreen)
{
	if (isFullscreen == _isFullscreen)
		return;

	_isFullscreen = isFullscreen;

	if (!_core->isDocumentOpened ())
		return;

	if (_isFullscreen) {
		_page->setActionBarVisibility (ChromeVisibility::Hidden);
		_pageStatus->setVisible (false);
	} else
		_page->setActionBarVisibility (ChromeVisibility::Visible);

	_slider->setBottomSpace (getActionBarHeight ());
}

void
NSRReaderBB10::onPreventScreenLockSwitchRequested (bool isPreventScreenLock)
{
	if (isPreventScreenLock)
		Application::instance()->mainWindow()->setScreenIdleMode (ScreenIdleMode::KeepAwake);
	else
		Application::instance()->mainWindow()->setScreenIdleMode (ScreenIdleMode::Normal);
}

void
NSRReaderBB10::onCardPooled (const bb::system::CardDoneMessage& message)
{
	Q_UNUSED (message);

	_core->closeDocument ();

	resetState ();
	updateVisualControls ();
	setVolumeKeysEnabled (false);
}

void
NSRReaderBB10::onCardResize (const bb::system::CardResizeMessage& message)
{
	_invokeManager->cardResized (message);
}

void
NSRReaderBB10::resetState ()
{
	_pageView->resetPage ();
	_pageView->setViewMode (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
	_pageStatus->setStatus (0, 0);
	_slider->setVisible (false);
	_slider->setRange (0, 0);
	_slider->setValue (0);
}

void
NSRReaderBB10::showAboutPage (NSRAboutPage::NSRAboutSection section)
{
	_actionAggregator->setActionEnabled ("help", false);
	_naviPane->push (new NSRAboutPage (section));
}

void
NSRReaderBB10::showToast (const QString& text, bool reset)
{
	if (reset)
		_welcomeView->setCardMode (_startMode == ApplicationStartupMode::InvokeCard);

	_toast->cancel ();
	_toast->setBody (text);
	_toast->button()->setLabel ("OK");
	_toast->show ();

	if (reset) {
		resetState ();
		updateVisualControls ();
	}
}

int
NSRReaderBB10::getActionBarHeight ()
{
	return getActionBarHeightForOrientation (OrientationSupport::instance()->orientation ());
}

int
NSRReaderBB10::getActionBarHeightForOrientation (bb::cascades::UIOrientation::Type orientation)
{
	if (!_isFullscreen)
		return 0;

	if (_page->actionBarVisibility () == ChromeVisibility::Hidden ||
	    _page->actionBarVisibility () == ChromeVisibility::Visible)
		return 0;

	if (_bpsHandler->isVkbVisible ())
		return 0;

	QSize displaySize = DisplayInfo().pixelSize ();

	if (displaySize.width () == displaySize.height ())
		return NSR_GUI_ACTION_BAR_REDUCED_HEIGHT;
	else {
		if (orientation == UIOrientation::Portrait)
			return NSR_GUI_ACTION_BAR_NORMAL_HEIGHT;
		else
			return NSR_GUI_ACTION_BAR_REDUCED_HEIGHT;
	}
}

void
NSRReaderBB10::onThumbnail ()
{
	setVolumeKeysEnabled (false);

	NSRSceneCover *cover = dynamic_cast < NSRSceneCover * > (Application::instance()->cover ());

	if (cover == NULL)
		return;

	_isActiveFrame = true;

	if (_core->isDocumentOpened ()) {
		NSRRenderedPage page = _core->getCurrentPage ();

		if (page.isValid ()) {
			cover->setPageData (page,
					    QFileInfo(_core->getDocumentPath ()).fileName (),
					    _core->getPagesCount ());
			cover->setTextOnly (_core->isTextReflow ());
			cover->setInvertedColors (_core->isInvertedColors ());
			cover->updateState (false);
		} else
			cover->updateState (true);
	} else
		cover->updateState (true);
}

void
NSRReaderBB10::onFullscreen ()
{
	setVolumeKeysEnabled (true);

	NSRSceneCover *cover = dynamic_cast < NSRSceneCover * > (Application::instance()->cover ());

	if (cover == NULL)
		return;

	_isActiveFrame = false;

	cover->resetPageData ();
	cover->updateState (true);
}

void
NSRReaderBB10::onOrientationAboutToChange (bb::cascades::UIOrientation::Type orientation)
{
	if (_page->actionBarVisibility () != ChromeVisibility::Hidden)
		_slider->setBottomSpace (getActionBarHeightForOrientation (orientation));
}

void
NSRReaderBB10::onPageSliderInteractionStarted ()
{
	_pageStatus->setVisible (true);
}

void
NSRReaderBB10::onPageSliderInteractionEnded ()
{
	int pageNum = _slider->getValue ();

	if (pageNum != _core->getCurrentPage().getNumber ())
		_core->navigateToPage (NSRReaderCore::PAGE_LOAD_CUSTOM, pageNum);
}

void
NSRReaderBB10::onPageSliderValueChanged (int value)
{
	_pageStatus->setStatus (value, _core->getPagesCount ());
}

void
NSRReaderBB10::onVkbVisibilityChanged (bool visible)
{
	if (visible) {
		_wasSliderVisible = _slider->isVisible ();
		_slider->setVisible (false);
	} else
		_slider->setVisible (_wasSliderVisible);
}

void
NSRReaderBB10::onBookmarkChanged (int page, bool removed)
{
	/* It's a crap, that shouldn't be happened */
	if (!_core->isDocumentOpened ())
		return;

	if (page != _core->getCurrentPage().getNumber ())
		return;

	retranslateBookmarkAction (!removed);
}

void
NSRReaderBB10::onBookmarkPageRequested (int page)
{
	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (pane != NULL)
		pane->setActiveTab (pane->at (NSR_GUI_MAIN_TAB_INDEX));

	if (page != _core->getCurrentPage().getNumber ())
		_core->navigateToPage (NSRReaderCore::PAGE_LOAD_CUSTOM, page);
}

void
NSRReaderBB10::onDocumentOpened ()
{
	_isWaitingForFirstPage = true;

	if (_startMode == ApplicationStartupMode::InvokeCard)
		return;

	onPreventScreenLockSwitchRequested (NSRSettings::instance()->isPreventScreenLock ());
}

void
NSRReaderBB10::onDocumentClosed ()
{
	_isWaitingForFirstPage = false;

	if (_startMode == ApplicationStartupMode::InvokeCard)
		return;

	onPreventScreenLockSwitchRequested (false);
}

#ifdef NSR_CORE_LITE_VERSION
void
NSRReaderBB10::onLiteVersionOverPage ()
{
	QString text = trUtf8("Lite version of NSR Reader allows to read only "
			      "first %1 pages of the file. If you want to read "
			      "larger files, please consider buying the full "
			      "version.").arg (NSRSettings::getMaxAllowedPages ());
	showToast (text, false);
}

void
NSRReaderBB10::onBuyActionTriggered ()
{
	NSRFileSharer::getInstance()->invokeUri ("appworld://content/27985686", "sys.appworld", "bb.action.OPEN");
}
#endif

void
NSRReaderBB10::retranslateUi ()
{
	_filePicker->setTitle (trUtf8 ("Select File", "Open file window"));

	bool hasBookmark = false;
	NSRBookmarksPage *bookmarksPage = getBookmarksPage ();

	if (bookmarksPage != NULL)
		hasBookmark = bookmarksPage->hasBookmark (_core->getCurrentPage().getNumber ());

	retranslateBookmarkAction (hasBookmark);

	_translator->translate ();
}

void
NSRReaderBB10::retranslateBookmarkAction (bool hasBookmark)
{
	ActionItem *bookmarkAction = static_cast < ActionItem *> (_actionAggregator->actionByName ("bookmark"));

	if (bookmarkAction != NULL) {
		bookmarkAction->setTitle (hasBookmark ? trUtf8 ("Edit Bookmark") : trUtf8 ("Add Bookmark"));
		bookmarkAction->setImageSource (hasBookmark ? QUrl ("asset:///bookmark-edit.png") : QUrl ("asset:///bookmark-add.png"));

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
		bookmarkAction->accessibility()->setName (hasBookmark ? trUtf8 ("Edit bookmark for current page")
								      : trUtf8 ("Add bookmark for current page"));
#  endif
#endif
	}
}

void
NSRReaderBB10::setViewMode (NSRAbstractDocument::NSRDocumentStyle mode)
{
	bool needRefit = false;

	needRefit = (_pageView->getViewMode () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT) &&
		    (mode == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) &&
		    !_core->getCurrentPage().isCached () &&
		    _core->getCurrentPage().isAutoCrop () &&
		    _core->isFitToWidth ();

	_pageView->setViewMode (mode);

	if (needRefit)
		_pageView->fitToWidth (NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH);
}

NSRBookmarksPage *
NSRReaderBB10::getBookmarksPage () const
{

	if (_startMode == ApplicationStartupMode::InvokeCard)
		return NULL;

	TabbedPane *pane = dynamic_cast < TabbedPane * > (Application::instance()->scene ());

	if (pane != NULL)
		return dynamic_cast < NSRBookmarksPage * > (pane->at(NSR_GUI_BOOKMARKS_TAB_INDEX)->content ());
	else
		return NULL;
}

void
NSRReaderBB10::setVolumeKeysEnabled (bool enabled)
{
	if (enabled) {
		/* We do not check connect() result here because it can fail if such a connection
		 * pair is already exist */
		connect (_mediaKeys[0], SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
			 this, SLOT (onPrevPageRequested ()),
			 Qt::UniqueConnection);
		connect (_mediaKeys[1], SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
			 this, SLOT (onNextPageRequested ()),
			 Qt::UniqueConnection);
	} else {
		disconnect (_mediaKeys[0], SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
			    this, SLOT (onPrevPageRequested ()));
		disconnect (_mediaKeys[1], SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
			    this, SLOT (onNextPageRequested ()));
	}
}
