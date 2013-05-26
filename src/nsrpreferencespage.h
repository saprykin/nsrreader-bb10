#ifndef NSRPREFERENCESPAGE_H_
#define NSRPREFERENCESPAGE_H_

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

private:
	bb::cascades::ToggleButton	*_isSaveLastPos;
	bb::cascades::ToggleButton	*_isFullscreen;
	bb::cascades::ToggleButton	*_isTextMode;
	bb::cascades::ToggleButton	*_isInvertedColors;
	bb::cascades::DropDown		*_encodingsList;
};

#endif /* NSRPREFERENCESPAGE_H_ */
