#include "nsrreaderbb10.h"
#include "nsrsettings.h"
#include "nsrsession.h"
#include "nsrpreferencespage.h"
#include "nsrlastdocspage.h"
#include "nsraboutpage.h"
#include "nsrfilesharer.h"

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
#include <bb/cascades/SettingsActionItem>

#include <bbndk.h>

#ifdef BBNDK_VERSION_AT_LEAST
#include <bb/cascades/SystemShortcut>
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

#define NSR_QUICK_GUIDE "app/native/assets/Welcome to NSR Reader.pdf"

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app),
	_core (NULL),
	_pageView (NULL),
	_pageStatus (NULL),
	_readProgress (NULL),
	_welcomeView (NULL),
	_actionAggregator (NULL),
	_naviPane (NULL),
	_page (NULL),
	_filePicker (NULL),
	_indicator (NULL),
	_prompt (NULL),
	_toast (NULL),
	_invokeManager (NULL),
	_startMode (ApplicationStartupMode::LaunchApplication),
	_isFullscreen (false)
{
	_invokeManager = new InvokeManager (this);

	connect (_invokeManager, SIGNAL (invoked (const bb::system::InvokeRequest&)),
		 this, SLOT (onInvoke (const bb::system::InvokeRequest&)));

	_startMode = _invokeManager->startupMode ();

	initFullUI ();
}

NSRReaderBB10::~NSRReaderBB10 ()
{
}

void
NSRReaderBB10::initFullUI ()
{
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

	connect (_pageView, SIGNAL (prevPageRequested ()), this, SLOT (onPrevPageRequested ()));
	connect (_pageView, SIGNAL (nextPageRequested ()), this, SLOT (onNextPageRequested ()));
	connect (_pageView, SIGNAL (fitToWidthRequested ()), this, SLOT (onFitToWidthRequested ()));

	_pageStatus = new NSRPageStatus ();
	_pageStatus->setHorizontalAlignment(HorizontalAlignment::Left);
	_pageStatus->setVerticalAlignment(VerticalAlignment::Top);
	_pageStatus->setStatus (0, 0);

	connect (_pageView, SIGNAL (viewTapped ()), this, SLOT (onPageTapped ()));

	_welcomeView = new NSRWelcomeView ();

	connect (_welcomeView, SIGNAL (openDocumentRequested ()), this, SLOT (onOpenActionTriggered ()));
	connect (_welcomeView, SIGNAL (recentDocumentsRequested ()), this, SLOT (onRecentDocsTriggered ()));

	rootContainer->add (_pageView);
	rootContainer->add (_welcomeView);
	rootContainer->add (_indicator);
	rootContainer->add (_pageStatus);
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

	ActionItem *openAction = ActionItem::create().enabled (true);
	openAction->setTitle (trUtf8 ("Open", "Open document"));
	ActionItem *prevPageAction = ActionItem::create().enabled (false);
	prevPageAction->setTitle (trUtf8 ("Previous", "Previous page"));
	ActionItem *nextPageAction = ActionItem::create().enabled (false);
	nextPageAction->setTitle (trUtf8 ("Next", "Next page"));
	ActionItem *gotoAction = ActionItem::create().enabled (false);
	gotoAction->setTitle (trUtf8 ("Go to", "Go to page"));
	SettingsActionItem *prefsAction = SettingsActionItem::create().title(trUtf8 ("Settings"));
	ActionItem *recentDocsAction = ActionItem::create().title (trUtf8 ("Recent Documents"));
	ActionItem *fitToWidthAction = ActionItem::create().enabled (false);
	fitToWidthAction->setTitle (trUtf8 ("Fit to Width", "Fit document to screen width"));
	ActionItem *helpAction = ActionItem::create().title (trUtf8 ("About", "About a program, window title"));
	ActionItem *shareAction = ActionItem::create().enabled (false);
	shareAction->setTitle (trUtf8 ("Share", "Share document between users"));

	_page->addAction (openAction, ActionBarPlacement::OnBar);
	_page->addAction (prevPageAction, ActionBarPlacement::OnBar);
	_page->addAction (nextPageAction, ActionBarPlacement::OnBar);
	_page->addAction (gotoAction, ActionBarPlacement::InOverflow);
	_page->addAction (recentDocsAction, ActionBarPlacement::InOverflow);
	_page->addAction (fitToWidthAction, ActionBarPlacement::InOverflow);
	_page->addAction (shareAction, ActionBarPlacement::InOverflow);

	_actionAggregator->addAction ("open", openAction);
	_actionAggregator->addAction ("prev", prevPageAction);
	_actionAggregator->addAction ("next", nextPageAction);
	_actionAggregator->addAction ("goto", gotoAction);
	_actionAggregator->addAction ("recent-docs", recentDocsAction);
	_actionAggregator->addAction ("fit-to-width", fitToWidthAction);
	_actionAggregator->addAction ("share", shareAction);
	_actionAggregator->addAction ("prefs", prefsAction);
	_actionAggregator->addAction ("help", helpAction);

	openAction->setImageSource (QUrl ("asset:///open.png"));
	prevPageAction->setImageSource (QUrl ("asset:///previous.png"));
	nextPageAction->setImageSource (QUrl ("asset:///next.png"));
	gotoAction->setImageSource (QUrl ("asset:///goto.png"));
	recentDocsAction->setImageSource (QUrl ("asset:///recent-documents.png"));
	fitToWidthAction->setImageSource (QUrl ("asset:///fit-to-width.png"));
	helpAction->setImageSource (QUrl ("asset:///about.png"));
	shareAction->setImageSource (QUrl ("asset:///share.png"));

#ifdef BBNDK_VERSION_AT_LEAST
	SystemShortcut *prevShortcut = SystemShortcut::create (SystemShortcuts::PreviousSection);
	SystemShortcut *nextShortcut = SystemShortcut::create (SystemShortcuts::NextSection);
	SystemShortcut *zoomInShortcut = SystemShortcut::create (SystemShortcuts::ZoomIn);
	SystemShortcut *zoomOutShortcut = SystemShortcut::create (SystemShortcuts::ZoomOut);

	connect (prevShortcut, SIGNAL (triggered ()), this, SLOT (onSystemShortcutTriggered ()));
	connect (nextShortcut, SIGNAL (triggered ()), this, SLOT (onSystemShortcutTriggered ()));
	connect (zoomInShortcut, SIGNAL (triggered ()), this, SLOT (onSystemShortcutTriggered ()));
	connect (zoomOutShortcut, SIGNAL (triggered ()), this, SLOT (onSystemShortcutTriggered ()));

	prevPageAction->addShortcut (prevShortcut);
	nextPageAction->addShortcut (nextShortcut);
#endif

	connect (openAction, SIGNAL (triggered ()), this, SLOT (onOpenActionTriggered ()));
	connect (prevPageAction, SIGNAL (triggered ()), this, SLOT (onPrevPageActionTriggered ()));
	connect (nextPageAction, SIGNAL (triggered ()), this, SLOT (onNextPageActionTriggered ()));
	connect (gotoAction, SIGNAL (triggered ()), this, SLOT (onGotoActionTriggered ()));
	connect (prefsAction, SIGNAL (triggered ()), this, SLOT (onPrefsActionTriggered ()));
	connect (recentDocsAction, SIGNAL (triggered ()), this, SLOT (onRecentDocsTriggered ()));
	connect (fitToWidthAction, SIGNAL (triggered ()), this, SLOT (onFitToWidthTriggered ()));
	connect (helpAction, SIGNAL (triggered ()), this, SLOT (onHelpActionTriggered ()));
	connect (shareAction, SIGNAL (triggered ()), this, SLOT (onShareActionTriggered ()));

	Menu *menu = new Menu ();
	menu->setSettingsAction (prefsAction);
	menu->addAction (helpAction);
	Application::instance()->setMenu (menu);

	_filePicker = new FilePicker (this);
	_filePicker->setTitle (trUtf8 ("Select File", "Open file window"));
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);
	_filePicker->setFilter (QStringList ("*.pdf") << "*.djvu" << "*.djv" <<
					     "*.tiff" << "*.tif" << "*.txt");

	_core = new NSRReaderCore (this);

	connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		 this, SLOT (onFileSelected (const QStringList&)));
	connect (_core, SIGNAL (pageRendered (int)), this, SLOT (onPageRendered (int)));
	connect (_core, SIGNAL (needIndicator (bool)), this, SLOT (onIndicatorRequested (bool)));
	connect (_core, SIGNAL (needPassword ()), this, SLOT (onPasswordRequested ()));
	connect (_core, SIGNAL (errorWhileOpening (NSRAbstractDocument::DocumentError)),
		 this, SLOT (onErrorWhileOpening (NSRAbstractDocument::DocumentError)));
	connect (_core, SIGNAL (needViewMode (NSRPageView::NSRViewMode)),
		 this, SLOT (onViewModeRequested (NSRPageView::NSRViewMode)));

	_naviPane = NavigationPane::create().add (_page);

	connect (_naviPane, SIGNAL (popTransitionEnded (bb::cascades::Page *)),
		 this, SLOT (onPopTransitionEnded (bb::cascades::Page *)));

	Application::instance()->setScene (_naviPane);

	MediaKeyWatcher *volumeUpWatcher = new MediaKeyWatcher (MediaKey::VolumeUp, this);
	MediaKeyWatcher *volumeDownWatcher = new MediaKeyWatcher (MediaKey::VolumeDown, this);

	connect (volumeUpWatcher, SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
		 this, SLOT (onPrevPageRequested ()));
	connect (volumeDownWatcher, SIGNAL (shortPress (bb::multimedia::MediaKey::Type)),
		 this, SLOT (onNextPageRequested ()));

	bb::cascades::LocaleHandler *localeHandler = new bb::cascades::LocaleHandler (this);
	connect (localeHandler, SIGNAL (systemLanguageChanged ()),
		 this, SLOT (onSystemLanguageChanged ()));

	NSRSettings settings;

	_pageView->setViewMode (settings.isWordWrap () ? NSRPageView::NSR_VIEW_MODE_TEXT
							: NSRPageView::NSR_VIEW_MODE_GRAPHIC);
	_isFullscreen = settings.isFullscreenMode ();

	connect (_pageView, SIGNAL (zoomChanged (double, bool)),
		 this, SLOT (onZoomChanged (double, bool)));

	Application::instance()->setAutoExit (false);
	connect (Application::instance (), SIGNAL (manualExit ()),
		 this, SLOT (onManualExit ()));

	/* Initial loading logic:
	 *   - if we are launching from invoke request, wait for further request;
	 *   - in case of first start load quick start guide;
	 *   - in other cases try to load previous session. */

	if (_startMode == ApplicationStartupMode::InvokeApplication ||
	    _startMode == ApplicationStartupMode::InvokeCard) {
		updateVisualControls ();
		return;
	}

	if (settings.isFirstStart ()) {
		settings.saveFirstStart ();
		loadSession (QUrl::fromLocalFile (NSR_QUICK_GUIDE).path ());
	} else {
		/* Load previously saved session */
		if (QFile::exists (settings.getLastSession().getFile ()) &&
				settings.isLoadLastDoc ())
			loadSession ();
		else
			updateVisualControls ();
	}
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	NSRSettings	settings;

	if (_core->getDocumentPath () == files.first ()) {
		_toast->setBody (trUtf8 ("Selected file is already opened."));
		_toast->resetButton ();
		_toast->show ();
		return;
	}

	/* Save session for opened document */
	settings.saveLastOpenDir (QFileInfo (files.first ()).absolutePath ());

	saveSession ();
	loadSession (files.first ());
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
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_PREV);
}

void
NSRReaderBB10::onNextPageActionTriggered ()
{
	_core->navigateToPage (NSRReaderCore::PAGE_LOAD_NEXT);
}

void
NSRReaderBB10::onGotoActionTriggered ()
{
	if (_prompt != NULL)
		return;

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter Page", "Enter page number"));
	_prompt->setBody (trUtf8("Enter page (1 - %1):").arg (_core->getPagesCount ()));
	_prompt->inputField()->setInputMode (SystemUiInputMode::NumericKeypad);
	_prompt->setDismissAutomatically (false);
	_prompt->inputField()->setMaximumLength (QString::number(_core->getPagesCount ()).length ());

	bool res = connect (_prompt, SIGNAL (finished (bb::system::SystemUiResult::Type)),
			    this, SLOT (onGotoDialogFinished (bb::system::SystemUiResult::Type)));

	if (res)
		_prompt->exec ();
	else {
		_prompt->deleteLater ();
		_prompt = NULL;
	}
}

void
NSRReaderBB10::onPrefsActionTriggered ()
{
	_actionAggregator->setActionEnabled ("prefs", false);
	_naviPane->push (new NSRPreferencesPage ());
}

void
NSRReaderBB10::onRecentDocsTriggered ()
{
	NSRLastDocsPage *page = new NSRLastDocsPage ();

	connect (page, SIGNAL (requestDocument (QString)),
		 this, SLOT (onLastDocumentRequested (QString)));

	_naviPane->push (page);
}

void
NSRReaderBB10::onFitToWidthTriggered ()
{
	_pageView->fitToWidth ();
}

void
NSRReaderBB10::onHelpActionTriggered ()
{
	NSRAboutPage *aboutPage = new NSRAboutPage ();
	connect (aboutPage, SIGNAL (startGuideRequested ()), this, SLOT (onStartGuideRequested ()));

	_naviPane->push (aboutPage);
}

void
NSRReaderBB10::onShareActionTriggered ()
{
	if (!_core->isDocumentOpened ())
		return;

	NSRFileSharer::getInstance()->shareFile (_core->getDocumentPath ());
}

void
NSRReaderBB10::onPageRendered (int number)
{
	Q_UNUSED (number);

	_pageView->setZoomRange (_core->getMinZoom (), _core->getMaxZoom ());
	_pageView->setPage (_core->getCurrentPage ());
	_pageStatus->setStatus (_core->getCurrentPage().getNumber (),
				_core->getPagesCount ());
	_readProgress->setCurrentPage (_core->getCurrentPage().getNumber ());
	_readProgress->setPagesCount (_core->getPagesCount ());
	updateVisualControls ();
}

void
NSRReaderBB10::updateVisualControls ()
{
	_actionAggregator->setActionEnabled ("open", true);
	_actionAggregator->setActionEnabled ("prefs", true);
	_actionAggregator->setActionEnabled ("help", true);
	_actionAggregator->setActionEnabled ("recent-docs", true);
	_actionAggregator->setActionEnabled ("fit-to-width",
				       	     _core->isDocumentOpened () &&
				       	     _pageView->getViewMode() == NSRPageView::NSR_VIEW_MODE_GRAPHIC);
	_actionAggregator->setActionEnabled ("share",
				       	     _core->isDocumentOpened () &&
				       	     NSRFileSharer::isSharable (_core->getDocumentPath ()));
	_pageView->setVisible (_core->isDocumentOpened ());
	_welcomeView->setVisible (!_core->isDocumentOpened ());

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
	}
}

void
NSRReaderBB10::disableVisualControls ()
{
	_actionAggregator->setAllEnabled (false);
}

void
NSRReaderBB10::reloadSettings ()
{
	NSRSettings settings;

	_core->reloadSettings (&settings);
	_pageView->setInvertedColors (settings.isInvertedColors ());
	_isFullscreen = settings.isFullscreenMode ();

	if (!_isFullscreen)
		_page->setActionBarVisibility (ChromeVisibility::Visible);

	/* Check whether we have noted user about text mode */
	if (settings.isWordWrap () && !settings.isTextModeNoted ()) {
		settings.saveTextModeNoted ();
		_toast->setBody (trUtf8 ("You are using text reflow the first time. Note that "
					 "document formatting may be differ than in original one, "
					 "no images displayed and page can be empty if there is "
					 "no text in the document. Also text may not be displayed "
					 "properly if appropriate language is not supported by phone.",
					 "Text reflow is a view mode of PDF/DjVu document when "
					 "only text information without images is displayed with "
					 "word wrap feature enabled."));
		_toast->button()->setLabel ("OK");
		_toast->show ();
	}
}

void
NSRReaderBB10::loadSession (const QString& path, int page)
{
	NSRSession	session;
	int		width = _pageView->getSize().width ();

	if (path.isEmpty ())
		session = NSRSettings().getLastSession ();
	else
		session = NSRSettings().getSessionForFile (path);

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
	_pageView->setTextZoom (session.getZoomText ());
	_pageView->setScrollPositionOnLoad (session.getPosition (),
					    NSRPageView::NSR_VIEW_MODE_GRAPHIC);
	_pageView->setScrollPositionOnLoad (session.getTextPosition (),
					    NSRPageView::NSR_VIEW_MODE_TEXT);
	_core->loadSession (&session);
}

void
NSRReaderBB10::saveSession ()
{
	NSRSession	session;
	NSRSettings	settings;

	if (!_core->isDocumentOpened () || _core->isPageRendering ())
		return;

	session.setFile (_core->getDocumentPath ());
	session.setPage (_core->getCurrentPage().getNumber ());
	session.setFitToWidth (_core->isFitToWidth ());
	session.setZoomGraphic (_core->getZoom ());
	session.setZoomText (_pageView->getTextZoom ());
	session.setPosition (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_GRAPHIC));
	session.setTextPosition (_pageView->getScrollPosition (NSRPageView::NSR_VIEW_MODE_TEXT));
	settings.saveSession (&session);
}

void
NSRReaderBB10::zoomIn ()
{
	if (!_core->isDocumentOpened () || _indicator->isVisible ())
		return;

	if (_pageView->getViewMode () == NSRPageView::NSR_VIEW_MODE_GRAPHIC)
		_core->zoomIn ();
	else
		_pageView->setTextZoom (_pageView->getTextZoom () + 10);
}

void
NSRReaderBB10::zoomOut ()
{
	if (!_core->isDocumentOpened () || _indicator->isVisible ())
		return;

	if (_pageView->getViewMode () == NSRPageView::NSR_VIEW_MODE_GRAPHIC)
		_core->zoomOut ();
	else
		_pageView->setTextZoom (_pageView->getTextZoom () - 10);
}

void
NSRReaderBB10::onIndicatorRequested (bool enabled)
{
	disableVisualControls ();
	_indicator->setVisible (enabled);
	_pageView->setZoomEnabled (!enabled);

	if (enabled)
		_indicator->start ();
	else
		_indicator->stop ();
}

void
NSRReaderBB10::onPasswordRequested ()
{
	if (_prompt != NULL)
		return;

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter Password"));
	_prompt->setBody (trUtf8 ("Enter password:"));
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
NSRReaderBB10::onGotoDialogFinished (bb::system::SystemUiResult::Type res)
{
	int pageNum = 0;

	if (res == SystemUiResult::ConfirmButtonSelection)
		pageNum = _prompt->inputFieldTextEntry().toInt ();

	_prompt->deleteLater ();
	_prompt = NULL;

	if (res == SystemUiResult::CancelButtonSelection)
		return;

	if (pageNum != _core->getCurrentPage().getNumber ())
		_core->navigateToPage (NSRReaderCore::PAGE_LOAD_CUSTOM, pageNum);
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
				   "Please check this file on desktop computer.");

	_toast->setBody (errorStr);
	_toast->button()->setLabel ("OK");
	_toast->show ();

	_pageView->resetPage ();
	_pageView->setViewMode (NSRPageView::NSR_VIEW_MODE_GRAPHIC);
	_pageStatus->setStatus (0, 0);
	_readProgress->setPagesCount (0);
	_readProgress->setCurrentPage (0);
	updateVisualControls ();
}

void
NSRReaderBB10::onSystemLanguageChanged ()
{
	QTranslator translator;
	QString locale_string = QLocale().name ();
	QString filename = QString("nsrreader_bb10_%1").arg (locale_string);

	if (translator.load (filename, "app/native/qm"))
		QCoreApplication::instance()->installTranslator (&translator);
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
		_page->setActionBarVisibility (ChromeVisibility::Visible);
	else
		_page->setActionBarVisibility (ChromeVisibility::Hidden);
}

void
NSRReaderBB10::onViewModeRequested (NSRPageView::NSRViewMode mode)
{
	NSRPageView::NSRViewMode newMode;

	if (mode == NSRPageView::NSR_VIEW_MODE_PREFERRED)
		newMode = NSRSettings().isWordWrap () ? NSRPageView::NSR_VIEW_MODE_TEXT
						      : NSRPageView::NSR_VIEW_MODE_GRAPHIC;
	else
		newMode = mode;

	_pageView->setViewMode (newMode);
}

void
NSRReaderBB10::onPopTransitionEnded (bb::cascades::Page *page)
{
	if (dynamic_cast<NSRPreferencesPage *> (page) != NULL) {
		NSRPreferencesPage *prefsPage = dynamic_cast<NSRPreferencesPage *> (page);
		prefsPage->saveSettings ();

		reloadSettings ();
		updateVisualControls ();
	}

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
NSRReaderBB10::onZoomChanged (double zoom, bool toWidth)
{
	if (toWidth)
		_core->setScreenWidth (_pageView->getSize().width ());

	_core->setZoom (zoom, toWidth);
}

void
NSRReaderBB10::onManualExit ()
{
	saveSession ();
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
	if (_actionAggregator->isActionEnabled ("fit-to-width"))
		onFitToWidthTriggered ();
}

void
NSRReaderBB10::onSystemShortcutTriggered ()
{
#ifdef BBNDK_VERSION_AT_LEAST
	SystemShortcut *shortcut = static_cast<SystemShortcut *> (sender ());

	switch (shortcut->type ()) {
	case SystemShortcuts::PreviousSection:
		if (_actionAggregator->isActionEnabled ("prev"))
			onPrevPageActionTriggered ();
		break;
	case SystemShortcuts::NextSection:
		if (_actionAggregator->isActionEnabled ("next"))
			onNextPageActionTriggered ();
		break;
	case SystemShortcuts::ZoomIn:
		zoomIn ();
		break;
	case SystemShortcuts::ZoomOut:
		zoomOut ();
		break;
	default:
		break;
	}
#endif
}

void
NSRReaderBB10::onStartGuideRequested ()
{
	_naviPane->pop ();
	loadSession (QUrl::fromLocalFile (NSR_QUICK_GUIDE).path ());
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

	if (target == "com.gmail.reader.nsr") {
		saveSession ();
		loadSession (file, page);
	}
}
