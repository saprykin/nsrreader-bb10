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

bb::cascades::VisualStyle::Type
NSRThemeSupport::getVisualStyle () const
{
	return _visualStyle;
}

NSRThemeSupport::NSRThemeSupport ()
{
	_visualStyle = Application::instance()->themeSupport()->theme()->colorTheme()->style ();

	switch (_visualStyle) {
	case VisualStyle::Bright:
	{
		_colorBackground = Color (Color::White);
		_colorImageBackground = Color::fromRGBA (0.85f, 0.85f, 0.85f);
		_colorRecentItemBackground = Color::fromRGBA (0.8f, 0.8f, 0.8f);
		_colorText = Color (Color::Black);
		_colorRecentItemText = Color (Color::Black);
		_colorTipText = Color (Color::DarkGray);
#if defined (BBNDK_VERSION_AT_LEAST) && BBNDK_VERSION_AT_LEAST(10,3,0)
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
		_colorText = Color (Color::White);
		_colorRecentItemText = Color (Color::Gray);
		_colorTipText = Color (Color::Gray);
		_colorOverlay = Color::fromRGBA (0.1f, 0.1f, 0.1f, 0.95f);
	}
	}
}

NSRThemeSupport::~NSRThemeSupport ()
{
}
