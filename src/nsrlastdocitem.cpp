#include "nsrlastdocitem.h"
#include "nsrglobalnotifier.h"
#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/Application>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Color>
#include <bb/cascades/ImagePaint>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/ActionSet>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/Theme>
#include <bb/cascades/ColorTheme>

#include <QFile>

using namespace bb::cascades;

NSRLastDocItem::NSRLastDocItem (bb::cascades::Container* parent) :
	CustomControl (parent),
	_translator (NULL),
	_imageView (NULL),
	_textView (NULL),
	_label (NULL),
	_lockContainer (NULL),
	_viewContainer (NULL),
	_imgTracker (NULL),
	_solidContainer (NULL),
	_innerContainer (NULL),
	_selectAnimation (NULL),
	_selected (false)
{
	_translator = new NSRTranslator (this);

	memset (_solidSelect, 0, sizeof (_solidSelect));
	memset (_innerSelect, 0, sizeof (_innerSelect));

	Container *rootContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create ())
						      .background(NSRThemeSupport::instance()->getRecentItemBackground ());

	_viewContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Fill)
					    .layout(DockLayout::create())
					    .layoutProperties(StackLayoutProperties::create().spaceQuota(1.0));

	_imageView = ImageView::create().horizontal(HorizontalAlignment::Fill)
					.vertical(VerticalAlignment::Fill)
					.scalingMethod(ScalingMethod::AspectFill);
	_textView = Label::create().horizontal(HorizontalAlignment::Fill)
				   .vertical(VerticalAlignment::Fill)
				   .visible(false);
	_textView->textStyle()->setFontSize (FontSize::XXSmall);
	_textView->textStyle()->setColor (NSRThemeSupport::instance()->getRecentItemText ());
	_textView->setMultiline (true);

	_viewContainer->add (_imageView);
	_viewContainer->add (_textView);

	_label = Label::create ().horizontal(HorizontalAlignment::Center)
				 .vertical(VerticalAlignment::Center);
	_label->textStyle()->setColor (Color::White);
	_label->textStyle()->setFontSize (FontSize::XSmall);

	Container *labelContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						       .vertical(VerticalAlignment::Bottom)
						       .layout(DockLayout::create ())
						       .background(Color::Black);
	labelContainer->setOpacity (0.8);
	labelContainer->setTopMargin (0);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	labelContainer->setMinHeight (ui()->sdu (7));
	labelContainer->setLeftPadding (ui()->sdu (1.5f));
	labelContainer->setRightPadding (ui()->sdu (1.5f));
#else
	labelContainer->setMinHeight (70);
	labelContainer->setLeftPadding (15);
	labelContainer->setRightPadding (15);
#endif

	labelContainer->add (_label);

	_lockContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					    .vertical(VerticalAlignment::Center)
					    .background(Color::Transparent)
					    .layout(StackLayout::create ())
					    .visible(false);

	QString themeDir = NSRThemeSupport::instance()->getAssetsThemeDirectory ();

	ImageView *lockImage = ImageView::create().imageSource(QUrl (QString("asset:///%1/lock.png").arg (themeDir)))
						  .horizontal(HorizontalAlignment::Center)
						  .vertical(VerticalAlignment::Center);
	Label *lockLabel = Label::create ();
	lockLabel->textStyle()->setFontSize (FontSize::XSmall);

	_lockContainer->add (lockImage);
	_lockContainer->add (lockLabel);

	rootContainer->add (_viewContainer);
	rootContainer->add (labelContainer);
	rootContainer->add (_lockContainer);

	_imgTracker = new ImageTracker (this);
	bool ok = connect (_imgTracker, SIGNAL (stateChanged (bb::cascades::ResourceState::Type)),
			   this, SLOT (onImageStateChanged (bb::cascades::ResourceState::Type)));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	Color frameColor = Application::instance()->themeSupport()->theme()->colorTheme()->primary ();
#else
	Color frameColor = NSRThemeSupport::instance()->getPrimaryBrand ();
#endif

	Container *mainContainer = Container::create().horizontal(HorizontalAlignment::Fill)
						      .vertical(VerticalAlignment::Fill)
						      .layout(DockLayout::create())
						      .background(Color::Black);

	_solidContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Fill)
					     .layout(DockLayout::create())
					     .background(Color::Transparent);

	_solidSelect[0] = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Top)
					     .background(frameColor);
	_solidSelect[1] = Container::create().horizontal(HorizontalAlignment::Right)
					     .vertical(VerticalAlignment::Center)
					     .background(frameColor);
	_solidSelect[2] = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Bottom)
					     .background(frameColor);
	_solidSelect[3] = Container::create().horizontal(HorizontalAlignment::Left)
					     .vertical(VerticalAlignment::Center)
					     .background(frameColor);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_solidSelect[0]->setPreferredHeight (ui()->sdu (0.4f));
	_solidSelect[1]->setPreferredWidth (ui()->sdu (0.4f));
	_solidSelect[2]->setPreferredHeight (ui()->sdu (0.4f));
	_solidSelect[3]->setPreferredWidth (ui()->sdu (0.4f));
#else
	_solidSelect[0]->setPreferredHeight (4);
	_solidSelect[1]->setPreferredWidth (4);
	_solidSelect[2]->setPreferredHeight (4);
	_solidSelect[3]->setPreferredWidth (4);
#endif

	_solidContainer->add (_solidSelect[0]);
	_solidContainer->add (_solidSelect[1]);
	_solidContainer->add (_solidSelect[2]);
	_solidContainer->add (_solidSelect[3]);

	_solidContainer->setVisible (false);

	_innerContainer = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Fill)
					     .layout(DockLayout::create())
					     .background(Color::Transparent);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_innerContainer->setTopPadding (ui()->sdu (0.4f));
	_innerContainer->setRightPadding (ui()->sdu (0.4f));
	_innerContainer->setBottomPadding (ui()->sdu (0.4f));
	_innerContainer->setLeftPadding (ui()->sdu (0.4f));
#else
	_innerContainer->setTopPadding (4);
	_innerContainer->setRightPadding (4);
	_innerContainer->setBottomPadding (4);
	_innerContainer->setLeftPadding (4);
#endif

	_innerSelect[0] = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Top)
					     .background(frameColor);
	_innerSelect[1] = Container::create().horizontal(HorizontalAlignment::Right)
					     .vertical(VerticalAlignment::Center)
					     .background(frameColor);
	_innerSelect[2] = Container::create().horizontal(HorizontalAlignment::Fill)
					     .vertical(VerticalAlignment::Bottom)
					     .background(frameColor);
	_innerSelect[3] = Container::create().horizontal(HorizontalAlignment::Left)
					     .vertical(VerticalAlignment::Center)
					     .background(frameColor);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_innerSelect[0]->setPreferredHeight (ui()->sdu (0.8f));
	_innerSelect[1]->setPreferredWidth (ui()->sdu (0.8f));
	_innerSelect[2]->setPreferredHeight (ui()->sdu (0.8f));
	_innerSelect[3]->setPreferredWidth (ui()->sdu (0.8f));
#else
	_innerSelect[0]->setPreferredHeight (8);
	_innerSelect[1]->setPreferredWidth (8);
	_innerSelect[2]->setPreferredHeight (8);
	_innerSelect[3]->setPreferredWidth (8);
#endif

	_innerContainer->setOpacity (0.0);

	_innerContainer->add (_innerSelect[0]);
	_innerContainer->add (_innerSelect[1]);
	_innerContainer->add (_innerSelect[2]);
	_innerContainer->add (_innerSelect[3]);

	rootContainer->add (_solidContainer);
	rootContainer->add (_innerContainer);

	_selectAnimation = FadeTransition::create().duration(350).delay(30).to(0.3).target(_innerContainer);
	ok = connect (_selectAnimation, SIGNAL (stopped ()), this, SLOT (onAnimationStopped ()));
	Q_ASSERT (ok);

	LayoutUpdateHandler::create(this).onLayoutFrameChanged (this,
							       SLOT (onLayoutFrameChanged (QRectF)));

	mainContainer->add (rootContainer);
	setRoot (mainContainer);

#if BBNDK_VERSION_AT_LEAST(10,2,0)
	_imageView->accessibility()->addLabel (_label);
	_textView->accessibility()->addLabel (_label);
#endif

	ok = connect (NSRGlobalNotifier::instance (), SIGNAL (languageChanged ()),
		      this, SLOT (retranslateUi ()));
	Q_ASSERT (ok);
}

NSRLastDocItem::~NSRLastDocItem ()
{
}

void
NSRLastDocItem::updateItem (const QString&	title,
			    const QString&	imgPath,
			    const QString&	text,
			    const QString&	path,
			    bool		encrypted)
{
	_label->setText (title);
	_path = path;

	_lockContainer->setVisible (encrypted);

	if (encrypted) {
		_imageView->setVisible (false);
		_textView->setVisible (false);
		return;
	}

	if (QFile::exists (imgPath)) {
		_textView->setVisible (false);
		_imageView->setVisible (true);
		_imgTracker->setImageSource (QUrl::fromLocalFile (imgPath));
		_viewContainer->setLeftPadding (0);
		_viewContainer->setTopPadding (0);
		_viewContainer->setRightPadding (0);
		_viewContainer->setBottomPadding (0);
	} else {
		_imageView->setVisible (false);
		_textView->setVisible (true);
		_textView->setText (text);

#if BBNDK_VERSION_AT_LEAST(10,3,0)
		_viewContainer->setLeftPadding (ui()->sdu (1.2f));
		_viewContainer->setTopPadding (ui()->sdu (1.2f));
		_viewContainer->setRightPadding (ui()->sdu (1.2f));
		_viewContainer->setBottomPadding (ui()->sdu (1.2f));
#else
		_viewContainer->setLeftPadding (12);
		_viewContainer->setTopPadding (12);
		_viewContainer->setRightPadding (12);
		_viewContainer->setBottomPadding (12);
#endif
	}

	if (actionSetCount () > 0)
		actionSetAt(0)->setTitle (title);

	retranslateSubtitle ();
}

void
NSRLastDocItem::select (bool select)
{
	if (_selected == select)
		return;

	_selected = select;

	_solidContainer->setVisible (select);
	_innerContainer->setVisible (select);
}

void
NSRLastDocItem::reset (bool selected, bool activated)
{
	select (selected);
	activate (activated);
}

void
NSRLastDocItem::activate (bool activate)
{
	if (_selected)
		return;

	if (activate) {
		_innerContainer->setOpacity (0.0);
		_solidContainer->setOpacity (1.0);
		_selectAnimation->play ();
		_solidContainer->setVisible (true);
		_innerContainer->setVisible (true);
	} else {
		_selectAnimation->stop ();
		_solidContainer->setVisible (false);
		_innerContainer->setVisible (false);
		_innerContainer->setOpacity (1.0);
		_solidContainer->setOpacity (1.0);
	}
}

QString
NSRLastDocItem::getDocumentPath () const
{
	return _path;
}

QString
NSRLastDocItem::getDocumentTitle () const
{
	return _label->text ();
}

NSRTranslator *
NSRLastDocItem::getTranslator ()
{
	return _translator;
}

void
NSRLastDocItem::onImageStateChanged (bb::cascades::ResourceState::Type state)
{
	if (state == ResourceState::Loaded) {
		_imageView->setImage (_imgTracker->image ());

		if ((double) _imgTracker->height () / _imgTracker->width () < 1.2) {
			_imageView->setScalingMethod (ScalingMethod::AspectFit);
			_imageView->setVerticalAlignment (VerticalAlignment::Fill);
			_imageView->setHorizontalAlignment (HorizontalAlignment::Fill);
		} else {
			_imageView->setScalingMethod (ScalingMethod::AspectFill);
			_imageView->setVerticalAlignment (VerticalAlignment::Fill);
			_imageView->setHorizontalAlignment (HorizontalAlignment::Fill);
		}
	}
}

void
NSRLastDocItem::onLayoutFrameChanged (const QRectF& rect)
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_solidSelect[1]->setPreferredHeight (rect.height () - ui()->sdu (0.8f));
	_solidSelect[3]->setPreferredHeight (rect.height () - ui()->sdu (0.8f));
	_innerSelect[1]->setPreferredHeight (rect.height () - ui()->sdu (2.4f));
	_innerSelect[3]->setPreferredHeight (rect.height () - ui()->sdu (2.4f));
#else
	_solidSelect[1]->setPreferredHeight (rect.height () - 8);
	_solidSelect[3]->setPreferredHeight (rect.height () - 8);
	_innerSelect[1]->setPreferredHeight (rect.height () - 24);
	_innerSelect[3]->setPreferredHeight (rect.height () - 24);
#endif
}

void
NSRLastDocItem::onAnimationStopped ()
{
	_innerContainer->setOpacity (1.0);
	_solidContainer->setOpacity (1.0);
}

void
NSRLastDocItem::retranslateUi ()
{
	retranslateSubtitle ();

	_translator->translate ();
}

void
NSRLastDocItem::retranslateSubtitle ()
{
	if (actionSetCount () > 0)
		actionSetAt(0)->setSubtitle (NSRTranslator::translatePath (QFileInfo(_path).canonicalPath ()));
}
