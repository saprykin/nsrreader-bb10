#ifndef NSRABOUTPAGE_H_
#define NSRABOUTPAGE_H_

#include <bb/cascades/Page>

#include <QObject>

class NSRAboutPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRAboutPage (QObject *parent = 0);
	virtual ~NSRAboutPage ();

Q_SIGNALS:
	void startGuideRequested ();

private Q_SLOTS:
	void onGuideButtonClicked ();
};

#endif /* NSRABOUTPAGE_H_ */
