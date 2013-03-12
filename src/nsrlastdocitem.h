#ifndef NSRLASTDOCITEM_H_
#define NSRLASTDOCITEM_H_

#include <bb/cascades/CustomControl>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>

class NSRLastDocItem : public bb::cascades::CustomControl,
		       public bb::cascades::ListItemListener
{
	Q_OBJECT
public:
	NSRLastDocItem (bb::cascades::Container *parent = 0);
	virtual ~NSRLastDocItem ();

	void updateItem (const QString& title, const QString &imgPath);

	/* ListItemListener iface */
	void select (bool select);
	void reset (bool selected, bool activated);
	void activate (bool activate);

private:
	bb::cascades::ImageView	*_imageView;
	bb::cascades::Label	*_label;
};

#endif /* NSRLASTDOCITEM_H_ */
