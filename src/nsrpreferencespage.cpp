#include "nsrpreferencespage.h"
#include "nsrsettings.h"
#include "nsrglobalnotifier.h"
#include "nsrreader.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Label>
#include <bb/cascades/TitleBar>
#include <bb/cascades/ScrollView>
#include <bb/cascades/Divider>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
	_isFullscreen (NULL),
	_isAutoCrop (NULL),
	_isPreventScreenLock (NULL),
	_encodingsList (NULL)
{
	QString defEncoding ("UTF-8");

	_translator = new NSRTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());

	_isFullscreen = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isAutoCrop = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_isPreventScreenLock = ToggleButton::create().horizontal(HorizontalAlignment::Right);
	_encodingsList = DropDown::create().horizontal(HorizontalAlignment::Fill);

	_isFullscreen->setChecked (NSRSettings::instance()->isFullscreenMode ());
	_isAutoCrop->setChecked (NSRSettings::instance()->isAutoCrop ());
	_isPreventScreenLock->setChecked (NSRSettings::instance()->isPreventScreenLock ());
	_encodingsList->setFocusPolicy (FocusPolicy::Touch);

	QString textEncoding = NSRSettings::instance()->getTextEncoding ();
	QStringList encodings = NSRSettings::getSupportedEncodings ();
	int encodingIndex = NSRSettings::mapEncodingToIndex (textEncoding);

	int count = encodings.count ();
	for (int i = 0; i < count; ++i)
		_encodingsList->add (Option::create().text(encodings.at (i)));
	_encodingsList->setSelectedIndex (encodingIndex);

	/* First container is out - was to save last positions */

	/* 'Fullscreen Mode' option */
	Container *secondContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(StackLayout::create()
									    .orientation(LayoutOrientation::LeftToRight));

	secondContainer->setTopPadding (30);
	secondContainer->setLeftPadding (20);
	secondContainer->setRightPadding (20);

	Label *fullscreenLabel = Label::create(trUtf8 ("Fullscreen Mode", "Option in preferences"))
					.horizontal(HorizontalAlignment::Left)
					.vertical(VerticalAlignment::Center)
					.multiline(true)
					.layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	secondContainer->add (fullscreenLabel);
	secondContainer->add (_isFullscreen);

	/* 'Text Encoding' drop down list */
	Container *fifthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(StackLayout::create());
	Label *textEncodingLabel = Label::create(trUtf8 ("Text Encoding", "Option in preferences, "
						     	    	   	  "selects text encoding"))
				     	 .horizontal(HorizontalAlignment::Fill)
				     	 .vertical(VerticalAlignment::Center)
				     	 .multiline(true);
	Label *encodingInfo = Label::create(trUtf8 ("Text encoding is used only for plain text files, "
					       	    "none other format supports encoding selection."))
				     .horizontal(HorizontalAlignment::Fill)
				     .vertical(VerticalAlignment::Center);
	encodingInfo->textStyle()->setFontSize (FontSize::XSmall);
	encodingInfo->setMultiline (true);

	fifthContainer->setBottomPadding (20);
	fifthContainer->setLeftPadding (20);
	fifthContainer->setRightPadding (20);

	fifthContainer->add (textEncodingLabel);
	fifthContainer->add (_encodingsList);
	fifthContainer->add (encodingInfo);

	/* 'Crop Blank Edges' option */
	Container *sixthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(StackLayout::create()
									    .orientation(LayoutOrientation::LeftToRight));

	Label *cropLabel = Label::create(trUtf8 ("Crop Blank Edges", "Option in preferences"))
				 .horizontal(HorizontalAlignment::Left)
				 .vertical(VerticalAlignment::Center)
				 .multiline(true)
				 .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	sixthContainer->add (cropLabel);
	sixthContainer->add (_isAutoCrop);

	sixthContainer->setLeftPadding (20);
	sixthContainer->setRightPadding (20);

	/* 'Prevent Screen Locking' option */
	Container *firstInnerContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						   .layout(StackLayout::create()
								       .orientation(LayoutOrientation::LeftToRight));

	Label *preventScreenLockLabel = Label::create(trUtf8 ("Prevent Screen Locking", "Option in preferences"))
					      .horizontal(HorizontalAlignment::Left)
					      .vertical(VerticalAlignment::Center)
					      .multiline(true)
					      .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));

	Label *screenLockInfo = Label::create(trUtf8 ("Do not automatically lock screen while reading."))
				      .horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Center);
	screenLockInfo->textStyle()->setFontSize (FontSize::XSmall);
	screenLockInfo->setMultiline (true);

	Container *firstContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(StackLayout::create ());

	firstInnerContainer->add (preventScreenLockLabel);
	firstInnerContainer->add (_isPreventScreenLock);

	firstContainer->add (firstInnerContainer);
	firstContainer->add (screenLockInfo);

	firstContainer->setLeftPadding (20);
	firstContainer->setRightPadding (20);
	firstContainer->setBottomPadding (20);

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	_isFullscreen->accessibility()->addLabel (fullscreenLabel);
	_isAutoCrop->accessibility()->addLabel (cropLabel);
	_isPreventScreenLock->accessibility()->addLabel (preventScreenLockLabel);
#  endif
#endif

	/* Add all options to root layout */
	rootContainer->add (secondContainer);
	rootContainer->add (Divider::create().bottomMargin(30).topMargin(30));
	rootContainer->add (sixthContainer);
	rootContainer->add (Divider::create().bottomMargin(30).topMargin(30));
	rootContainer->add (firstContainer);
	rootContainer->add (Divider::create().bottomMargin(30).topMargin(30));
	rootContainer->add (fifthContainer);

	ScrollView *scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
						     .vertical(VerticalAlignment::Fill)
						     .content(rootContainer);

	setContent (scrollView);
	setTitleBar (TitleBar::create().title (trUtf8 ("Settings")));

	bool ok = connect (_isFullscreen, SIGNAL (checkedChanged (bool)),
			   this, SIGNAL (switchFullscreen (bool)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_isPreventScreenLock, SIGNAL (checkedChanged (bool)),
		      this, SIGNAL (switchPreventScreenLock (bool)));
	Q_ASSERT (ok);

	_translator->addTranslatable ((UIObject *) fullscreenLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Fullscreen Mode"));
	_translator->addTranslatable ((UIObject *) textEncodingLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Text Encoding"));
	_translator->addTranslatable ((UIObject *) encodingInfo,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Text encoding is used only for plain text files, "
					       "none other format supports encoding selection."));
	_translator->addTranslatable ((UIObject *) cropLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Crop Blank Edges"));
	_translator->addTranslatable ((UIObject *) preventScreenLockLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Prevent Screen Locking"));
	_translator->addTranslatable ((UIObject *) screenLockInfo,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRPreferencesPage"),
				      QString ("Do not automatically lock screen while reading."));
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRPreferencesPage"),
				      QString ("Settings"));

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		     this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);
}

NSRPreferencesPage::~NSRPreferencesPage ()
{
}

void
NSRPreferencesPage::saveSettings ()
{
	NSRSettings::instance()->saveFullscreenMode (_isFullscreen->isChecked ());
	NSRSettings::instance()->saveAutoCrop (_isAutoCrop->isChecked ());
	NSRSettings::instance()->savePreventScreenLock (_isPreventScreenLock->isChecked ());

	if (_encodingsList->isSelectedOptionSet ())
		NSRSettings::instance()->saveTextEncoding (NSRSettings::mapIndexToEncoding (_encodingsList->selectedIndex ()));
}

void
NSRPreferencesPage::retranslateUi ()
{
	QStringList encodings = NSRSettings::getSupportedEncodings ();

	int count = encodings.count ();
	int optCount = _encodingsList->count ();

	/* Sanity check */
	if (count == optCount) {
		for (int i = 0; i < count; ++i)
			_encodingsList->at(i)->setText (encodings.at (i));
	}

	_translator->translate ();
}
