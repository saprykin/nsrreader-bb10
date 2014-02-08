#include "nsraboutpage.h"
#include "nsrsettings.h"
#include "nsrglobalnotifier.h"
#include "nsrfilesharer.h"
#include "nsrreader.h"
#include "nsrreaderbb10.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Button>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Option>
#include <bb/cascades/WebSettings>
#include <bb/cascades/Color>
#include <bb/cascades/ActionItem>

using namespace bb::cascades;

NSRAboutPage::NSRAboutPage (NSRAboutSection section, QObject *parent) :
	Page (parent),
	_aboutContainer (NULL),
	_helpContainer (NULL),
	_scrollView (NULL),
	_webHelp (NULL),
	_versionPlatform (NULL),
	_versionEngine (NULL)
#ifdef NSR_CORE_LITE_VERSION
	,_liteLabel (NULL)
#endif
{
	_translator = new NSRTranslator (this);

	SegmentedControl *segmentedControl = SegmentedControl::create().horizontal(HorizontalAlignment::Center)
								       .vertical(VerticalAlignment::Top);
	segmentedControl->add (Option::create().text (trUtf8 ("About", "About a program")));
	segmentedControl->add (Option::create().text (trUtf8 ("Help", "Help section of a program")));
	segmentedControl->add (Option::create().text (trUtf8 ("Changes", "Changes in new version")));

	bool ok = connect (segmentedControl, SIGNAL (selectedIndexChanged (int)), this, SLOT (onSelectedIndexChanged (int)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	/* About section goes first */

	_aboutContainer = Container::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Fill)
					     .layout(StackLayout::create());

	ImageView *logoView = ImageView::create().horizontal(HorizontalAlignment::Center)
						 .vertical(VerticalAlignment::Center);

#ifndef NSR_CORE_LITE_VERSION
	logoView->setImageSource (QUrl ("asset:///nsrlogo.png"));
#else
	logoView->setImageSource (QUrl ("asset:///nsrlogo-lite.png"));
#endif

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	logoView->accessibility()->setName (trUtf8 ("Logo of the app"));
#  endif
#endif

	Label *versionInfo = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Fill)
#ifdef NSR_CORE_LITE_VERSION
					    .text(QString("NSR Reader Lite ").append (NSRReaderBB10::getVersion ()));
#else
					    .text(QString("NSR Reader ").append (NSRReaderBB10::getVersion ()));
#endif

	versionInfo->textStyle()->setFontSize (FontSize::Large);

	_versionPlatform = Label::create().horizontal(HorizontalAlignment::Center)
					  .vertical(VerticalAlignment::Fill);
	_versionPlatform->textStyle()->setFontSize (FontSize::Small);

	_versionEngine = Label::create().horizontal(HorizontalAlignment::Center)
					.vertical(VerticalAlignment::Fill);
	_versionEngine->textStyle()->setFontSize (FontSize::Small);

	Label *authorInfo = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Fill)
					   .text(QString::fromUtf8 ("© 2011-2014 Alexander Saprykin"))
					   .multiline(true);
	authorInfo->textStyle()->setTextAlign (TextAlign::Center);

	Label *contactsInfo = Label::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Fill)
					     .text("<a href='mailto:nsr.reader@gmail.com'>"
						   "nsr.reader@gmail.com</a>");
	contactsInfo->textStyle()->setFontSize (FontSize::Medium);
	contactsInfo->setTextFormat (TextFormat::Html);
	contactsInfo->content()->setFlags (TextContentFlag::ActiveText);

	Container *contactsContainer = Container::create().horizontal(HorizontalAlignment::Center)
							  .vertical(VerticalAlignment::Fill)
							  .layout(StackLayout::create ());
	contactsContainer->setTopPadding (20);
	contactsContainer->setBottomPadding (20);
	contactsContainer->add (authorInfo);
	contactsContainer->add (contactsInfo);

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	contactsInfo->accessibility()->setName (trUtf8 ("nsr.reader@gmail.com - tap to write a email"));
#  endif
#endif

	Label *reviewLabel = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Fill)
					    .multiline(true);
	reviewLabel->setText (trUtf8 ("Please, leave a review if you like this app\n<b>Thank you!</b>"));
	reviewLabel->textStyle()->setFontSize (FontSize::Medium);
	reviewLabel->setTextFormat (TextFormat::Html);
	reviewLabel->textStyle()->setTextAlign (TextAlign::Center);

#ifdef NSR_CORE_LITE_VERSION
	_liteLabel = Label::create().horizontal(HorizontalAlignment::Center)
					  .vertical(VerticalAlignment::Fill)
					  .multiline(true);
	_liteLabel->textStyle()->setTextAlign(TextAlign::Center);
#endif

	_aboutContainer->setTopPadding (40);
	_aboutContainer->setBottomPadding (40);

	_aboutContainer->add (logoView);
	_aboutContainer->add (versionInfo);
	_aboutContainer->add (_versionPlatform);
	_aboutContainer->add (_versionEngine);
	_aboutContainer->add (contactsContainer);
	_aboutContainer->add (reviewLabel);

#ifdef NSR_CORE_LITE_VERSION
	_aboutContainer->add (_liteLabel);
#endif

	/* Help section goes next */

	QVariantMap viewportMap;
	viewportMap["width"] = "device-width";
	viewportMap["initial-scale"] = 1.0;

	_helpContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(StackLayout::create ());

	_webHelp = WebView::create ();
	_webHelp->settings()->setDevicePixelRatio (1.0);
	_webHelp->settings()->setViewportArguments (viewportMap);
	_webHelp->settings()->setBackground (Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f));

	_helpContainer->add (_webHelp);
	_helpContainer->setVisible (false);

	/* And the version changes goes last */

	_changesContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					       .vertical(VerticalAlignment::Fill)
					       .layout(StackLayout::create ());

	WebView *webChanges = WebView::create ();
	webChanges->settings()->setDevicePixelRatio (1.0);
	webChanges->settings()->setViewportArguments (viewportMap);
	webChanges->settings()->setBackground (Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f));
	webChanges->setUrl (QUrl ("local:///assets/whats-new.html"));

	_changesContainer->add (webChanges);
	_changesContainer->setVisible (false);


	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());
	Container *contentContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							 .vertical(VerticalAlignment::Fill)
							 .layout(StackLayout::create ())
							 .background(Color::fromRGBA (0.09f, 0.09f, 0.09f, 1.0f));

	rootContainer->add (_aboutContainer);
	rootContainer->add (_helpContainer);
	rootContainer->add (_changesContainer);

	_scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
					  .vertical(VerticalAlignment::Fill)
					  .content(rootContainer);
	contentContainer->add (segmentedControl);
	contentContainer->add (_scrollView);

	setContent (contentContainer);

	ActionItem *reviewAction = ActionItem::create().imageSource(QUrl ("asset:///review.png"))
						       .title(trUtf8 ("Review", "Review the app in the store"));
	ActionItem *twitterAction = ActionItem::create().imageSource(QUrl ("asset:///twitter.png"))
							.title("Twitter");
	ActionItem *facebookAction = ActionItem::create().imageSource(QUrl ("asset:///facebook.png"))
							 .title("Facebook");
#ifdef NSR_CORE_LITE_VERSION
	ActionItem *buyAction = ActionItem::create().imageSource(QUrl ("asset:///buy.png"))
						    .title(trUtf8 ("Buy"));
#endif


#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	reviewAction->accessibility()->setName (trUtf8 ("Review the app in the store"));
	twitterAction->accessibility()->setName (trUtf8 ("Visit Twitter page"));
	facebookAction->accessibility()->setName (trUtf8 ("Visit Facebook page"));
#    ifdef NSR_CORE_LITE_VERSION
	buyAction->accessibility()->setName (trUtf8 ("Buy full version of the app"));
#    endif
#  endif
#endif

	ok = connect (reviewAction, SIGNAL (triggered ()), this, SLOT (onReviewActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (twitterAction, SIGNAL (triggered ()), this, SLOT (onTwitterActionTriggered ()));
	Q_ASSERT (ok);

	ok = connect (facebookAction, SIGNAL (triggered ()), this, SLOT (onFacebookActionTriggered ()));
	Q_ASSERT (ok);

#ifdef NSR_CORE_LITE_VERSION
	ok = connect (buyAction, SIGNAL (triggered ()), this, SLOT (onBuyActionTriggered ()));
	Q_ASSERT (ok);
#endif

	addAction (reviewAction, ActionBarPlacement::OnBar);
#ifdef NSR_CORE_LITE_VERSION
	addAction (buyAction, ActionBarPlacement::OnBar);
#endif
	addAction (twitterAction, ActionBarPlacement::InOverflow);
	addAction (facebookAction, ActionBarPlacement::InOverflow);

	segmentedControl->setSelectedIndex ((int) section);

	retranslateUi ();

	_translator->addTranslatable ((UIObject *) segmentedControl->at (0),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRAboutPage"),
				      QString ("About"));
	_translator->addTranslatable ((UIObject *) segmentedControl->at (1),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRAboutPage"),
				      QString ("Help"));
	_translator->addTranslatable ((UIObject *) segmentedControl->at (2),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_OPTION,
				      QString ("NSRAboutPage"),
				      QString ("Changes"));
	_translator->addTranslatable ((UIObject *) reviewLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRAboutPage"),
				      QString ("Please, leave a review if you like this app\n<b>Thank you!</b>"));
	_translator->addTranslatable ((UIObject *) reviewAction,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
				      QString ("NSRAboutPage"),
				      QString ("Review"));
#ifdef NSR_CORE_LITE_VERSION
	_translator->addTranslatable ((UIObject *) buyAction,
			      	      NSRTranslator::NSR_TRANSLATOR_TYPE_ACTION,
			      	      QString ("NSRAboutPage"),
			      	      QString ("Buy"));
#endif

#ifdef BBNDK_VERSION_AT_LEAST
#  if BBNDK_VERSION_AT_LEAST(10,2,0)
	_translator->addTranslatable ((UIObject *) logoView->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("Logo of the app"));
	_translator->addTranslatable ((UIObject *) contactsInfo->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("nsr.reader@gmail.com - tap to write a email"));
	_translator->addTranslatable ((UIObject *) reviewAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("Review the app in the store"));
	_translator->addTranslatable ((UIObject *) twitterAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("Visit Twitter page"));
	_translator->addTranslatable ((UIObject *) facebookAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("Visit Facebook page"));
#    ifdef NSR_CORE_LITE_VERSION
	_translator->addTranslatable ((UIObject *) buyAction->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRAboutPage"),
				      QString ("Buy full version of the app"));
#    endif
#  endif
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);
}

NSRAboutPage::~NSRAboutPage ()
{
}

void
NSRAboutPage::onSelectedIndexChanged (int index)
{
	switch (index) {
	case 0:
		_aboutContainer->setVisible (true);
		_helpContainer->setVisible (false);
		_changesContainer->setVisible (false);
		_scrollView->scrollToPoint (0, 0, ScrollAnimation::None);
		break;
	case 1:
		_aboutContainer->setVisible (false);
		_helpContainer->setVisible (true);
		_changesContainer->setVisible (false);
		_scrollView->scrollToPoint (0, 0, ScrollAnimation::None);
		break;
	case 2:
		_aboutContainer->setVisible (false);
		_helpContainer->setVisible (false);
		_changesContainer->setVisible (true);
		_scrollView->scrollToPoint (0, 0, ScrollAnimation::None);
		break;
	default:
		break;
	}
}

void
NSRAboutPage::onReviewActionTriggered ()
{
#ifdef NSR_CORE_LITE_VERSION
	NSRFileSharer::getInstance()->invokeUri ("appworld://content/39670893", "sys.appworld", "bb.action.OPEN");
#else
	NSRFileSharer::getInstance()->invokeUri ("appworld://content/27985686", "sys.appworld", "bb.action.OPEN");
#endif
}

void
NSRAboutPage::onTwitterActionTriggered ()
{
	NSRFileSharer::getInstance()->invokeUri ("http://www.twitter.com/NSRReader", "sys.browser", "bb.action.OPEN");
}

void
NSRAboutPage::onFacebookActionTriggered ()
{
	NSRFileSharer::getInstance()->invokeUri ("http://www.facebook.com/pages/NSR-Reader/162440877184478", "sys.browser", "bb.action.OPEN");
}

#ifdef NSR_CORE_LITE_VERSION
void
NSRAboutPage::onBuyActionTriggered ()
{
	NSRFileSharer::getInstance()->invokeUri ("appworld://content/27985686", "sys.appworld", "bb.action.OPEN");
}
#endif

void
NSRAboutPage::retranslateUi ()
{
#ifdef NSR_CORE_LITE_VERSION
	_liteLabel->setText (trUtf8 ("You are using the Lite version which can read only first "
				     "%1 pages of the file. Please consider buying the full version "
				     "if you want to read larger files.").arg (NSRSettings::getMaxAllowedPages ()));
#endif
	_versionPlatform->setText (trUtf8("for BlackBerry%1 10", "%1 will be replaced with reg in circle symbol")
				   .arg (QString::fromUtf8 ("\u00AE")));
	_versionEngine->setText (trUtf8("Rendering engine %1").arg (NSRSettings::getVersion ()));

	QString welcomeTitle = trUtf8 ("Welcome!");
	QString navTitle = trUtf8 ("Navigation", "Navigation between document pages");
	QString settingsTitle = trUtf8 ("Settings", "Application settings");
	QString tipsTitle = trUtf8 ("Tips & Tricks");

	QString welcomeSection = trUtf8 ("NSR Reader is a handy tool for reading PDF, "
					 "DjVu, TIFF and TXT files. Hope you will be "
					 "enjoying using it! Please take a look at the following "
					 "instructions and tips to make reading more comfortable.");
	QString navigationSection = trUtf8 ("Use navigation arrows on Action Bar (at the bottom of the screen). "
			    	    	    "Double tap on the left area of the screen to go to previous page, on the right "
			    	    	    "area to go to next page and on the center to fit page to screen width. "
			    	    	    "Another way of navigation is to use <i>Volume Up/Down</i> hardware keys: "
			    	    	    "<i>Volume Up</i> key for the previous page and <i>Volume Down</i> key for "
			    	    	    "the next page. "
					    "<p></p>"
			    	    	    "If you have physical keyboard use the following keys (may vary with language): "
					    "<ul>"
					    "<li><b>P</b> key to navigate to previous page</li>"
			    	    	    "<li><b>N</b> key to navigate to next page</li>"
					    "<li><b>Ctrl + G</b> to go to arbitrary page</li>"
					    "<li><b>Ctrl + O</b> to open another file</li>"
					    "<li><b>Ctrl + B</b> to add or edit bookmark</li>"
					    "<li><b>Ctrl + T</b> to switch text reflow mode</li>"
					    "<li><b>Ctrl + I</b> to invert colors</li>"
					    "<li><b>Space</b> to scroll down one screen</li>"
					    "<li><b>Shift + Space</b> to scroll up one screen</li>"
					    "<li><b>B</b> to scroll to bottom</li>"
					    "<li><b>T</b> to scroll to top</li>"
					    "</ul>"
			    	    	    "Use <b>Go to</b> page slider to move to arbitrary page.");
	QString fullScrSet = trUtf8 ("<b>Fullscreen Mode</b> &ndash; Action Bar at the bottom of the screen will "
			    	     "be hidden (tap the screen somewhere to show or hide it again).");
	QString cropSet = trUtf8 ("<b>Crop Blank Edges</b> &ndash; automatically tries to detect and crop page's blank edges "
				  "to reduce side-to-side scrolling, especially when page is fitted to screen width.");
	QString screenLockSet = trUtf8 ("<b>Prevent Screen Locking</b> &ndash; disables screen autolocking feature "
					"while you are reading the file.");
	QString encodSet = trUtf8 ("<b>Text Encoding</b> &ndash; this is only applied for plain text files. "
			    	   "Use it for files with specific national encoding.");
	QString tip1 = trUtf8 ("NSR Reader always saves position for every file.");
	QString tip2 = trUtf8 ("Just tap the screen to see current page and overall page count at the top left corner.");
	QString tip3 = trUtf8 ("Use <i>Recent</i> tab to get fast access to files you have been reading.");
	QString tip4 = trUtf8 ("Use pinch gesture for zooming and to increase/decrease font size in text reflow mode.");
	QString tip5 = trUtf8 ("NSR Reader supports password protected PDF files (except for latest "
			       "Adobe&reg; Reader&reg; X encryption algorithm), so don't scary them!");
	QString tip6 = trUtf8 ("NSR Reader caches already rendered pages to increase performance.");
	QString tip7 = trUtf8 ("If you have any problems with the app, please contact me (see contacts on "
			       "<i>About</i> page). Any suggestions are highly welcomed, too.");
	QString tip8 = trUtf8 ("Please leave a review for NSR Reader if you like it to help others find it in the store. Thank you!");

	QString htmlHelp = QString ("<html><head/><body style=\"font-family: arial, sans-serif; "
				    "font-size: 28pt; background: #171717; color: #E6E6E6;\">"
			    	    "<div style=\"text-align: center;\"><div style=\"display: inline-block; "
				    "border-bottom: 1px solid white; font-size: 30pt;\">%1</div></div>"
				    "<div><p>%2</p></div>"
			    	    "<div style=\"text-align: center;\"><div style=\"display: inline-block; "
				    "border-bottom: 1px solid white; font-size: 30pt;\">%3</div></div>"
				    "<div><p>%4</p></div>"
				    "<div style=\"text-align: center;\"><div style=\"display: inline-block; "
				    "border-bottom: 1px solid white; font-size: 30pt;\">%5</div></div>"
				    "<div><p>%6</p></div>"
				    "<div><p>%7</p></div>"
				    "<div><p>%8</p></div>"
				    "<div><p>%9</p></div>"
				    "<div style=\"text-align: center;\"><div style=\"display: inline-block; "
				    "border-bottom: 1px solid white; font-size: 30pt;\">%10</div></div>"
				    "<div><p>%11</p></div>"
				    "<div><p>%12</p></div>"
				    "<div><p>%13</p></div>"
				    "<div><p>%14</p></div>"
				    "<div><p>%15</p></div>"
				    "<div><p>%16</p></div>"
				    "<div><p>%17</p></div>"
				    "<div><p>%18</p></div>"
				    "</body></html>");
	htmlHelp = htmlHelp.arg(welcomeTitle).arg(welcomeSection).arg(navTitle).arg(navigationSection);
	htmlHelp = htmlHelp.arg(settingsTitle).arg(fullScrSet).arg(cropSet).arg(screenLockSet).arg(encodSet);
	htmlHelp = htmlHelp.arg(tipsTitle).arg(tip1).arg(tip2).arg(tip3).arg(tip4).arg(tip5)
			   .arg(tip6).arg(tip7).arg(tip8);

	_webHelp->setHtml (htmlHelp);

	_translator->translate ();
}
