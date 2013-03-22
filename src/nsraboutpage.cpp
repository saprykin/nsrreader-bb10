#include "nsraboutpage.h"
#include "nsrsettings.h"

#include <bb/cascades/TitleBar>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/ScrollView>

using namespace bb::cascades;

NSRAboutPage::NSRAboutPage (QObject *parent) :
	Page (parent)
{
	setTitleBar (TitleBar::create().title(trUtf8 ("About", "About program")));

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create());

	ImageView *logoView = ImageView::create().horizontal(HorizontalAlignment::Center)
						 .vertical(VerticalAlignment::Center)
						 .imageSource(QUrl ("asset:///nsrlogo-welcome.png"));

	Label *versionInfo = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Fill)
					    .text(QString ("NSR Reader ").append (NSRSettings().getVersion ()));
	versionInfo->textStyle()->setFontSize (FontSize::Large);

	Label *authorInfo = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Fill)
					   .text(QString ("© 2013 Alexander Saprykin"));

	Container *firstLine = Container::create().horizontal(HorizontalAlignment::Fill)
						  .vertical(VerticalAlignment::Fill)
						  .background(Color::Gray);
	firstLine->setMaxHeight (1);
	firstLine->setPreferredHeight (1);

	Label *contactsInfo = Label::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Fill)
					     .text(trUtf8 ("If you have any questions or suggestions "
							   "(or even want to know what does NSR mean), "
							   "do not hesitate to write me: "
							   "<a href='mailto:nsr.reader@gmail.com'>nsr.reader@gmail.com</a>"));
	contactsInfo->textStyle()->setFontSize (FontSize::XSmall);
	contactsInfo->setMultiline (true);
	contactsInfo->setTextFormat (TextFormat::Html);
	contactsInfo->content()->setFlags (TextContentFlag::ActiveText);

	Container *contactsContainer = Container::create().horizontal(HorizontalAlignment::Center)
							  .vertical(VerticalAlignment::Fill)
							  .layout(DockLayout::create());
	contactsContainer->setTopPadding (20);
	contactsContainer->setLeftPadding (40);
	contactsContainer->setRightPadding (40);
	contactsContainer->setBottomPadding (35);
	contactsContainer->add (contactsInfo);

	Container *secondLine = Container::create().horizontal(HorizontalAlignment::Fill)
						   .vertical(VerticalAlignment::Fill)
						   .background(Color::Gray);
	secondLine->setMaxHeight (1);
	secondLine->setPreferredHeight (1);

	Container *twitterContainer = Container::create().horizontal(HorizontalAlignment::Center)
							 .vertical(VerticalAlignment::Fill)
							 .layout(StackLayout::create()
								 .orientation(LayoutOrientation::LeftToRight));
	ImageView *twitterImage = ImageView::create().imageSource(QUrl ("asset:///twitter.png"));
	Label *twitterInfo = Label::create().horizontal(HorizontalAlignment::Center)
					    .vertical(VerticalAlignment::Center)
					    .text("<a href='http://www.twitter.com/NSRReader'>www.twitter.com/NSRReader</a>")
					    .format(TextFormat::Html);
	twitterInfo->textStyle()->setFontSize (FontSize::XSmall);
	twitterInfo->content()->setFlags (TextContentFlag::ActiveText);

	twitterContainer->setLeftPadding (40);
	twitterContainer->setRightPadding (40);
	twitterContainer->add (twitterImage);
	twitterContainer->add (twitterInfo);

	Container *fbContainer = Container::create().horizontal(HorizontalAlignment::Center)
						    .vertical(VerticalAlignment::Fill)
						    .layout(StackLayout::create()
							    .orientation(LayoutOrientation::LeftToRight));
	ImageView *fbImage = ImageView::create().imageSource(QUrl ("asset:///facebook.png"));
	Label *fbInfo = Label::create().horizontal(HorizontalAlignment::Center)
				       .vertical(VerticalAlignment::Center)
				       .text("<a href='http://www.facebook.com/pages/NSR-Reader/162440877184478'>"
					     "www.facebook.com</a>")
				       .format(TextFormat::Html);
	fbInfo->textStyle()->setFontSize (FontSize::XSmall);
	fbInfo->content()->setFlags (TextContentFlag::ActiveText);

	fbContainer->setLeftPadding (40);
	fbContainer->setRightPadding (40);
	fbContainer->add (fbImage);
	fbContainer->add (fbInfo);

	rootContainer->setTopPadding (40);

	rootContainer->add (logoView);
	rootContainer->add (versionInfo);
	rootContainer->add (authorInfo);
	rootContainer->add (firstLine);
	rootContainer->add (contactsContainer);
	rootContainer->add (secondLine);
	rootContainer->add (twitterContainer);
	rootContainer->add (fbContainer);

	ScrollView *scrollView = ScrollView::create().horizontal(HorizontalAlignment::Fill)
						     .vertical(VerticalAlignment::Fill)
						     .content(rootContainer);
	setContent (scrollView);
}

NSRAboutPage::~NSRAboutPage ()
{
}

