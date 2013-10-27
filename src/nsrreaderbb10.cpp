#include "nsrreaderbb10.h"
#include "nsrsettings.h"
#include "nsrsession.h"
#include "nsrpreferencespage.h"
#include "nsrlastdocspage.h"
#include "nsrfilesharer.h"
#include "nsrscenecover.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"

#include <bb/cascades/Application>
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

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,1,0)
#    include <bb/cascades/SystemShortcut>
#    include <bb/cascades/Shortcut>
#  endif
#endif

#include <bb/system/SystemToast>
#include <bb/system/LocaleHandler>

#include <bb/device/DisplayInfo>

#include <bb/multimedia/MediaKeyWatcher>

using namespace bb::system;
using namespace bb::cascades;
using namespace bb::cascades::pickers;
using namespace bb::device;
using namespace bb::multimedia;

#define NSR_ACTION_BAR_NORMAL_HEIGHT	140
#define NSR_ACTION_BAR_REDUCED_HEIGHT	100

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app),
	_core (NULL),
	_pageView (NULL),
	_pageStatus (NULL),
	_readProgress (NULL),
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
	_invokeManager (NULL),
	_startMode (ApplicationStartupMode::LaunchApplication),
	_isFullscreen (false),
	_isActiveFrame (false),
	_wasSliderVisible (false)
{
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

	_startMode = _invokeManager->startupMode ();

	initFullUI ();
}

NSRReaderBB10::~NSRReaderBB10 ()
{
}

void
NSRReaderBB10::initFullUI ()
{
	_translator = new NSRTranslator (this);
	_qtranslator = new QTranslator (this);

	Container *rootContainer = new Container ();
	rootContainer->setLayout (DockLayout::create ());
	rootContainer->setHorizontalAlignment (HorizontalAlignment::Fill);
	rootContainer->setVerticalAlignment (VerticalAlignment::Fill);

	_toast = new SystemToast (this);

	_pageView = new NSRPageView ();
	_pageView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_pageView->setVerticalAlignment (VerticalAlignment::Fill);
	_pageView->setVisible (false);
	_indicator = ActivityIndicator::create().horizontal(HorizontalAlignment::Fill)
						.vertical(VerticalAlignment::Fill)
						.visible(false);

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
	_pageStatus->setHorizontalAlignment(HorizontalAlignment::Left);
	_pageStatus->setVerticalAlignment(VerticalAlignment::Top);
	_pageStatus->setStatus (0, 0);

	ok = connect (_pageView, SIGNAL (viewTapped ()), this, SLOT (onPageTapped ()));
	Q_ASSERT (ok);

	_welcomeView = new NSRWelcomeView ();
	/* We need welcome view only if not loading file at start up */
	_welcomeView->setCardMode (true);

	ok = connect (_welcomeView, SIGNAL (openDocumentRequested ()), this, SLOT (onOpenActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (_welcomeView, SIGNAL (recentDocumentsRequested ()), this, SLOT (onRecentDocsTriggered ()));
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
	rootContainer->setBackground (Color::Black);
	rootContainer->setLayoutProperties (StackLayoutProperties::create().spaceQuota (1.0));

	_readProgress = new NSRReadProgress ();

	Container *mainContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ())
						      .background(Color::Black);

	mainContainer->add (rootContainer);
	mainContainer->add (_readProgress);

	_page = Page::create().content (mainContainer);
	_actionAggregator = new NSRActionAggregator (this);

	ok = connect (_page, SIGNAL (peekedAtChanged (bool)), this, SLOT (onTopPagePeeked (bool)));
	Q_ASSERT (ok);

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
	ActionItem *prefsAction = ActionItem::create().title(trUtf8 ("Settings"));
	ActionItem *recentDocsAction = ActionItem::create().title (trUtf8 ("Recent"));
	ActionItem *helpAction = ActionItem::create().title (trUtf8 ("About", "About a program, window title"));
	ActionItem *shareAction = ActionItem::create().enabled (false);
	shareAction->setTitle (trUtf8 ("Share", "Share file between users"));
#ifdef NSR_LITE_VERSION
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
	_translator->addTranslatable ((UIObject *) prefsAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Settings"));
	_translator->addTranslatable ((UIObject *) recentDocsAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Recent"));
	_translator->addTranslatable ((UIObject *) helpAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("About"));
	_translator->addTranslatable ((UIObject *) shareAction, NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRReaderBB10"),
				      QString ("Share"));
#ifdef NSR_LITE_VERSION
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
	prefsAction->accessibility()->setName (trUtf8 ("Open Settings page"));
	recentDocsAction->accessibility()->setName (trUtf8 ("Open page with recent files"));
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
	_translator->addTranslatable ((UIObject *) prefsAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open Settings page"));
	_translator->addTranslatable ((UIObject *) recentDocsAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open page with recent files"));
	_translator->addTranslatable ((UIObject *) helpAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Open page with information about the app and help sections"));
	_translator->addTranslatable ((UIObject *) shareAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Share file with others"));
#    ifdef NSR_LITE_VERSION
	_translator->addTranslatable ((UIObject *) buyAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRReaderBB10"),
				      QString ("Buy full version of the app in the store"));
#    endif

#  endif
#endif

	_page->addAction (openAction, ActionBarPlacement::OnBar);
	_page->addAction (prevPageAction, ActionBarPlacement::OnBar);
	_page->addAction (nextPageAction, ActionBarPlacement::OnBar);
	_page->addAction (gotoAction, ActionBarPlacement::InOverflow);
	_page->addAction (recentDocsAction, ActionBarPlacement::InOverflow);
	_page->addAction (shareAction, ActionBarPlacement::InOverflow);

	_actionAggregator->addAction ("open", openAction);
	_actionAggregator->addAction ("prev", prevPageAction);
	_actionAggregator->addAction ("next", nextPageAction);
	_actionAggregator->addAction ("goto", gotoAction);
	_actionAggregator->addAction ("recent-docs", recentDocsAction);
	_actionAggregator->addAction ("share", shareAction);
	_actionAggregator->addAction ("prefs", prefsAction);
	_actionAggregator->addAction ("help", helpAction);

	openAction->setImageSource (QUrl ("asset:///open.png"));
	prevPageAction->setImageSource (QUrl ("asset:///previous.png"));
	nextPageAction->setImageSource (QUrl ("asset:///next.png"));
	gotoAction->setImageSource (QUrl ("asset:///goto.png"));
	prefsAction->setImageSource (QUrl ("asset:///settings.png"));
	recentDocsAction->setImageSource (QUrl ("asset:///recent-documents.png"));
	helpAction->setImageSource (QUrl ("asset:///about.png"));
	shareAction->setImageSource (QUrl ("asset:///share.png"));
#ifdef NSR_LITE_VERSION
	buyAction->setImage (QUrl ("asset:///buy.png"));
#endif

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,1,0)
	SystemShortcut *prevShortcut = SystemShortcut::create (SystemShortcuts::PreviousSection);
	SystemShortcut *nextShortcut = SystemShortcut::create (SystemShortcuts::NextSection);
	Shortcut *openShortcut = Shortcut::create().key("Ctrl + O");
	Shortcut *gotoShortcut = Shortcut::create().key("Ctrl + G");

	prevPageAction->addShortcut (prevShortcut);
	nextPageAction->addShortcut (nextShortcut);
	openAction->addShortcut (openShortcut);
	gotoAction->addShortcut (gotoShortcut);
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

	ok = connect (prefsAction, SIGNAL (triggered ()), this, SLOT (onPrefsActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (recentDocsAction, SIGNAL (triggered ()), this, SLOT (onRecentDocsTriggered ()));
	Q_ASSERT (ok);

	ok = connect (helpAction, SIGNAL (triggered ()), this, SLOT (onHelpActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (shareAction, SIGNAL (triggered ()), this, SLOT (onShareActionTriggered ()));
	Q_ASSERT (ok);

#ifdef NSR_LITE_VERSION
	ok = connect (buyAction, SIGNAL (triggered ()), this, SLOT (onBuyActionTriggered ()));
	Q_ASSERT (ok);
#endif

	Menu *menu = new Menu ();
	menu->addAction (helpAction);
	menu->addAction (prefsAction);
#ifdef NSR_LITE_VERSION
	menu->addAction (buyAction);
#endif
	Application::instance()->setMenu (menu);

	_filePicker = new FilePicker (this);
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);
	_filePicker->setFilter (QStringList ("*.pdf") << "*.djvu" << "*.djv" <<
					     "*.tiff" << "*.tif" << "*.txt");

	_core = new NSRReaderCore (_startMode, this);

	ok = connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		      this, SLOT (onFileSelected (const QStringList&)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (pageRendered (int)), this, SLOT (onPageRendered (int)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (needIndicator (bool)), this, SLOT (onIndicatorRequested (bool)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (needPassword ()), this, SLOT (onPasswordRequested ()));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (errorWhileOpening (NSRAbstractDocument::DocumentError)),
		      this, SLOT (onErrorWhileOpening (NSRAbstractDocument::DocumentError)));
	Q_ASSERT (ok);

	ok = connect (_core, SIGNAL (needViewMode (NSRPageView::NSRViewMode)),
		      this, SLOT (onViewModeRequested (NSRPageView::NSRViewMode)));
	Q_ASSERT (ok);

#ifdef NSR_LITE_VERSION
	ok = connect (_core, SIGNAL (liteVersionOverPage ()), this, SLOT (onLiteVersionOverPage ()));
	Q_ASSERT (ok);
#endif

	_naviPane = NavigationPane::create().add (_page);

	ok = connect (_naviPane, SIGNAL (popTransitionEnded (bb::cascades::Page *)),
		      this, SLOT (onPopTransitionEnded (bb::cascades::Page *)));
	Q_ASSERT (ok);

	Application::instance()->setScene (_naviPane);

	MediaKeyWatcher *volumeUpWatcher = new MediaKeyWatcher (MediaKey::VolumeUp, this);
	MediaKeyWatcher *volumeDownWatcher = new MediaKeyWatcher (MediaKey::VolumeDown, this);

	ok = connect (volumeUpWatcher, SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
		      this, SLOT (onPrevPageRequested ()));
	Q_ASSERT (ok);

	ok = connect (volumeDownWatcher, SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
		      this, SLOT (onNextPageRequested ()));
	Q_ASSERT (ok);

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
		_pageView->setViewMode (NSRPageView::NSR_VIEW_MODE_GRAPHIC);
		_isFullscreen = true;
		onFullscreenSwitchRequested (true);
	} else {
		/* We do need it here to not to read settings in card mode */
		NSRSettings settings;

		_pageView->setInvertedColors (settings.isInvertedColors ());
		_pageView->setViewMode (settings.isWordWrap () ? NSRPageView::NSR_VIEW_MODE_TEXT
							       : NSRPageView::NSR_VIEW_MODE_GRAPHIC);
		onFullscreenSwitchRequested (settings.isFullscreenMode ());
	}

	ok = connect (_pageView, SIGNAL (zoomChanged (double, NSRRenderedPage::NSRRenderReason)),
		      this, SLOT (onZoomChanged (double, NSRRenderedPage::NSRRenderReason)));
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
		return;
	}

	NSRSettings settings;

	if (settings.isFirstStart ()) {
		settings.saveFirstStart ();
		settings.saveNewsShown ();
		showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_HELP);

		_welcomeView->setCardMode (false);

		updateVisualControls ();
	} else {
		if (!settings.isNewsShown ()) {
			settings.saveNewsShown ();
			showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_CHANGES);
		}

		/* Load previously saved session */
		if (QFile::exists (settings.getLastSession().getFile ()))
			loadSession ();
		else {
			_welcomeView->setCardMode (false);
			updateVisualControls ();
		}
	}
}

void
NSRReaderBB10::initCardUI ()
{
	if (_startMode != ApplicationStartupMode::InvokeCard)
		return;

	_page->removeAction (_actionAggregator->removeAction ("open"));
	_page->removeAction (_actionAggregator->removeAction ("recent-docs"));
	_page->removeAction (_actionAggregator->removeAction ("share"));

	_pageView->setInvertedColors (false);
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	NSRSettings settings;

	if (_core->getDocumentPath () == files.first ()) {
		_toast->setBody (trUtf8 ("Selected file is already opened"));
		_toast->resetButton ();
		_toast->show ();
		return;
	}

	QFileInfo finfo (files.first ());

	/* Save session for opened document */
	settings.saveLastOpenDir (finfo.canonicalPath ());

	saveSession ();
	loadSession (finfo.canonicalFilePath ());
}

void
NSRReaderBB10::onOpenActionTriggered ()
{
	_filePicker->setDirectories (QStringList (NSRSettings().getLastOpenDir ()));
	_filePicker->open ();
}

void
NSRReaderBB10::onPrevPageActionTriggered ()
{
	_core->saveCurrentPagePositions (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_GRAPHIC),
					 _pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_TEXT));
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_PREV);
}

void
NSRReaderBB10::onNextPageActionTriggered ()
{
	_core->saveCurrentPagePositions (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_GRAPHIC),
					 _pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_TEXT));
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_NEXT);
}

void
NSRReaderBB10::onGotoActionTriggered ()
{
	_slider->setBottomSpace (getActionBarHeight ());
	_slider->setVisible (!_slider->isVisible ());
	_readProgress->setVisible (!_slider->isVisible ());
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

	_naviPane->push (prefsPage);
}

void
NSRReaderBB10::onRecentDocsTriggered ()
{
	NSRLastDocsPage *page = new NSRLastDocsPage ();

	bool ok = connect (page, SIGNAL (requestDocument (QString)), this, SLOT (onLastDocumentRequested (QString)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (page, SIGNAL (documentToBeDeleted (QString)), this, SLOT (onDocumentToBeDeleted (QString)));
	Q_ASSERT (ok);

	ActionItem *pageBackAction = ActionItem::create();
	page->setPaneProperties (NavigationPaneProperties::create().backButton(pageBackAction));

	ok = connect (pageBackAction, SIGNAL (triggered ()), this, SLOT (onBackButtonTriggered ()));
	Q_ASSERT (ok);

	_naviPane->push (page);
}

void
NSRReaderBB10::onHelpActionTriggered ()
{
	showAboutPage (NSRAboutPage::NSR_ABOUT_SECTION_MAIN);
}

void
NSRReaderBB10::onShareActionTriggered ()
{
	if (!_core->isDocumentOpened ())
		return;

	NSRFileSharer::getInstance()->shareFiles (QStringList (_core->getDocumentPath ()));
}

void
NSRReaderBB10::onPageRendered (int number)
{
	int pagesCount = _core->getPagesCount ();
	NSRRenderedPage page = _core->getCurrentPage ();

	_pageView->setZoomRange (_core->getMinZoom (), _core->getMaxZoom ());
	_pageView->setPage (page);

	_pageStatus->setStatus (number, pagesCount);
	_readProgress->setCurrentPage (number);
	_readProgress->setPagesCount (pagesCount);
	_slider->setRange (1, pagesCount);
	_slider->setValue (number);

	/* Fit cropped page to width only in graphic mode,
	 * cached pages should be already cropped and fitted */
	if (_pageView->getViewMode () == NSRPageView::NSR_VIEW_MODE_GRAPHIC) {
		if (_core->isFitToWidth () && page.isCropped () && !page.isCached () &&
		    page.getRenderReason () != NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH)
			_pageView->fitToWidth (NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH);
	}

	if (_isActiveFrame)
		onThumbnail ();
}

void
NSRReaderBB10::updateVisualControls ()
{
	_actionAggregator->setActionEnabled ("open", true);
	_actionAggregator->setActionEnabled ("prefs", true);
	_actionAggregator->setActionEnabled ("recent-docs", true);
	_actionAggregator->setActionEnabled ("share",
				       	     _core->isDocumentOpened () &&
				       	     NSRFileSharer::isSharable (_core->getDocumentPath ()));
	_pageView->setVisible (_core->isDocumentOpened ());
	_welcomeView->setVisible (!_core->isDocumentOpened ());
	_readProgress->setVisible (!_slider->isVisible () && _core->isDocumentOpened () && _core->getPagesCount () > 1);
	_slider->setEnabled (_core->isDocumentOpened ());

	if (!_core->isDocumentOpened ()) {
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

		/* Maybe action bar is not autohidden after previous document failed to be opened? */
		if (_isFullscreen && _page->actionBarVisibility () == ChromeVisibility::Visible)
			_page->setActionBarVisibility (ChromeVisibility::Hidden);
	}
}

void
NSRReaderBB10::disableVisualControls ()
{
	_actionAggregator->setActionEnabled ("open", false);
	_actionAggregator->setActionEnabled ("prev", false);
	_actionAggregator->setActionEnabled ("next", false);
	_actionAggregator->setActionEnabled ("goto", false);
	_actionAggregator->setActionEnabled ("recent-docs", false);
	_actionAggregator->setActionEnabled ("share", false);
	_actionAggregator->setActionEnabled ("prefs", false);
	_slider->setEnabled (false);
}

void
NSRReaderBB10::reloadSettings ()
{
	NSRSettings settings;

	_core->reloadSettings (&settings);
	_pageView->setInvertedColors (settings.isInvertedColors ());

	/* Check whether we have noted user about text mode */
	if (settings.isWordWrap () && !settings.isTextModeNoted ()) {
		settings.saveTextModeNoted ();
		QString text = trUtf8 ("You are using text reflow the first time. Note that "
				       "file formatting may be differ than in original one, "
				       "no images displayed and page can be empty if there is "
				       "no text in the file. Also text may not be displayed "
				       "properly if appropriate language is not supported by phone",
				       "Text reflow is a view mode of PDF/DjVu files when "
				       "only text information without images is displayed with "
				       "word wrap feature enabled. Use pinch gesture to adjust "
				       "text size");
		showToast (text, false);
	}
}

void
NSRReaderBB10::loadSession (const QString& path, int page)
{
	NSRSession	session;
	int		width = _pageView->getSize().width ();

	if (!path.isEmpty () && !QFile::exists (path)) {
		QString errorStr = trUtf8 ("It seems that selected file doesn't exist anymore "
					   "or NSR Reader doesn't have Shared Files permission. Please "
					   "check permissions at Settings->Security and Privacy->"
					   "Application Permissions");

		showToast (errorStr, true);
		return;
	}

	if (_startMode == ApplicationStartupMode::InvokeCard) {
		session.setFile (path);
		session.setPage (1);
	} else {
		if (path.isEmpty ())
			session = NSRSettings().getLastSession ();
		else
			session = NSRSettings().getSessionForFile (path);
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
				NSRPageView::NSR_VIEW_MODE_GRAPHIC);
		_pageView->setScrollPositionOnLoad (session.getTextPosition (),
				NSRPageView::NSR_VIEW_MODE_TEXT);
	}

	_core->loadSession (&session);
}

void
NSRReaderBB10::saveSession ()
{
	NSRSession	session;
	NSRSettings	settings;

	if (_startMode == ApplicationStartupMode::InvokeCard)
		return;

	if (!_core->isDocumentOpened () || _core->isPageRendering ())
		return;

	session.setFile (_core->getDocumentPath ());
	session.setPage (_core->getCurrentPage().getNumber ());
	session.setFitToWidth (_core->isFitToWidth ());
	session.setZoomGraphic (_core->getZoom ());
	session.setRotation (_core->getRotation ());
	session.setZoomText (_pageView->getTextZoom ());
	session.setPosition (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_GRAPHIC));
	session.setTextPosition (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_TEXT));
	settings.saveSession (&session);
}

void
NSRReaderBB10::onIndicatorRequested (bool enabled)
{
	if (_indicator->isRunning () == enabled)
		return;

	_pageView->setZoomEnabled (!enabled);

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
	updateVisualControls ();
}

void
NSRReaderBB10::onPasswordRequested ()
{
	if (_prompt != NULL)
		return;

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter Password"));
	_prompt->inputField()->setEmptyText (trUtf8 ("Enter file password"));
	_prompt->inputField()->setInputMode (SystemUiInputMode::Password);
	_prompt->setDismissAutomatically (false);

	bool res = connect (_prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			    this, SLOT (onPasswordDialogFinished (bb::system::SystemUiResult::Type)));

	if (res)
		_prompt->exec ();
	else {
		_prompt->deleteLater ();
		_prompt = NULL;
	}
}

void
NSRReaderBB10::onPasswordDialogFinished (bb::system::SystemUiResult::Type res)
{
	if (res == SystemUiResult::ConfirmButtonSelection)
		_core->setPassword (_prompt->inputFieldTextEntry ());

	_prompt->deleteLater ();
	_prompt = NULL;
}

void
NSRReaderBB10::onErrorWhileOpening (NSRAbstractDocument::DocumentError error)
{
	QString errorStr;

	if (error == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		errorStr = trUtf8 ("Seems that entered password is wrong or "
				   "encryption algorithm is not supported.\n"
				   "Please do not blame hard NSR Reader for "
				   "that because it is only a piece of "
				   "software :)");
	else
		errorStr = trUtf8 ("Unknown error! Maybe file is broken.\n"
				   "NSR Reader tried open this file, but it can't :( "
				   "Please check this file on desktop computer");

	showToast (errorStr, true);
}

void
NSRReaderBB10::onSystemLanguageChanged ()
{
	QString 	locale_string = QLocale().name ();
	QString		filename = QString("nsrreader_bb10_%1").arg (locale_string);

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

	if (!_pageStatus->isVisible ())
		_pageStatus->setOnScreen (true);

	if (!_isFullscreen)
		return;

	if (_page->actionBarVisibility () == ChromeVisibility::Hidden)
		_page->setActionBarVisibility (ChromeVisibility::Overlay);
	else
		_page->setActionBarVisibility (ChromeVisibility::Hidden);

	_slider->setBottomSpace (getActionBarHeight ());
}

void
NSRReaderBB10::onViewModeRequested (NSRPageView::NSRViewMode mode)
{
	NSRPageView::NSRViewMode	newMode;
	bool				needRefit = false;

	if (mode == NSRPageView::NSR_VIEW_MODE_PREFERRED) {
		if (_startMode == ApplicationStartupMode::InvokeCard)
			newMode = NSRPageView::NSR_VIEW_MODE_GRAPHIC;
		else
			newMode = NSRSettings().isWordWrap () ? NSRPageView::NSR_VIEW_MODE_TEXT
							      : NSRPageView::NSR_VIEW_MODE_GRAPHIC;
	} else
		newMode = mode;

	needRefit = (_pageView->getViewMode () == NSRPageView::NSR_VIEW_MODE_TEXT) &&
		    (newMode == NSRPageView::NSR_VIEW_MODE_GRAPHIC) &&
		    !_core->getCurrentPage().isCached () &&
		    _core->getCurrentPage().isCropped () &&
		    _core->isFitToWidth ();

	_pageView->setViewMode (newMode);

	if (needRefit)
		_pageView->fitToWidth (NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH);
}

void
NSRReaderBB10::onPopTransitionEnded (bb::cascades::Page *page)
{
	if (dynamic_cast<NSRPreferencesPage *> (page) != NULL) {
		NSRPreferencesPage *prefsPage = dynamic_cast<NSRPreferencesPage *> (page);
		prefsPage->saveSettings ();

		reloadSettings ();

		_actionAggregator->setActionEnabled ("prefs", true);
	} else if (dynamic_cast<NSRAboutPage *> (page) != NULL)
		_actionAggregator->setActionEnabled ("help", true);

	if (page != NULL)
		delete page;
}

void
NSRReaderBB10::onLastDocumentRequested (const QString& path)
{
	_naviPane->pop ();

	onFileSelected (QStringList (path));
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
NSRReaderBB10::onZoomChanged (double zoom, NSRRenderedPage::NSRRenderReason reason)
{
	if (reason == NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
	    reason == NSRRenderedPage::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_core->setScreenWidth (_pageView->getSize().width ());

	_core->setZoom (zoom, reason);
}

void
NSRReaderBB10::onManualExit ()
{
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
	_pageView->fitToWidth (NSRRenderedPage::NSR_RENDER_REASON_ZOOM_TO_WIDTH);
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

#ifdef NSR_LITE_VERSION
	if (target == "com.gmail.lite.reader.nsr") {
#else
	if (target == "com.gmail.reader.nsr") {
#endif
		saveSession ();
		loadSession (file, page);
#ifdef NSR_LITE_VERSION
	} else if (target == "com.gmail.lite.reader.nsr.viewer") {
#else
	} else if (target == "com.gmail.reader.nsr.viewer") {
#endif
		initCardUI ();
		loadSession (file, page);
	}

	if (!_core->isDocumentOpened ())
		_welcomeView->setCardMode (_startMode == ApplicationStartupMode::InvokeCard);
}

void
NSRReaderBB10::onRotateLeftRequested ()
{
	if (_core->isDocumentOpened ())
		_core->rotate (-90);
}

void
NSRReaderBB10::onRotateRightRequested ()
{
	if (_core->isDocumentOpened ())
		_core->rotate (90);
}

void
NSRReaderBB10::onFullscreenSwitchRequested (bool isFullscreen)
{
	if (isFullscreen == _isFullscreen)
		return;

	_isFullscreen = isFullscreen;

	if (_isFullscreen)
		_page->setActionBarVisibility (ChromeVisibility::Hidden);
	else
		_page->setActionBarVisibility (ChromeVisibility::Visible);

	_slider->setBottomSpace (getActionBarHeight ());
}

void
NSRReaderBB10::onCardPooled (const bb::system::CardDoneMessage& message)
{
	Q_UNUSED (message);

	_core->closeDocument ();

	resetState ();
	updateVisualControls ();
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
	_pageView->setViewMode (NSRPageView::NSR_VIEW_MODE_GRAPHIC);
	_pageStatus->setStatus (0, 0);
	_readProgress->setVisible (false);
	_readProgress->setPagesCount (0);
	_readProgress->setCurrentPage (0);
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

	if (_page->actionBarVisibility () == ChromeVisibility::Hidden)
		return 0;

	if (_bpsHandler->isVkbVisible ())
		return 0;

	QSize	displaySize = DisplayInfo().pixelSize ();

	if (displaySize.width () == displaySize.height ())
		return NSR_ACTION_BAR_REDUCED_HEIGHT;
	else {
		if (orientation == UIOrientation::Portrait)
			return NSR_ACTION_BAR_NORMAL_HEIGHT;
		else
			return NSR_ACTION_BAR_REDUCED_HEIGHT;
	}
}

void
NSRReaderBB10::onBackButtonTriggered ()
{
	NSRLastDocsPage *page = dynamic_cast<NSRLastDocsPage *> (_naviPane->top ());

	if (page != NULL)
		page->finishToast ();

	_naviPane->pop ();
}

void
NSRReaderBB10::onTopPagePeeked (bool isPeeked)
{
	if (isPeeked) {
		NSRLastDocsPage *page = dynamic_cast<NSRLastDocsPage *> (_naviPane->top ());

		if (page != NULL)
			page->finishToast ();
	}
}

void
NSRReaderBB10::onThumbnail ()
{
	NSRSceneCover *cover = dynamic_cast<NSRSceneCover *> (Application::instance()->cover ());

	if (cover == NULL)
		return;

	_isActiveFrame = true;

	if (_core->isDocumentOpened ()) {
		NSRRenderedPage page = _core->getCurrentPage ();

		if (!page.isEmpty ()) {
			cover->setPageData (page,
					    QFileInfo(_core->getDocumentPath ()).fileName (),
					    _core->getPagesCount ());
			cover->setStatic (false);
		} else
			cover->setStatic (true);
	} else
		cover->setStatic (true);
}

void
NSRReaderBB10::onFullscreen ()
{
	NSRSceneCover *cover = dynamic_cast<NSRSceneCover *> (Application::instance()->cover ());

	if (cover == NULL)
		return;

	_isActiveFrame = false;

	cover->resetPageData ();
	cover->setStatic (true);
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
	_pageStatus->setAutoHide (false);
	_pageStatus->setOnScreen (true);
}

void
NSRReaderBB10::onPageSliderInteractionEnded ()
{
	_pageStatus->setAutoHide (true);

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

#ifdef NSR_LITE_VERSION
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
	_translator->translate ();
}
