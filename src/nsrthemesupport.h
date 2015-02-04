#ifndef NSRTHEMESUPPORT_H_
#define NSRTHEMESUPPORT_H_

#include "nsrreadingtheme.h"

#include <bb/cascades/VisualStyle>
#include <bb/cascades/Color>

class NSRThemeSupport
{
public:
	static NSRThemeSupport * instance ();
	void release ();

	bb::cascades::Color getBackground () const;
	bb::cascades::Color getImageBackground () const;
	bb::cascades::Color getRecentItemBackground () const;
	bb::cascades::Color getText () const;
	bb::cascades::Color getRecentItemText () const;
	bb::cascades::Color getTipText () const;
	bb::cascades::Color getOverlay () const;
	bb::cascades::Color getPrimaryBrand () const;
	bb::cascades::VisualStyle::Type getVisualStyle () const;
	void setVisualStyle (bb::cascades::VisualStyle::Type visualStyle);
	QString getAssetsThemeDirectory () const;

	static bb::cascades::Color getReadingColor (NSRReadingTheme::Type type);
	static bb::cascades::Color getReadingBackground (NSRReadingTheme::Type type);

private:
	NSRThemeSupport ();
	virtual ~NSRThemeSupport ();

	static NSRThemeSupport * 	_instance;
	bb::cascades::VisualStyle::Type	_visualStyle;
	bb::cascades::Color		_colorBackground;
	bb::cascades::Color		_colorImageBackground;
	bb::cascades::Color		_colorRecentItemBackground;
	bb::cascades::Color		_colorText;
	bb::cascades::Color		_colorRecentItemText;
	bb::cascades::Color		_colorTipText;
	bb::cascades::Color		_colorOverlay;
	bb::cascades::Color		_colorPrimaryBrand;
};

#endif /* NSRTHEMESUPPORT_H_ */
