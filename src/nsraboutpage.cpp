#include "nsraboutpage.h"
#include "nsrsettings.h"

#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/Button>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Option>
#include <bb/cascades/WebView>
#include <bb/cascades/WebSettings>
#include <bb/cascades/Color>
#include <bb/cascades/ActionItem>

#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>

using namespace bb::cascades;
using namespace bb::system;

NSRAboutPage::NSRAboutPage (NSRAboutSection section, QObject *parent) :
	Page (parent),
	_aboutContainer (NULL),
	_helpContainer (NULL),
	_scrollView (NULL)
{
	SegmentedControl *segmentedControl = SegmentedControl::create().horizontal(HorizontalAlignment::Center)
								       .vertical(VerticalAlignment::Top);
	segmentedControl->add (Option::create().text (trUtf8 ("About", "About a program")));
	segmentedControl->add (Option::create().text (trUtf8 ("Help", "Help section of a program")));
	segmentedControl->add (Option::create().text (trUtf8 ("Changes", "Changes in new version")));

	Q_ASSERT (connect (segmentedControl, SIGNAL (selectedIndexChanged (int)),
			   this, SLOT (onSelectedIndexChanged (int))));

	/* About section goes first */

	_aboutContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Fill)
					     .layout(StackLayout::create());

	ImageView *logoView = ImageView::create().horizontal(HorizontalAlignment::Center)
						 .vertical(VerticalAlignment::Center)
						 .imageSource(QUrl ("asset:///nsrlogo.png"));

	Label *versionInfo = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Fill)
					    .text(QString ("NSR Reader ").append (NSRSettings::getVersion ()));
	versionInfo->textStyle()->setFontSize (FontSize::Large);
	Label *versionPlatform = Label::create().horizontal(HorizontalAlignment::Center)
						.vertical(VerticalAlignment::Fill)
						.text(trUtf8("for BlackBerry%1 10").arg (QString::fromUtf8 ("\u00AE")));
	versionPlatform->textStyle()->setFontSize (FontSize::Small);

	Label *authorInfo = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Fill)
					   .text(QString ("© 2011-2013 Alexander Saprykin"));

	Label *contactsInfo = Label::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Fill)
					     .text(trUtf8 ("Contacts: <a href='mailto:nsr.reader@gmail.com'>"
							   "nsr.reader@gmail.com</a>"));
	contactsInfo->textStyle()->setFontSize (FontSize::Medium);
	contactsInfo->setMultiline (true);
	contactsInfo->setTextFormat (TextFormat::Html);
	contactsInfo->content()->setFlags (TextContentFlag::ActiveText);

	Container *contactsContainer = Container::create().horizontal(HorizontalAlignment::Center)
							  .vertical(VerticalAlignment::Fill)
							  .layout(DockLayout::create());
	contactsContainer->setTopPadding (20);
	contactsContainer->setLeftPadding (40);
	contactsContainer->setRightPadding (40);
	contactsContainer->setBottomPadding (20);
	contactsContainer->add (contactsInfo);

	Label *reviewLabel = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Fill);
	reviewLabel->setText (trUtf8 ("Please, leave a review if you liked this app."));
	reviewLabel->textStyle()->setFontSize (FontSize::Medium);

	Container *twitterContainer = Container::create().horizontal(HorizontalAlignment::Center)
							 .vertical(VerticalAlignment::Fill)
							 .layout(StackLayout::create()
							 .orientation(LayoutOrientation::LeftToRight));

	ImageView *twitterImage = ImageView::create().imageSource(QUrl ("asset:///twitter.png"));
	Label *twitterInfo = Label::create().horizontal(HorizontalAlignment::Left)
					    .vertical(VerticalAlignment::Center)
					    .text("<a href='http://www.twitter.com/NSRReader'>"
						  "Follow on Twitter</a>")
					    .format(TextFormat::Html);
	twitterInfo->textStyle()->setFontSize (FontSize::Medium);
	twitterInfo->content()->setFlags (TextContentFlag::ActiveText);

	twitterContainer->setLeftPadding (40);
	twitterContainer->setRightPadding (40);
	twitterContainer->setBottomPadding (30);
	twitterContainer->add (twitterImage);
	twitterContainer->add (twitterInfo);

	Container *fbContainer = Container::create().horizontal(HorizontalAlignment::Center)
						    .vertical(VerticalAlignment::Fill)
						    .layout(StackLayout::create()
						    .orientation(LayoutOrientation::LeftToRight));

	ImageView *fbImage = ImageView::create().imageSource(QUrl ("asset:///facebook.png"));
	Label *fbInfo = Label::create().horizontal(HorizontalAlignment::Left)
				       .vertical(VerticalAlignment::Center)
				       .text("<a href='http://www.facebook.com/pages/NSR-Reader/162440877184478'>"
					     "Visit on Facebook</a>")
				       .format(TextFormat::Html);
	fbInfo->textStyle()->setFontSize (FontSize::Medium);
	fbInfo->content()->setFlags (TextContentFlag::ActiveText);

	fbContainer->setLeftPadding (40);
	fbContainer->setRightPadding (40);
	fbContainer->add (fbImage);
	fbContainer->add (fbInfo);

	_aboutContainer->setTopPadding (40);
	_aboutContainer->setBottomPadding (40);

	_aboutContainer->add (logoView);
	_aboutContainer->add (versionInfo);
	_aboutContainer->add (versionPlatform);
	_aboutContainer->add (authorInfo);
	_aboutContainer->add (contactsContainer);
	_aboutContainer->add (twitterContainer);
	_aboutContainer->add (fbContainer);
	_aboutContainer->add (reviewLabel);

	/* Help section goes next */

	QVariantMap viewportMap;
	viewportMap["width"] = "device-width";
	viewportMap["initial-scale"] = 1.0;

	_helpContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(StackLayout::create ());

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
					    "<li><b>Space</b> to scroll down one screen</li>"
					    "<li><b>Shift + Space</b> to scroll up one screen</li>"
					    "<li><b>B</b> to scroll to bottom</li>"
					    "<li><b>T</b> to scroll to top</li>"
					    "</ul>"
			    	    	    "Use <b>Go to</b> dialog to jump to arbitrary page.");
	QString fullScrSet = trUtf8 ("<b>Fullscreen Mode</b> &ndash; Action Bar at the bottom of the screen will "
			    	     "be hidden (tap the screen somewhere to show or hide it again).");
	QString reflowSet = trUtf8 ("<b>Text Reflow</b> &ndash; this is a special mode for files which contain a lot of "
			    	    "text information. In this mode NSR Reader will display only text data and wrap lines "
			    	    "for comfortable reading.");
	QString invertSet = trUtf8 ("<b>Invert Colors</b> &ndash; use this feature during dark days or if you have OLED "
			    	    "display (like <b>Q10</b> does) to save power.");
	QString encodSet = trUtf8 ("<b>Text Encoding</b> &ndash; this is only applied for plain text files (.txt). "
			    	   "Use it for files with specific national encoding.");
	QString tip1 = trUtf8 ("NSR Reader always saves position for every file.");
	QString tip2 = trUtf8 ("Do you see orange bar above the bottom Action Bar? This is a progress reading indicator.");
	QString tip3 = trUtf8 ("Just tap the screen to see current page and overall page count at the top left corner.");
	QString tip4 = trUtf8 ("Use <i>Recent</i> page to get fast access to files you have been reading.");
	QString tip5 = trUtf8 ("Use pinch gesture for zooming.");
	QString tip6 = trUtf8 ("NSR Reader supports password protected PDF files (except for latest "
			       "Adobe&reg; Reader&reg; X encryption algorithm), so don't scary them!");
	QString tip7 = trUtf8 ("NSR Reader caches already rendered pages to increase performance.");
	QString tip8 = trUtf8 ("If you have any problems with the app, please contact me (see contacts on "
			       "<i>About</i> page). Any suggestions are highly welcomed, too.");
	QString tip9 = trUtf8 ("Please leave a review for NSR Reader if you liked it to help others find it in the store. Thank you!");

	QString htmlHelp = QString ("<html><head/><body style=\"font-family: arial, sans-serif; "
				    "font-size: 28pt; background: #0F0F0F; color: #E6E6E6;\">"
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
				    "<div><p>%19</p></div>"
				    "</body></html>");
	htmlHelp = htmlHelp.arg(welcomeTitle).arg(welcomeSection).arg(navTitle).arg(navigationSection);
	htmlHelp = htmlHelp.arg(settingsTitle).arg(fullScrSet).arg(reflowSet)
			   .arg(invertSet).arg(encodSet);
	htmlHelp = htmlHelp.arg(tipsTitle).arg(tip1).arg(tip2).arg(tip3).arg(tip4).arg(tip5)
			   .arg(tip6).arg(tip7).arg(tip8).arg(tip9);


	WebView *webHelp = WebView::create ();
	webHelp->settings()->setDevicePixelRatio (1.0);
	webHelp->settings()->setViewportArguments (viewportMap);
	webHelp->settings()->setBackground (Color::fromRGBA (0.059f, 0.059f, 0.059f));
	webHelp->setHtml (htmlHelp);

	_helpContainer->add (webHelp);
	_helpContainer->setVisible (false);

	/* And the version changes goes last */

	_changesContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					       .vertical(VerticalAlignment::Fill)
					       .layout(StackLayout::create ());

	WebView *webChanges = WebView::create ();
	webChanges->settings()->setDevicePixelRatio (1.0);
	webChanges->settings()->setViewportArguments (viewportMap);
	webChanges->settings()->setBackground (Color::fromRGBA (0.059f, 0.059f, 0.059f));
	webChanges->setUrl (QUrl ("local:///assets/whats-new.html"));

	_changesContainer->add (webChanges);
	_changesContainer->setVisible (false);


	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());
	Container *contentContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							 .vertical(VerticalAlignment::Fill)
							 .layout(StackLayout::create ())
							 .background(Color::fromRGBA (0.059f, 0.059f, 0.059f));

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
	Q_ASSERT (connect (reviewAction, SIGNAL (triggered ()),
			   this, SLOT (onReviewActionTriggered ())));
	addAction (reviewAction, ActionBarPlacement::OnBar);

	segmentedControl->setSelectedIndex ((int) section);
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
	InvokeManager		invokeManager;
	InvokeRequest		invokeRequest;
	InvokeTargetReply	*invokeReply;

	invokeRequest.setUri (QUrl ("appworld://content/27985686"));
	invokeRequest.setAction ("bb.action.OPEN");
	invokeRequest.setTarget ("sys.appworld");

	invokeReply = invokeManager.invoke (invokeRequest);

	if (invokeReply != NULL) {
		invokeReply->setParent (this);
		Q_ASSERT (connect (invokeReply, SIGNAL (finished ()),
				   invokeReply, SLOT (deleteLater ())));
	}
}



