#include "nsrthemesupport.h"
#include "nsrreader.h"

#include <bb/cascades/Application>
#include <bb/cascades/ThemeSupport>
#include <bb/cascades/Theme>
#include <bb/cascades/ColorTheme>

using namespace bb::cascades;

NSRThemeSupport * NSRThemeSupport::_instance = NULL;

NSRThemeSupport *
NSRThemeSupport::instance ()
{
	if (_instance == NULL)
		_instance = new NSRThemeSupport ();

	return _instance;
}

void
NSRThemeSupport::release ()
{
	if (_instance != NULL) {
		delete _instance;
		_instance = NULL;
	}
}

bb::cascades::Color
NSRThemeSupport::getBackground () const
{
	return _colorBackground;
}

bb::cascades::Color
NSRThemeSupport::getRecentItemBackground () const
{
	return _colorRecentItemBackground;
}

bb::cascades::Color
NSRThemeSupport::getImageBackground () const
{
	return _colorImageBackground;
}

bb::cascades::Color
NSRThemeSupport::getText () const
{
	return _colorText;
}

bb::cascades::Color
NSRThemeSupport::getRecentItemText () const
{
	return _colorRecentItemText;
}

bb::cascades::Color
NSRThemeSupport::getTipText () const
{
	return _colorTipText;
}

bb::cascades::Color
NSRThemeSupport::getOverlay () const
{
	return _colorOverlay;
}

bb::cascades::Color
NSRThemeSupport::getPrimaryBrand () const
{
	return _colorPrimaryBrand;
}

bb::cascades::VisualStyle::Type
NSRThemeSupport::getVisualStyle () const
{
	return _visualStyle;
}

void
NSRThemeSupport::setVisualStyle (bb::cascades::VisualStyle::Type visualStyle)
{
	_visualStyle = visualStyle;

	switch (_visualStyle) {
	case VisualStyle::Bright:
	{
		_colorBackground = Color::White;
		_colorImageBackground = Color::fromRGBA (0.85f, 0.85f, 0.85f);
		_colorRecentItemBackground = Color::fromRGBA (0.8f, 0.8f, 0.8f);
		_colorText = Color::Black;
		_colorRecentItemText = Color::Black;
		_colorTipText = Color::DarkGray;
#if BBNDK_VERSION_AT_LEAST(10,3,0)
		_colorOverlay = Color::fromRGBA (0.9f, 0.9f, 0.9f, 0.95f);
#else
		_colorOverlay = Color::fromRGBA (0.1f, 0.1f, 0.1f, 0.95f);
#endif

	}
	break;
	case VisualStyle::Dark:
	default:
	{
		_colorBackground = Color::fromRGBA (0.09f, 0.09f, 0.09f);
		_colorImageBackground = Color::fromRGBA (0.09f, 0.09f, 0.09f);
		_colorRecentItemBackground = Color::fromRGBA (0.2f, 0.2f, 0.2f);
		_colorText = Color::White;
		_colorRecentItemText = Color::Gray;
		_colorTipText = Color::Gray;
		_colorOverlay = Color::fromRGBA (0.1f, 0.1f, 0.1f, 0.95f);
	}
	}
}

QString
NSRThemeSupport::getAssetsThemeDirectory () const
{
	if (_visualStyle == VisualStyle::Bright)
		return QString ("bright");
	else
		return QString ("dark");
}

bb::cascades::Color
NSRThemeSupport::getReadingColor (NSRReadingTheme::Type type)
{
	switch (type) {
	case NSRReadingTheme::Normal:
		return Color::Black;
	case NSRReadingTheme::Sepia:
		return Color::fromRGBA (0.39f, 0.23f, 0.06f);
	default:
		return Color::Black;
	}
}

bb::cascades::Color
NSRThemeSupport::getReadingBackground (NSRReadingTheme::Type type)
{
	switch (type) {
	case NSRReadingTheme::Normal:
		return Color::White;
	case NSRReadingTheme::Sepia:
		return Color::fromRGBA (0.93f, 0.91f, 0.82f);
	default:
		return Color::White;
	}
}

NSRThemeSupport::NSRThemeSupport () :
	_visualStyle (VisualStyle::Dark),
	_colorBackground (Color::fromRGBA (0.09f, 0.09f, 0.09f)),
	_colorImageBackground (Color::fromRGBA (0.09f, 0.09f, 0.09f)),
	_colorRecentItemBackground (Color::fromRGBA (0.2f, 0.2f, 0.2f)),
	_colorText (Color::White),
	_colorRecentItemText (Color::Gray),
	_colorTipText (Color::Gray),
	_colorOverlay (Color::fromRGBA (0.1f, 0.1f, 0.1f, 0.95f)),
	_colorPrimaryBrand (Color::fromRGBA (0.0f, 0.66f, 0.87f))
{
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	_colorPrimaryBrand = Color::fromRGBA (1.0f, 0.5f, 0.0f);
#else
	_colorPrimaryBrand = Color::fromRGBA (0.0f, 0.66f, 0.87f);
#endif

	setVisualStyle (Application::instance()->themeSupport()->theme()->colorTheme()->style ());
}

NSRThemeSupport::~NSRThemeSupport ()
{
}
