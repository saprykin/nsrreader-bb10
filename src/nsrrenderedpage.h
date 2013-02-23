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
	QSize getSize () const;
	bb::ImageData getImage () const;
	bool isValid () const;

	void setNumber (int number);
	void setImage (bb::ImageData img);

private:
	bb::ImageData		_image;
	int			_number;
};

#endif /* NSRRENDEREDPAGE_H_ */
