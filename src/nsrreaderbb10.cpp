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
#include <bb/cascades/ActionItem>
#include <bb/cascades/Color>

using namespace bb::cascades;
using namespace bb::cascades::pickers;

NSRReaderBB10::NSRReaderBB10 (bb::cascades::Application *app) :
	QObject (app)
{
	Container *rootContainer = new Container ();
	rootContainer->setLayout (DockLayout::create ());
	rootContainer->setHorizontalAlignment (HorizontalAlignment::Center);
	rootContainer->setVerticalAlignment (VerticalAlignment::Center);

	_imageView = new NSRImageView ();
	_imageView->setHorizontalAlignment (HorizontalAlignment::Center);
	_imageView->setVerticalAlignment (VerticalAlignment::Center);
	rootContainer->add (_imageView);
	rootContainer->setBackground (Color::Black);

	Page *page = new Page ();
	page->setContent (rootContainer);

	ActionItem *openItem = ActionItem::create().title ("Open");
	page->addAction (openItem, ActionBarPlacement::OnBar);

	connect (openItem, SIGNAL (triggered ()),
		 this, SLOT (onOpenActionTriggered ()));

	_filePicker = new FilePicker (this);
	_filePicker->setTitle ("Select file");
	_filePicker->setMode (FilePickerMode::Picker);
	_filePicker->setType (FileType::Other);

	_core = new NSRReaderCore (this);

	connect (_filePicker, SIGNAL (fileSelected (const QStringList&)),
		 this, SLOT (onFileSelected (const QStringList&)));

	Application::instance()->setScene (page);
}

void
NSRReaderBB10::onFileSelected (const QStringList &files)
{
	_core->openDocument (files.first ());
	_imageView->setImage (*_core->getCurrentPage());
}

void
NSRReaderBB10::onOpenActionTriggered ()
{
	_filePicker->open ();
}
