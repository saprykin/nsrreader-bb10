#include "nsrreaderbb10.h"
#include "nsrimageview.h"
#include "nsrreadercore.h"

#include <bb/cascades/Application>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Page>
#include <bb/cascades/Color>

using namespace bb::cascades;
using namespace bb::cascades::pickers;

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app),
	_core (NULL),
	_imageView (NULL),
	_filePicker (NULL),
	_prevPageAction (NULL),
	_nextPageAction (NULL),
	_indicator (NULL)
{
	Container *rootContainer = new Container ();
	rootContainer->setLayout (DockLayout::create ());
	rootContainer->setHorizontalAlignment (HorizontalAlignment::Fill);
	rootContainer->setVerticalAlignment (VerticalAlignment::Fill);

	_imageView = new NSRImageView ();
	_imageView->setHorizontalAlignment (HorizontalAlignment::Center);
	_imageView->setVerticalAlignment (VerticalAlignment::Center);
	_indicator = ActivityIndicator::create().horizontal(HorizontalAlignment::Fill)
						.vertical(VerticalAlignment::Fill);

	rootContainer->add (_imageView);
	rootContainer->add (_indicator);
	rootContainer->setBackground (Color::Black);

	Page *page = new Page ();
	page->setContent (rootContainer);

	ActionItem *openItem = ActionItem::create().title ("Open");
	_prevPageAction = ActionItem::create().title ("Previous");
	_nextPageAction = ActionItem::create().title ("Next");
	page->addAction (openItem, ActionBarPlacement::OnBar);
	page->addAction (_prevPageAction, ActionBarPlacement::OnBar);
	page->addAction (_nextPageAction, ActionBarPlacement::OnBar);

	openItem->setImageSource (QUrl ("asset:///open.png"));
	_prevPageAction->setImageSource (QUrl ("asset:///previous.png"));
	_nextPageAction->setImageSource (QUrl ("asset:///next.png"));

	connect (openItem, SIGNAL (triggered ()),
		 this, SLOT (onOpenActionTriggered ()));
	connect (_prevPageAction, SIGNAL (triggered ()),
		 this, SLOT (onPrevPageActionTriggered ()));
	connect (_nextPageAction, SIGNAL (triggered ()),
		 this, SLOT (onNextPageActionTriggered ()));

	_filePicker = new FilePicker (this);
	_filePicker->setTitle ("Select file");
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);

	_core = new NSRReaderCore (this);

	connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		 this, SLOT (onFileSelected (const QStringList&)));
	connect (_core, SIGNAL (pageRendered (int)), this, SLOT (onPageRendered (int)));
	connect (_core, SIGNAL (needIndicator (bool)), this, SLOT (setIndicatorEnabled (bool)));

	Application::instance()->setScene (page);

	updateVisualControls ();
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	_core->openDocument (files.first ());
	updateVisualControls ();
}

void
NSRReaderBB10::onOpenActionTriggered ()
{
	_filePicker->open ();
}

void
NSRReaderBB10::onPrevPageActionTriggered ()
{
	_core->loadPage (NSRReaderCore::PAGE_LOAD_PREV);
	updateVisualControls ();
}

void
NSRReaderBB10::onNextPageActionTriggered ()
{
	_core->loadPage (NSRReaderCore::PAGE_LOAD_NEXT);
	updateVisualControls ();
}

void
NSRReaderBB10::onPageRendered (int number)
{
	Q_UNUSED (number)

	_imageView->setImage (_core->getCurrentPage().getImage ());
}

void
NSRReaderBB10::updateVisualControls ()
{
	if (!_core->isDocumentOpened ()) {
		_prevPageAction->setEnabled (false);
		_nextPageAction->setEnabled (false);
	} else {
		int totalPages = _core->getPagesCount ();
		int currentPage = _core->getCurrentPage().getNumber ();

		_prevPageAction->setEnabled (totalPages != 1 && currentPage > 1);
		_nextPageAction->setEnabled (totalPages != 1 && currentPage != totalPages);
	}
}

void
NSRReaderBB10::setIndicatorEnabled (bool enabled)
{
	if (enabled)
		_indicator->start ();
	else
		_indicator->stop ();
}
