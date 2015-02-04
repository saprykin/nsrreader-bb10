#ifndef NSRPREFERENCESPAGE_H_
#define NSRPREFERENCESPAGE_H_

#include "nsrtranslator.h"
#include "nsrreader.h"

#include <bb/cascades/Page>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/DropDown>

#include <QObject>

class NSRPreferencesPage: public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRPreferencesPage (QObject *parent = 0);
	virtual ~NSRPreferencesPage ();

	void saveSettings ();

Q_SIGNALS:
	void switchFullscreen (bool isFullscreen);
	void switchPreventScreenLock (bool isPreventScreenLock);

private Q_SLOTS:
	void retranslateUi ();
	void onEncodingAutodetectionCheckedChanged (bool checked);
	void onDynamicDUFactorChanged (float dduFactor);

private:
	NSRTranslator *			_translator;
	bb::cascades::Container *	_themeContainer;
	bb::cascades::Container *	_textThemeContainer;
	bb::cascades::Container *	_fullscreenContainer;
	bb::cascades::Container *	_cropContainer;
	bb::cascades::Container *	_screenLockContainer;
	bb::cascades::Container *	_encodingContainer;
	bb::cascades::ToggleButton *	_isFullscreen;
	bb::cascades::ToggleButton *	_isAutoCrop;
	bb::cascades::ToggleButton *	_isPreventScreenLock;
	bb::cascades::ToggleButton *	_isEncodingAutodetection;
	bb::cascades::ToggleButton *	_isBrandColors;
	bb::cascades::DropDown *	_encodingsList;
	bb::cascades::DropDown *	_themeList;
	bb::cascades::DropDown *	_textThemeList;
};

#endif /* NSRPREFERENCESPAGE_H_ */
