#include "nsrpreferencespage.h"

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Label>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/DropDown>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRPreferencesPage::NSRPreferencesPage (QObject *parent) :
	Page (parent)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(StackLayout::create ());
	/* 'Save last position' option */
	Container *firstContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	firstContainer->setLeftPadding (20);
	firstContainer->setRightPadding (20);
	firstContainer->setTopPadding (20);
	firstContainer->setBottomPadding (20);

	firstContainer->add (Label::create(trUtf8 ("Save last position")).horizontal(HorizontalAlignment::Left));
	firstContainer->add (ToggleButton::create().horizontal (HorizontalAlignment::Right));

	/* 'Fullscreen mode' option */
	Container *secondContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());

	secondContainer->setLeftPadding (20);
	secondContainer->setRightPadding (20);
	secondContainer->setTopPadding (20);
	secondContainer->setBottomPadding (20);

	secondContainer->add (Label::create(trUtf8 ("Fullscreen mode")).horizontal(HorizontalAlignment::Left));
	secondContainer->add (ToggleButton::create().horizontal (HorizontalAlignment::Right));

	/* 'Text mode' option */
	Container *thirdContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	thirdContainer->setLeftPadding (20);
	thirdContainer->setRightPadding (20);
	thirdContainer->setTopPadding (20);
	thirdContainer->setBottomPadding (20);

	thirdContainer->add (Label::create(trUtf8 ("Column view")).horizontal(HorizontalAlignment::Left));
	thirdContainer->add (ToggleButton::create().horizontal (HorizontalAlignment::Right));

	/* 'Invert colors' option */
	Container *fourthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
							.layout(DockLayout::create());

	fourthContainer->setLeftPadding (20);
	fourthContainer->setRightPadding (20);
	fourthContainer->setTopPadding (20);
	fourthContainer->setBottomPadding (20);

	fourthContainer->add (Label::create(trUtf8 ("Invert colors")).horizontal(HorizontalAlignment::Left));
	fourthContainer->add (ToggleButton::create().horizontal (HorizontalAlignment::Right));

	/* 'Text font' drop down list */
	Container *fifthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	fifthContainer->setLeftPadding (20);
	fifthContainer->setRightPadding (20);
	fifthContainer->setTopPadding (20);
	fifthContainer->setBottomPadding (20);

	fifthContainer->add (DropDown::create().horizontal(HorizontalAlignment::Fill)
					       .title(trUtf8 ("Text font"))
					       .add(trUtf8 ("None")));

	/* 'Text encoding' drop down list */
	Container *sixthContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .layout(DockLayout::create());

	sixthContainer->setLeftPadding (20);
	sixthContainer->setRightPadding (20);
	sixthContainer->setTopPadding (20);
	sixthContainer->setBottomPadding (20);

	sixthContainer->add (DropDown::create().horizontal(HorizontalAlignment::Fill)
					       .title(trUtf8 ("Text encoding"))
					       .add(trUtf8 ("None")));


	/* Add all options to root layout */
	rootContainer->add (firstContainer);
	rootContainer->add (secondContainer);
	rootContainer->add (thirdContainer);
	rootContainer->add (fourthContainer);
	rootContainer->add (fifthContainer);
	rootContainer->add (sixthContainer);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title (trUtf8 ("Preferences")));
}

NSRPreferencesPage::~NSRPreferencesPage ()
{
}

