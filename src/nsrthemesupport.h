#ifndef NSRTHEMESUPPORT_H_
#define NSRTHEMESUPPORT_H_

#include <bb/cascades/VisualStyle>
#include <bb/cascades/Color>

class NSRThemeSupport
{
public:
	static NSRThemeSupport * instance ();
	void release ();

	bb::cascades::Color getBackground () const;
	bb::cascades::Color getImageBackground () const;
	bb::cascades::Color getText () const;
	bb::cascades::Color getTipText () const;
	bb::cascades::Color getOverlay () const;
	bb::cascades::VisualStyle::Type getVisualStyle () const;

private:
	NSRThemeSupport ();
	virtual ~NSRThemeSupport ();

	static NSRThemeSupport * 	_instance;
	bb::cascades::VisualStyle::Type	_visualStyle;
	bb::cascades::Color		_colorBackground;
	bb::cascades::Color		_colorImageBackground;
	bb::cascades::Color		_colorText;
	bb::cascades::Color		_colorTipText;
	bb::cascades::Color		_colorOverlay;
};

#endif /* NSRTHEMESUPPORT_H_ */
