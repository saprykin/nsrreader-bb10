#include "nsrwelcomeview.h"
#include "nsrglobalnotifier.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>

using namespace bb::cascades;

NSRWelcomeView::NSRWelcomeView (bb::cascades::Container *parent) :
	Container (parent),
	_translator (NULL),
	_openButton (NULL),
	_lastDocsButton (NULL),
	_startLabel (NULL)
{
	_translator = new NSRTranslator (this);

	setHorizontalAlignment (HorizontalAlignment::Fill);
	setVerticalAlignment (VerticalAlignment::Fill);
	setLayout (DockLayout::create ());
	setBackground (Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f));

	Container *innerContainer = Container::create().horizontal(HorizontalAlignment::Center)
						       .vertical(VerticalAlignment::Center)
						       .layout(StackLayout::create ());

	_openButton = Button::create().text(trUtf8 ("Open", "Open file"))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center);
	_lastDocsButton = Button::create().text(trUtf8 ("Recent", "Recently opened files"))
					  .horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Center);

	bool ok = connect (_openButton, SIGNAL (clicked ()), this, SIGNAL (openDocumentRequested ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_lastDocsButton, SIGNAL (clicked ()), this, SIGNAL (recentDocumentsRequested ()));
	Q_ASSERT (ok);

	innerContainer->add (_openButton);
	innerContainer->add (_lastDocsButton);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Center)
						      .vertical(VerticalAlignment::Center)
						      .layout(StackLayout::create ());
	_startLabel = Label::create().text(trUtf8 ("Start reading"))
				     .horizontal(HorizontalAlignment::Center)
				     .vertical(VerticalAlignment::Center);
	_startLabel->textStyle()->setFontSize (FontSize::XLarge);
	_startLabel->textStyle()->setColor (Color::White);
	_startLabel->setBottomMargin (60);

	rootContainer->add (_startLabel);
	rootContainer->add (innerContainer);

	add (rootContainer);

	_translator->addTranslatable ((UIObject *) _openButton, NSRTranslator::NSR_TRANSLATOR_TYPE_BUTTON,
				      QString ("NSRWelcomeView"),
				      QString ("Open"));
	_translator->addTranslatable ((UIObject *) _lastDocsButton, NSRTranslator::NSR_TRANSLATOR_TYPE_BUTTON,
				      QString ("NSRWelcomeView"),
				      QString ("Recent"));
	_translator->addTranslatable ((UIObject *) _startLabel, NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRWelcomeView"),
				      QString ("Start reading"));

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      _translator, SLOT (translate ()));
	Q_ASSERT (ok);
}

NSRWelcomeView::~NSRWelcomeView ()
{
}

void
NSRWelcomeView::setCardMode (bool enabled)
{
	_openButton->setVisible (!enabled);
	_lastDocsButton->setVisible (!enabled);
	_startLabel->setVisible (!enabled);
}
