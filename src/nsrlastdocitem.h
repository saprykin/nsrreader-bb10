#ifndef NSRLASTDOCITEM_H_
#define NSRLASTDOCITEM_H_

#include "nsrtranslator.h"

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/ImageTracker>
#include <bb/cascades/ResourceState>
#include <bb/cascades/FadeTransition>

#include <QRectF>
#include <QObject>

class NSRLastDocItem : public bb::cascades::CustomControl,
		       public bb::cascades::ListItemListener
{
	Q_OBJECT
public:
	NSRLastDocItem (bb::cascades::Container *parent = 0);
	virtual ~NSRLastDocItem ();

	void updateItem (const QString& title,
			 const QString&	imgPath,
			 const QString&	text,
			 const QString& path,
			 bool		encrypted);

	/* ListItemListener iface */
	void select (bool select);
	void reset (bool selected, bool activated);
	void activate (bool activate);
	QString getDocumentPath () const;
	QString getDocumentTitle () const;
	NSRTranslator * getTranslator ();

private Q_SLOTS:
	void onImageStateChanged (bb::cascades::ResourceState::Type state);
	void onLayoutFrameChanged (const QRectF& rect);
	void onAnimationStopped ();
	void retranslateUi ();
	void onWantsHighlightChanged (bool wantsHighlight);

private:
	void retranslateSubtitle ();

	NSRTranslator			*_translator;
	bb::cascades::ImageView		*_imageView;
	bb::cascades::Label		*_textView;
	bb::cascades::Label		*_label;
	bb::cascades::Container		*_lockContainer;
	bb::cascades::Container		*_viewContainer;
	bb::cascades::ImageTracker	*_imgTracker;
	bb::cascades::Container		*_solidSelect[4];
	bb::cascades::Container		*_innerSelect[4];
	bb::cascades::Container		*_solidContainer;
	bb::cascades::Container		*_innerContainer;
	bb::cascades::FadeTransition	*_selectAnimation;
	QString				_path;
	bool				_selected;
};

#endif /* NSRLASTDOCITEM_H_ */
