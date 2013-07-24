#include "nsrpreferencespage.h"
#include "nsrsettings.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Divider>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent),
	_isFullscreen (NULL),
	_isTextMode (NULL),
	_isInvertedColors (NULL),
	_encodingsList (NULL)
{
	NSRSettings	settings;
	QString		defEncoding ("UTF-8");

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());

	_isFullscreen = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isTextMode = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isInvertedColors = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_encodingsList = DropDown::create().horizontal(HorizontalAlignment::Fill)
					   .title(trUtf8 ("Text Encoding", "Option in preferences, "
							   	   	   "selects text encoding"));

	_isFullscreen->setChecked (settings.isFullscreenMode ());
	_isTextMode->setChecked (settings.isWordWrap ());
	_isInvertedColors->setChecked (settings.isInvertedColors ());
	_encodingsList->setFocusPolicy (FocusPolicy::Touch);

	QString textEncoding = settings.getTextEncoding ();
	QStringList encodings = NSRSettings::getSupportedEncodings ();
	int encodingIndex = NSRSettings::mapEncodingToIndex (textEncoding);

	int count = encodings.count ();
	for (int i = 0; i < count; ++i)
		_encodingsList->add (Option::create().text(encodings.at (i)));
	_encodingsList->setSelectedIndex (encodingIndex);

	/* First container is out - was to save last positions */

	/* 'Fullscreen mode' option */
	Container *secondContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());

	secondContainer->setTopPadding (40);
	secondContainer->setBottomPadding (40);
	secondContainer->setLeftPadding (20);
	secondContainer->setRightPadding (20);

	secondContainer->add (Label::create(trUtf8 ("Fullscreen Mode",
						    "Option in preferences"))
				     .horizontal(HorizontalAlignment::Left)
				     .vertical(VerticalAlignment::Center));
	secondContainer->add (_isFullscreen);

	/* 'Text mode' option */
	Container *thirdContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());
	Container *outerThirdContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							    .layout(StackLayout::create());
	Label *columnInfo = Label::create (trUtf8 ("Use Text Reflow to read files with "
						   "large amount of text data."))
				   .horizontal(HorizontalAlignment::Fill)
				   .vertical(VerticalAlignment::Center);
	columnInfo->textStyle()->setFontSize (FontSize::XSmall);
	columnInfo->setMultiline (true);

	thirdContainer->add (Label::create(trUtf8 ("Text Reflow",
						   "Option in preferences, "
						   "reflows PDF in single text column"))
				    .horizontal(HorizontalAlignment::Left)
				    .vertical(VerticalAlignment::Center));
	thirdContainer->add (_isTextMode);

	outerThirdContainer->setTopPadding (40);
	outerThirdContainer->setBottomPadding (40);
	outerThirdContainer->setLeftPadding (20);
	outerThirdContainer->setRightPadding (20);

	outerThirdContainer->add (thirdContainer);
	outerThirdContainer->add (columnInfo);

	/* 'Invert colors' option */
	Container *fourthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());
	Container *outerFourthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							     .layout(StackLayout::create());
	Label *invertInfo = Label::create (trUtf8 ("Inverted colors can save power with "
						   "OLED display and help to read at dark."))
				   .horizontal(HorizontalAlignment::Fill)
				   .vertical(VerticalAlignment::Center);
	invertInfo->textStyle()->setFontSize (FontSize::XSmall);
	invertInfo->setMultiline (true);

	fourthContainer->add (Label::create(trUtf8 ("Invert Colors",
						    "Option in preferences"))
				     .horizontal(HorizontalAlignment::Left)
				     .vertical(VerticalAlignment::Center));
	fourthContainer->add (_isInvertedColors);

	outerFourthContainer->setTopPadding (40);
	outerFourthContainer->setBottomPadding (40);
	outerFourthContainer->setLeftPadding (20);
	outerFourthContainer->setRightPadding (20);

	outerFourthContainer->add (fourthContainer);
	outerFourthContainer->add (invertInfo);

	/* 'Text encoding' drop down list */
	Container *fifthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(StackLayout::create());
	Label *encodingInfo = Label::create(trUtf8 ("Text encoding is used only for pure text files. "
						    "None other format supports encoding selection."))
				     .horizontal(HorizontalAlignment::Fill)
				     .vertical(VerticalAlignment::Center);
	encodingInfo->textStyle()->setFontSize (FontSize::XSmall);
	encodingInfo->setMultiline (true);

	fifthContainer->setTopPadding (40);
	fifthContainer->setBottomPadding (40);
	fifthContainer->setLeftPadding (20);
	fifthContainer->setRightPadding (20);

	fifthContainer->add (_encodingsList);
	fifthContainer->add (encodingInfo);

	/* Add all options to root layout */
	rootContainer->add (secondContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (outerThirdContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (outerFourthContainer);
	rootContainer->add (Divider::create ());
	rootContainer->add (fifthContainer);

	ScrollView *scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
						     .vertical(VerticalAlignment::Fill)
						     .content(rootContainer);

	setContent (scrollView);
	setTitleBar (TitleBar::create().title (trUtf8 ("Settings")));

	Q_ASSERT (connect (_isFullscreen, SIGNAL (checkedChanged (bool)),
			   this, SIGNAL (switchFullscreen (bool))));
}

NSRPreferencesPage::~NSRPreferencesPage ()
{
}

void
NSRPreferencesPage::saveSettings ()
{
	NSRSettings settings;

	settings.saveFullscreenMode (_isFullscreen->isChecked ());
	settings.saveWordWrap (_isTextMode->isChecked ());
	settings.saveInvertedColors (_isInvertedColors->isChecked ());

	if (_encodingsList->isSelectedOptionSet ())
		settings.saveTextEncoding (NSRSettings::mapIndexToEncoding (_encodingsList->selectedIndex ()));
}
