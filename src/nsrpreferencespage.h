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

private:
	NSRTranslator			*_translator;
	bb::cascades::ToggleButton	*_isFullscreen;
	bb::cascades::ToggleButton	*_isAutoCrop;
	bb::cascades::ToggleButton	*_isPreventScreenLock;
	bb::cascades::ToggleButton	*_isEncodingAutodetection;
#if BBNDK_VERSION_AT_LEAST(10,3,0)
	bb::cascades::ToggleButton	*_isBrandColors;
#endif
	bb::cascades::DropDown		*_encodingsList;
	bb::cascades::DropDown		*_themeList;
};

#endif /* NSRPREFERENCESPAGE_H_ */
