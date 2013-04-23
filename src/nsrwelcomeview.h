#ifndef NSRWELCOMEVIEW_H_
#define NSRWELCOMEVIEW_H_

#include <bb/cascades/Container>
#include <bb/cascades/Button>

#include <QObject>

class NSRWelcomeView : public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRWelcomeView (bb::cascades::Container *parent = 0);
	virtual ~NSRWelcomeView ();

Q_SIGNALS:
	void openDocumentRequested ();
	void recentDocumentsRequested ();

private:
	bb::cascades::Button	*_openButton;
	bb::cascades::Button	*_lastDocsButton;
};

#endif /* NSRWELCOMEVIEW_H_ */
