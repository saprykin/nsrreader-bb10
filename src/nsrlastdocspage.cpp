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

	_listView = new NSRLastDocsListView ();
	_listView->setVerticalAlignment (VerticalAlignment::Fill);
	_listView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_listView->setListItemProvider (new NSRLastDocItemFactory ());

	_listLayout = GridListLayout::create ();

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
				     .text(trUtf8 ("No recent files",
						   "List of recently used files is empty"))
				     .visible(false);
	_emptyLabel->textStyle()->setFontSize (FontSize::Large);

	rootContainer->add (_listView);
	rootContainer->add (_emptyLabel);
	rootContainer->setBackground (Color::Black);

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Recent",
						      "Title for window with recently "
						      "opened files")));

	loadData ();

	Q_ASSERT (connect (OrientationSupport::instance (),
			   SIGNAL (orientationAboutToChange (bb::cascades::UIOrientation::Type)),
			   this,
			   SLOT (onOrientationAboutToChange (bb::cascades::UIOrientation::Type))));
	Q_ASSERT (connect (_listView, SIGNAL (triggered (QVariantList)),
			   this, SLOT (onListItemTriggered (QVariantList))));
	Q_ASSERT (connect (_listView, SIGNAL (modelUpdated (bool)), this, SLOT (onModelUpdated (bool))));
	Q_ASSERT (connect (_listView, SIGNAL (documentToBeDeleted (QString)),
			   this, SIGNAL (documentToBeDeleted (QString))));
}

NSRLastDocsPage::~NSRLastDocsPage ()
{
}

void
NSRLastDocsPage::finishToast ()
{
	_listView->finishToast ();
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
NSRLastDocsPage::onModelUpdated (bool isEmpty)
{
	_listView->setVisible (!isEmpty);
	_emptyLabel->setVisible (isEmpty);
}

void
NSRLastDocsPage::loadData ()
{
	QVariantListDataModel	*model = new QVariantListDataModel ();
	QStringList		docs = NSRSettings().getLastDocuments ();
	int			count = docs.count ();

	for (int i = 0; i < count; ++i) {
		if (QFile::exists (docs.at (i))) {
			QVariantMap	map;
			bool 		isEncrypted = NSRThumbnailer::isThumbnailEncrypted (docs.at (i));

			map["title"] = QFileInfo(docs.at (i)).fileName ();
			map["path"] = docs.at (i);
			map["encrypted"] = isEncrypted;

			if (!isEncrypted) {
				map["image"] = NSRThumbnailer::getThumnailPath (docs.at (i));
				map["text"] = NSRThumbnailer::getThumbnailText (docs.at (i));
			}

			model->append (map);
		}
	}

	_listView->setDataModel (model);

	if (model->size () == 0)
		onModelUpdated (true);
}


