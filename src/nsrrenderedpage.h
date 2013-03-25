#ifndef NSRRENDEREDPAGE_H_
#define NSRRENDEREDPAGE_H_

#include <QObject>
#include <QSize>

#include <bb/ImageData>

class NSRRenderedPage : public QObject
{
	Q_OBJECT
public:
	NSRRenderedPage (QObject *parent = 0);
	NSRRenderedPage (int number, QObject *parent = 0);
	NSRRenderedPage (const NSRRenderedPage& page);
	virtual ~NSRRenderedPage ();
	NSRRenderedPage& operator= (const NSRRenderedPage& page);

	int getNumber () const;
	int getZoom () const;
	QSize getSize () const;
	bb::ImageData getImage () const;
	QString getText () const;
	bool isValid () const;

	void setNumber (int number);
	void setZoom (int zoom);
	void setImage (bb::ImageData img);
	void setText (const QString &text);

private:
	bb::ImageData		_image;
	QString			_text;
	int			_number;
	int			_zoom;
};

#endif /* NSRRENDEREDPAGE_H_ */
