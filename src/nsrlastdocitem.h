#ifndef NSRLASTDOCITEM_H_
#define NSRLASTDOCITEM_H_

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/ImageTracker>
#include <bb/cascades/ResourceState>

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
			 const QString& path);

	/* ListItemListener iface */
	void select (bool select);
	void reset (bool selected, bool activated);
	void activate (bool activate);
	QString getDocumentPath () const;
	QString getDocumentTitle () const;

private Q_SLOTS:
	void onImageStateChanged (bb::cascades::ResourceState::Type state);

private:
	bb::cascades::ImageView		*_imageView;
	bb::cascades::Label		*_textView;
	bb::cascades::Label		*_label;
	bb::cascades::Container		*_viewContainer;
	bb::cascades::ImageTracker	*_imgTracker;
	QString				_path;
};

#endif /* NSRLASTDOCITEM_H_ */
