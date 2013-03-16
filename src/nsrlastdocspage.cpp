#include "nsrlastdocspage.h"
#include "nsrlastdocitemfactory.h"
#include "nsrsettings.h"
#include "nsrthumbnailer.h"

#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/GridListLayout>
#include <bb/cascades/TitleBar>

using namespace bb::cascades;

NSRLastDocsPage::NSRLastDocsPage (QObject *parent) :
	Page (parent),
	_listView (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	_listView = ListView::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Fill)
				      .listItemProvider(new NSRLastDocItemFactory ());

	GridListLayout *grid = GridListLayout::create().columnCount(2);
	grid->setHorizontalCellSpacing (10);
	grid->setVerticalCellSpacing (10);
	grid->setCellAspectRatio (0.8);
	_listView->setLayout (grid);

	rootContainer->add (_listView);
	rootContainer->setBackground(Color::Black);
	rootContainer->setTopPadding (20);
	rootContainer->setBottomPadding (20);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Recent Documents")));

	loadData ();
}

NSRLastDocsPage::~NSRLastDocsPage ()
{
}

void
NSRLastDocsPage::loadData ()
{
	QVariantListDataModel	*model = new QVariantListDataModel ();
	QStringList		docs = NSRSettings().getLastDocuments ();
	int			count = docs.count ();

	for (int i = 0; i < count; ++i) {
		if (QFile::exists (docs.at (i))) {
			QVariantMap map;
			map["title"] = QFileInfo(docs.at (i)).fileName ();
			map["image"] = NSRThumbnailer::getThumnailPath (docs.at (i));
			map["text"] = NSRThumbnailer::getThumbnailText (docs.at (i));

			model->append (map);
		}
	}

	_listView->setDataModel (model);
}


