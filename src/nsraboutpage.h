#ifndef NSRABOUTPAGE_H_
#define NSRABOUTPAGE_H_

#include "nsrtranslator.h"

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ScrollView>
#include <bb/cascades/WebView>
#include <bb/cascades/Label>

#include <QObject>

class NSRAboutPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	enum NSRAboutSection {
		NSR_ABOUT_SECTION_MAIN		= 0,
		NSR_ABOUT_SECTION_HELP		= 1,
		NSR_ABOUT_SECTION_CHANGES	= 2
	};
	NSRAboutPage (NSRAboutSection section, QObject *parent = 0);
	virtual ~NSRAboutPage ();

private Q_SLOTS:
	void onSelectedIndexChanged (int index);
	void onReviewActionTriggered ();
	void onTwitterActionTriggered ();
	void onFacebookActionTriggered ();
#ifdef NSR_CORE_LITE_VERSION
	void onBuyActionTriggered ();
#endif
	void retranslateUi ();

private:
	NSRTranslator			*_translator;
	bb::cascades::Container		*_aboutContainer;
	bb::cascades::Container		*_helpContainer;
	bb::cascades::Container		*_changesContainer;
	bb::cascades::ScrollView	*_scrollView;
	bb::cascades::WebView		*_webHelp;
	bb::cascades::Label		*_versionPlatform;
#ifdef NSR_CORE_LITE_VERSION
	bb::cascades::Label		*_liteLabel;
#endif
};

#endif /* NSRABOUTPAGE_H_ */
