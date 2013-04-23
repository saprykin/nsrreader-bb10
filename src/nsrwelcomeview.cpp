#include "nsrwelcomeview.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/Label>

using namespace bb::cascades;

NSRWelcomeView::NSRWelcomeView (bb::cascades::Container *parent) :
	Container (parent),
	_openButton (NULL),
	_lastDocsButton (NULL)
{
	setHorizontalAlignment (HorizontalAlignment::Fill);
	setVerticalAlignment (VerticalAlignment::Fill);
	setLayout (DockLayout::create ());
	setBackground (Color::Black);

	Container *innerContainer = Container::create().horizontal(HorizontalAlignment::Center)
						       .vertical(VerticalAlignment::Center)
						       .layout(StackLayout::create ());

	_openButton = Button::create().text(trUtf8 ("Open", "Open document"))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center);
	_lastDocsButton = Button::create().text(trUtf8 ("Recent Documents", "Recently opened documents"))
					  .horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Center);

	connect (_openButton, SIGNAL (clicked ()), this, SIGNAL (openDocumentRequested ()));
	connect (_lastDocsButton, SIGNAL (clicked ()), this, SIGNAL (recentDocumentsRequested ()));

	innerContainer->add (_openButton);
	innerContainer->add (_lastDocsButton);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Center)
						      .vertical(VerticalAlignment::Center)
						      .layout(StackLayout::create ());
	Label *startLabel = Label::create().text(trUtf8 ("Start reading"))
					   .horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center);
	startLabel->textStyle()->setFontSize (FontSize::XLarge);
	startLabel->textStyle()->setFontStyle (FontStyle::Italic);
	startLabel->setBottomMargin (60);

	rootContainer->add (startLabel);
	rootContainer->add (innerContainer);

	add (rootContainer);
}

NSRWelcomeView::~NSRWelcomeView ()
{
}
