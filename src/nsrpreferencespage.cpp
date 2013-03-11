#include "nsrpreferencespage.h"
#include "nsrsettings.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent),
	_isSaveLastPos (NULL),
	_isFullscreen (NULL),
	_isTextMode (NULL),
	_isInvertedColors (NULL),
	_encodingsList (NULL)
{
	NSRSettings	settings;
	QString		defEncoding = trUtf8 ("UTF-8");

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());
	rootContainer->setLeftPadding (20);
	rootContainer->setRightPadding (20);
	rootContainer->setTopPadding (20);
	rootContainer->setBottomPadding (20);

	_isSaveLastPos = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isFullscreen = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isTextMode = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isInvertedColors = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_encodingsList = DropDown::create().horizontal(HorizontalAlignment::Fill)
					   .title(trUtf8 ("Text encoding"));

	_isSaveLastPos->setChecked (settings.isLoadLastDoc ());
	_isFullscreen->setChecked (settings.isFullscreenMode ());
	_isTextMode->setChecked (settings.isWordWrap ());
	_isInvertedColors->setChecked (settings.isInvertedColors ());

	QString textEncoding = settings.getTextEncoding ();
	QStringList encodings = settings.getSupportedEncodings ();
	int encodingIndex = encodings.indexOf (textEncoding, 0);

	if (encodingIndex == -1) {
		textEncoding = defEncoding;
		encodingIndex = encodings.indexOf (textEncoding, 0);
	}

	int count = encodings.count ();
	for (int i = 0; i < count; ++i)
		_encodingsList->add (Option::create().text(encodings.at (i)));
	_encodingsList->setSelectedIndex (encodingIndex);

	/* 'Save last position' option */
	Container *firstContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	firstContainer->setTopPadding (20);
	firstContainer->setBottomPadding (20);

	firstContainer->add (Label::create(trUtf8 ("Save last position")).horizontal(HorizontalAlignment::Left));
	firstContainer->add (_isSaveLastPos);

	/* 'Fullscreen mode' option */
	Container *secondContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());

	secondContainer->setTopPadding (20);
	secondContainer->setBottomPadding (20);

	secondContainer->add (Label::create(trUtf8 ("Fullscreen mode")).horizontal(HorizontalAlignment::Left));
	secondContainer->add (_isFullscreen);

	/* 'Text mode' option */
	Container *thirdContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	thirdContainer->setTopPadding (20);
	thirdContainer->setBottomPadding (20);

	thirdContainer->add (Label::create(trUtf8 ("Column view")).horizontal(HorizontalAlignment::Left));
	thirdContainer->add (_isTextMode);

	/* 'Invert colors' option */
	Container *fourthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());

	fourthContainer->setTopPadding (20);
	fourthContainer->setBottomPadding (20);

	fourthContainer->add (Label::create(trUtf8 ("Invert colors")).horizontal(HorizontalAlignment::Left));
	fourthContainer->add (_isInvertedColors);

	/* 'Text encoding' drop down list */
	Container *fifthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	fifthContainer->setTopPadding (20);
	fifthContainer->setBottomPadding (20);

	fifthContainer->add (_encodingsList);

	/* Add all options to root layout */
	rootContainer->add (firstContainer);
	rootContainer->add (secondContainer);
	rootContainer->add (thirdContainer);
	rootContainer->add (fourthContainer);
	rootContainer->add (fifthContainer);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title (trUtf8 ("Preferences")));
}

NSRPreferencesPage::~NSRPreferencesPage ()
{
}

void
NSRPreferencesPage::saveSettings ()
{
	NSRSettings settings;

	settings.saveLoadLastDoc (_isSaveLastPos->isChecked ());
	settings.saveFullscreenMode (_isFullscreen->isChecked ());
	settings.saveWordWrap (_isTextMode->isChecked ());
	settings.saveInvertedColors (_isInvertedColors->isChecked ());

	if (_encodingsList->isSelectedOptionSet ())
		settings.saveTextEncoding (_encodingsList->selectedOption()->text ());
}
