#include "nsrreaderbb10.h"
#include "nsrsettings.h"
#include "nsrsession.h"

#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Page>
#include <bb/cascades/Color>
#include <bb/cascades/LocaleHandler>

#include <bb/system/SystemToast>
#include <bb/system/LocaleHandler>

using namespace bb::system;
using namespace bb::cascades;
using namespace bb::cascades::pickers;

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app),
	_core (NULL),
	_pageView (NULL),
	_page (NULL),
	_filePicker (NULL),
	_openAction (NULL),
	_prevPageAction (NULL),
	_nextPageAction (NULL),
	_indicator (NULL),
	_prompt (NULL)
{
	Container *rootContainer = new Container ();
	rootContainer->setLayout (DockLayout::create ());
	rootContainer->setHorizontalAlignment (HorizontalAlignment::Fill);
	rootContainer->setVerticalAlignment (VerticalAlignment::Fill);

	_pageView = new NSRPageView ();
	_pageView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_pageView->setVerticalAlignment (VerticalAlignment::Fill);
	_indicator = ActivityIndicator::create().horizontal(HorizontalAlignment::Fill)
						.vertical(VerticalAlignment::Fill);

	connect (_pageView, SIGNAL (viewTapped ()), this, SLOT (onPageTapped ()));

	rootContainer->add (_pageView);
	rootContainer->add (_indicator);
	rootContainer->setBackground (Color::Black);

	_page = Page::create().content (rootContainer);

	_openAction = ActionItem::create().title(trUtf8 ("Open")).enabled (false);
	_prevPageAction = ActionItem::create().title(trUtf8 ("Previous")).enabled (false);
	_nextPageAction = ActionItem::create().title(trUtf8 ("Next")).enabled (false);
	_gotoAction = ActionItem::create().title(trUtf8 ("Go to")).enabled (false);
	_page->addAction (_openAction, ActionBarPlacement::OnBar);
	_page->addAction (_prevPageAction, ActionBarPlacement::OnBar);
	_page->addAction (_nextPageAction, ActionBarPlacement::OnBar);
	_page->addAction (_gotoAction, ActionBarPlacement::InOverflow);

	_openAction->setImageSource (QUrl ("asset:///open.png"));
	_prevPageAction->setImageSource (QUrl ("asset:///previous.png"));
	_nextPageAction->setImageSource (QUrl ("asset:///next.png"));
	_gotoAction->setImageSource (QUrl ("asset:///goto.png"));

	connect (_openAction, SIGNAL (triggered ()), this, SLOT (onOpenActionTriggered ()));
	connect (_prevPageAction, SIGNAL (triggered ()), this, SLOT (onPrevPageActionTriggered ()));
	connect (_nextPageAction, SIGNAL (triggered ()), this, SLOT (onNextPageActionTriggered ()));
	connect (_gotoAction, SIGNAL (triggered ()), this, SLOT (onGotoActionTriggered ()));

	_filePicker = new FilePicker (this);
	_filePicker->setTitle (trUtf8 ("Select file"));
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);
	_filePicker->setFilter (QStringList ("*.pdf") << "*.djvu" <<
					     "*.tiff" << "*.tif" << "*.txt");

	_core = new NSRReaderCore (this);

	connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		 this, SLOT (onFileSelected (const QStringList&)));
	connect (_core, SIGNAL (pageRendered (int)), this, SLOT (onPageRendered (int)));
	connect (_core, SIGNAL (needIndicator (bool)), this, SLOT (onIndicatorRequested (bool)));
	connect (_core, SIGNAL (needPassword ()), this, SLOT (onPasswordRequested ()));
	connect (_core, SIGNAL (errorWhileOpening (NSRAbstractDocument::DocumentError)),
		 this, SLOT (onErrorWhileOpening (NSRAbstractDocument::DocumentError)));

	Application::instance()->setScene (_page);

	bb::cascades::LocaleHandler *localeHandler = new bb::cascades::LocaleHandler (this);
	connect (localeHandler, SIGNAL (systemLanguageChanged ()),
		 this, SLOT (onSystemLanguageChanged ()));

	NSRSettings settings;

	_pageView->setViewMode (settings.isWordWrap () ? NSRPageView::NSR_VIEW_MODE_TEXT
							: NSRPageView::NSR_VIEW_MODE_GRAPHIC);

	/* Load previously saved session */
	if (QFile::exists (settings.getLastSession().getFile ()))
		loadSession ();
	else
		updateVisualControls ();
}

NSRReaderBB10::~NSRReaderBB10 ()
{
	saveSession ();
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	NSRSettings	settings;
	NSRSession	session = settings.getSessionForFile (files.first ());

	/* Save session for opened document */
	saveSession ();

	disableVisualControls ();
	_core->loadSession (&session);
}

void
NSRReaderBB10::onOpenActionTriggered ()
{
	_filePicker->open ();
}

void
NSRReaderBB10::onPrevPageActionTriggered ()
{
	disableVisualControls ();
	_core->loadPage (NSRReaderCore::PAGE_LOAD_PREV);
}

void
NSRReaderBB10::onNextPageActionTriggered ()
{
	disableVisualControls ();
	_core->loadPage (NSRReaderCore::PAGE_LOAD_NEXT);
}

void
NSRReaderBB10::onGotoActionTriggered ()
{
	if (_prompt != NULL)
		return;

	_prompt = new SystemPrompt (this);

	_prompt->setTitle (trUtf8 ("Enter page"));
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
NSRReaderBB10::onPageRendered (int number)
{
	Q_UNUSED (number)

	_pageView->setPage (_core->getCurrentPage());
	updateVisualControls ();
}

void
NSRReaderBB10::updateVisualControls ()
{
	_openAction->setEnabled (true);

	if (!_core->isDocumentOpened ()) {
		_prevPageAction->setEnabled (false);
		_nextPageAction->setEnabled (false);
		_gotoAction->setEnabled (false);
		_page->setActionBarVisibility (ChromeVisibility::Visible);
	} else {
		int totalPages = _core->getPagesCount ();
		int currentPage = _core->getCurrentPage().getNumber ();

		_prevPageAction->setEnabled (totalPages != 1 && currentPage > 1);
		_nextPageAction->setEnabled (totalPages != 1 && currentPage != totalPages);
		_gotoAction->setEnabled (totalPages > 1);
	}
}

void
NSRReaderBB10::disableVisualControls ()
{
	_openAction->setEnabled (false);
	_prevPageAction->setEnabled (false);
	_nextPageAction->setEnabled (false);
	_gotoAction->setEnabled (false);
}

void
NSRReaderBB10::reloadSettings ()
{
	NSRSettings settings;

	_core->reloadSettings (&settings);
}

void
NSRReaderBB10::loadSession ()
{
	NSRSession session = NSRSettings().getLastSession ();

	_core->loadSession (&session);
}

void
NSRReaderBB10::saveSession ()
{
	NSRSession	session;
	NSRSettings	settings;

	if (!_core->isDocumentOpened () || _core->isPageRendering ())
		return;

	session.setFile (_core->getDocumentPaht ());
	session.setPage (_core->getCurrentPage().getNumber ());
	settings.saveSession (&session);
}

void
NSRReaderBB10::onIndicatorRequested (bool enabled)
{
	_indicator->setVisible (enabled);

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

	_prompt->setTitle (trUtf8 ("Enter password"));
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

	if (pageNum != _core->getCurrentPage().getNumber ()) {
		disableVisualControls ();
		_core->loadPage (NSRReaderCore::PAGE_LOAD_CUSTOM, pageNum);
	}
}

void
NSRReaderBB10::onErrorWhileOpening (NSRAbstractDocument::DocumentError error)
{
	QString errorStr;

	if (error == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		errorStr = trUtf8 ("Seems that entered password is wrong.");
	else
		errorStr = trUtf8 ("Unknown error! Maybe file is broken.");

	SystemToast *toast = new SystemToast (this);
	toast->setBody (errorStr);
	toast->show ();

	_pageView->resetPage ();
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

	if (_page->actionBarVisibility () == ChromeVisibility::Hidden)
		_page->setActionBarVisibility (ChromeVisibility::Visible);
	else
		_page->setActionBarVisibility (ChromeVisibility::Hidden);
}
