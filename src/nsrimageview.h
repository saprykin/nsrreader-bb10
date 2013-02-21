#ifndef NSRIMAGEVIEW_H_
#define NSRIMAGEVIEW_H_

#include <QObject>

#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>
#include <bb/cascades/ImageView>
#include <bb/cascades/Image>

class NSRImageView: public bb::cascades::CustomControl
{
	Q_OBJECT

public:
	NSRImageView (bb::cascades::Container *parent = 0);
	virtual ~NSRImageView ();

	bb::cascades::Image getImage () const;
	void setImage (const bb::cascades::Image &img);
	void clearImage ();

private slots:
	void onWidthChanged (float width);
	void onHeightChanged (float height);

private:
	bb::cascades::Container		*_rootContainer;
	bb::cascades::ImageView		*_imageView;
};

#endif /* NSRIMAGEVIEW_H_ */
