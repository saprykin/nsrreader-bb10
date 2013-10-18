#ifndef NSRPREFERENCESPAGE_H_
#define NSRPREFERENCESPAGE_H_

#include "nsrtranslator.h"

#include <bb/cascades/Page>
#include <bb/cascades/ToggleButton>
#include <bb/cascades/DropDown>

class NSRPreferencesPage: public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRPreferencesPage (QObject *parent = 0);
	virtual ~NSRPreferencesPage ();

	void saveSettings ();

Q_SIGNALS:
	void switchFullscreen (bool isFullscreen);

private Q_SLOTS:
	void retranslateUi ();

private:
	NSRTranslator			*_translator;
	bb::cascades::ToggleButton	*_isFullscreen;
	bb::cascades::ToggleButton	*_isTextMode;
	bb::cascades::ToggleButton	*_isInvertedColors;
	bb::cascades::ToggleButton	*_isAutoCrop;
	bb::cascades::DropDown		*_encodingsList;
};

#endif /* NSRPREFERENCESPAGE_H_ */
