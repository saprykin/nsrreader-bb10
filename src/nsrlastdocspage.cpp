#include "nsrlastdocspage.h"
#include "nsrlastdocitemfactory.h"

#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/GridListLayout>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRLastDocsPage::NSRLastDocsPage (QObject *parent) :
	Page (parent)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	ListView *list = ListView::create().horizontal(HorizontalAlignment::Fill)
					   .vertical(VerticalAlignment::Fill)
					   .listItemProvider(new NSRLastDocItemFactory ());

	GridListLayout *grid = GridListLayout::create().columnCount(2);
	grid->setHorizontalCellSpacing (10);
	grid->setVerticalCellSpacing (10);
	grid->setCellAspectRatio (0.8);
	list->setLayout (grid);

	rootContainer->add (list);
	rootContainer->setBackground(Color::Black);
	rootContainer->setTopPadding (20);
	rootContainer->setBottomPadding (20);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Recent documents")));
}

NSRLastDocsPage::~NSRLastDocsPage ()
{
}

