#include "nsrlastdocspage.h"
#include "nsrlastdocitemfactory.h"
#include "nsrsettings.h"
#include "nsrthumbnailer.h"

#include <bb/cascades/Container>
#include <bb/cascades/ListView>
#include <bb/cascades/DockLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TitleBar>
#include <bb/cascades/QListDataModel>

using namespace bb::cascades;

NSRLastDocsPage::NSRLastDocsPage (QObject *parent) :
	Page (parent),
	_listView (NULL),
	_listLayout (NULL)
{
	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	_listView = ListView::create().horizontal(HorizontalAlignment::Fill)
				      .vertical(VerticalAlignment::Fill)
				      .listItemProvider(new NSRLastDocItemFactory ());

	_listLayout = GridListLayout::create();

	if (OrientationSupport::instance()->orientation () == UIOrientation::Portrait)
		_listLayout->setColumnCount (2);
	else
		_listLayout->setColumnCount (3);

	_listLayout->setHorizontalCellSpacing (10);
	_listLayout->setVerticalCellSpacing (10);
	_listLayout->setCellAspectRatio (0.8);
	_listView->setLayout (_listLayout);

	_emptyLabel = Label::create().horizontal(HorizontalAlignment::Center)
				     .vertical(VerticalAlignment::Center)
				     .text(trUtf8 ("No recent documents",
						   "List of recently used files is empty"))
				     .visible(false);

	rootContainer->add (_listView);
	rootContainer->add (_emptyLabel);
	rootContainer->setBackground(Color::Black);
	rootContainer->setTopPadding (20);
	rootContainer->setBottomPadding (20);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Recent Documents",
						      "Title for window with recently "
						      "opened documents")));

	loadData ();

	connect (OrientationSupport::instance (),
		 SIGNAL (orientationAboutToChange (bb::cascades::UIOrientation::Type)),
		 this,
		 SLOT (onOrientationAboutToChange (bb::cascades::UIOrientation::Type)));
	connect (_listView, SIGNAL (triggered (QVariantList)),
		 this, SLOT (onListItemTriggered (QVariantList)));
}

NSRLastDocsPage::~NSRLastDocsPage ()
{
}

void
NSRLastDocsPage::onOrientationAboutToChange (bb::cascades::UIOrientation::Type type)
{
	if (type == UIOrientation::Portrait)
		_listLayout->setColumnCount (2);
	else
		_listLayout->setColumnCount (3);
}

void
NSRLastDocsPage::onListItemTriggered (QVariantList indexPath)
{
	QVariantMap map = _listView->dataModel()->data(indexPath).toMap ();

	emit requestDocument (map["path"].toString ());
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
			map["path"] = docs.at (i);

			model->append (map);
		}
	}

	_listView->setDataModel (model);

	if (count == 0) {
		_listView->setVisible (false);
		_emptyLabel->setVisible (true);
	}
}


