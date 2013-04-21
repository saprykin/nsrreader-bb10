#ifndef NSRRENDEREDPAGE_H_
#define NSRRENDEREDPAGE_H_

#include <QObject>
#include <QSize>

#include <bb/ImageData>

class NSRRenderedPage : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRRenderReason)
public:
	enum NSRRenderReason {
		NSR_RENDER_REASON_NONE		= 0,
		NSR_RENDER_REASON_NAVIGATION	= 1,
		NSR_RENDER_REASON_SETTINGS	= 2,
		NSR_RENDER_REASON_ZOOM		= 3,
		NSR_RENDER_REASON_ZOOM_TO_WIDTH	= 4
	};

	NSRRenderedPage (QObject *parent = 0);
	NSRRenderedPage (int number, QObject *parent = 0);
	NSRRenderedPage (const NSRRenderedPage& page);
	virtual ~NSRRenderedPage ();
	NSRRenderedPage& operator= (const NSRRenderedPage& page);

	NSRRenderedPage::NSRRenderReason getRenderReason () const;
	int getNumber () const;
	double getZoom () const;
	QSize getSize () const;
	bb::ImageData getImage () const;
	QString getText () const;
	bool isValid () const;

	void setRenderReason (NSRRenderedPage::NSRRenderReason reason);
	void setNumber (int number);
	void setZoom (double zoom);
	void setImage (bb::ImageData img);
	void setText (const QString &text);

private:
	NSRRenderReason		_reason;
	bb::ImageData		_image;
	QString			_text;
	double			_zoom;
	int			_number;
};

#endif /* NSRRENDEREDPAGE_H_ */
