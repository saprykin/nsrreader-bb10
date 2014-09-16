#include "nsrlastdocspage.h"
#include "nsrlastdocitemfactory.h"
#include "nsrsettings.h"
#include "nsrthumbnailer.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/ListView>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayout>
#include <bb/cascades/Color>
#include <bb/cascades/TitleBar>
#include <bb/cascades/QListDataModel>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>

#include <bb/device/DisplayInfo>

using namespace bb::cascades;
using namespace bb::device;

#define NSR_LAST_DOC_WIDTH	360

NSRLastDocsPage::NSRLastDocsPage (QObject *parent) :
	Page (parent),
	_translator (NULL),
	_listView (NULL),
	_listLayout (NULL),
	_emptyContainer (NULL)
{
	_translator = new NSRTranslator (this);

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ());

	_listView = new NSRLastDocsListView ();
	_listView->setVerticalAlignment (VerticalAlignment::Fill);
	_listView->setHorizontalAlignment (HorizontalAlignment::Fill);
	_listView->setListItemProvider (new NSRLastDocItemFactory ());

	_listLayout = GridListLayout::create ();

	QSize displaySize = DisplayInfo().pixelSize ();

	if (OrientationSupport::instance()->orientation () == UIOrientation::Portrait)
		_listLayout->setColumnCount (displaySize.width () / NSR_LAST_DOC_WIDTH);
	else
		_listLayout->setColumnCount (displaySize.height () / NSR_LAST_DOC_WIDTH);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	_listLayout->setHorizontalCellSpacing (ui()->sdu (1));
	_listLayout->setVerticalCellSpacing (ui()->sdu (1));
#else
	_listLayout->setHorizontalCellSpacing (10);
	_listLayout->setVerticalCellSpacing (10);
#endif

	_listLayout->setCellAspectRatio (0.8);
	_listView->setLayout (_listLayout);

	Label *emptyLabel = Label::create().horizontal(HorizontalAlignment::Center)
					   .vertical(VerticalAlignment::Center)
					   .multiline(true)
					   .text(trUtf8 ("No recent files", "List of recently used files is empty"));
	emptyLabel->textStyle()->setFontSize (FontSize::Large);
	emptyLabel->textStyle()->setTextAlign (TextAlign::Center);

	Label *emptyMoreLabel = Label::create().horizontal(HorizontalAlignment::Center)
					       .vertical(VerticalAlignment::Center)
					       .multiline(true)
					       .text(trUtf8 ("Start reading to display files here"));
	emptyMoreLabel->textStyle()->setFontSize (FontSize::Medium);
	emptyMoreLabel->textStyle()->setTextAlign (TextAlign::Center);

	ImageView *emptyImage = ImageView::create().horizontal(HorizontalAlignment::Center)
						   .vertical(VerticalAlignment::Center)
						   .imageSource(QUrl ("asset:///file.png"));

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
	emptyImage->accessibility()->setName (trUtf8 ("Image of document"));
#endif

	_emptyContainer = Container::create().horizontal(HorizontalAlignment::Center)
					     .vertical(VerticalAlignment::Center)
					     .layout(StackLayout::create ())
					     .visible(false);

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
	_emptyContainer->setLeftPadding (ui()->sdu (2));
	_emptyContainer->setRightPadding (ui()->sdu (2));
#else
	_emptyContainer->setLeftPadding (20);
	_emptyContainer->setRightPadding (20);
#endif

	_emptyContainer->add (emptyImage);
	_emptyContainer->add (emptyLabel);
	_emptyContainer->add (emptyMoreLabel);

	rootContainer->add (_listView);
	rootContainer->add (_emptyContainer);
	rootContainer->setBackground (NSRThemeSupport::instance()->getBackground ());

	setContent (rootContainer);
	setTitleBar (TitleBar::create().title(trUtf8 ("Recent",
						      "Title for window with recently "
						      "opened files")));

	loadData ();

	bool ok = connect (OrientationSupport::instance (),
			   SIGNAL (orientationAboutToChange (bb::cascades::UIOrientation::Type)),
			   this,
			   SLOT (onOrientationAboutToChange (bb::cascades::UIOrientation::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_listView, SIGNAL (triggered (QVariantList)),
		      this, SLOT (onListItemTriggered (QVariantList)));
	Q_ASSERT (ok);

	ok = connect (_listView, SIGNAL (modelUpdated (bool)), this, SLOT (onModelUpdated (bool)));
	Q_ASSERT (ok);

	ok = connect (_listView, SIGNAL (documentToBeDeleted (QString)),
			   this, SIGNAL (documentToBeDeleted (QString)));
	Q_ASSERT (ok);

	_translator->addTranslatable ((UIObject *) emptyLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRLastDocsPage"),
				      QString ("No recent files"));
	_translator->addTranslatable ((UIObject *) emptyMoreLabel,
				      NSRTranslator::NSR_TRANSLATOR_TYPE_LABEL,
				      QString ("NSRLastDocsPage"),
				      QString ("Start reading to display files here"));
	_translator->addTranslatable ((UIObject *) titleBar (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_TITLEBAR,
				      QString ("NSRLastDocsPage"),
				      QString ("Recent"));

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,2,0)
	_translator->addTranslatable ((UIObject *) emptyImage->accessibility (),
				      NSRTranslator::NSR_TRANSLATOR_TYPE_A11Y,
				      QString ("NSRLastDocsPage"),
				      QString ("Image of document"));
#endif

#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,1,0)
	_listView->setScrollRole (ScrollRole::Main);
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      _translator, SLOT (translate ()));
	Q_ASSERT (ok);
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
NSRLastDocsPage::onDocumentOpened (const QString& file)
{
	_lastOpenedFile = file;

	finishToast ();

	QVariantListDataModel *model = static_cast < QVariantListDataModel * > (_listView->dataModel ());

	bool itemFound = false;
	int count = model->size ();

	for (int i = 0; i < count; ++i) {
		if (model->value(i).toMap()["path"] == _lastOpenedFile) {
			model->move (i, 0);
			itemFound = true;
			break;
		}
	}

	if (!itemFound)
		model->insert (0, createModelItem (_lastOpenedFile));

	onModelUpdated (model->size () == 0);
}

void
NSRLastDocsPage::onThumbnailRendered ()
{
	finishToast ();

	QVariantListDataModel *model = static_cast < QVariantListDataModel * > (_listView->dataModel ());

	int count = model->size ();

	for (int i = 0; i < count; ++i) {
		if (model->value(i).toMap()["path"] == _lastOpenedFile) {
			model->removeAt (i);
			break;
		}
	}

	model->insert (0, createModelItem (_lastOpenedFile));

	onModelUpdated (model->size () == 0);
}

void
NSRLastDocsPage::onOrientationAboutToChange (bb::cascades::UIOrientation::Type type)
{
	QSize displaySize = DisplayInfo().pixelSize ();

	if (type == UIOrientation::Portrait)
		_listLayout->setColumnCount (displaySize.width () / NSR_LAST_DOC_WIDTH);
	else
		_listLayout->setColumnCount (displaySize.height () / NSR_LAST_DOC_WIDTH);
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
	_emptyContainer->setVisible (isEmpty);
}

void
NSRLastDocsPage::loadData ()
{
	QVariantListDataModel	*model = new QVariantListDataModel ();
	QStringList		docs = NSRSettings::instance()->getLastDocuments ();
	int			count = docs.count ();

	for (int i = 0; i < count; ++i)
		if (QFile::exists (docs.at (i)))
			model->append (createModelItem (docs.at (i)));

	_listView->setDataModel (model);

	if (model->size () == 0)
		onModelUpdated (true);
}

QVariant
NSRLastDocsPage::createModelItem (const QString& file)
{
	QVariantMap	map;
	bool 		isEncrypted = NSRThumbnailer::instance()->isThumbnailEncrypted (file);

	map["title"] = QFileInfo(file).fileName ();
	map["path"] = file;
	map["encrypted"] = isEncrypted;

	if (!isEncrypted) {
		map["image"] = NSRThumbnailer::instance()->getThumbnailPath (file);
		map["text"] = NSRThumbnailer::instance()->getThumbnailText (file);
	}

	return map;
}
