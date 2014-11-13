#ifndef NSRWELCOMEVIEW_H_
#define NSRWELCOMEVIEW_H_

#include "nsrtranslator.h"

#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/cascades/Label>

#include <QObject>

class NSRWelcomeView : public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRWelcomeView (bb::cascades::Container *parent = 0);
	virtual ~NSRWelcomeView ();

	void setCardMode (bool enabled);
	void setReadOnly (bool readOnly);

Q_SIGNALS:
	void openDocumentRequested ();
	void recentDocumentsRequested ();

private Q_SLOTS:
	void onDynamicDUFactorChanged (float dduFactor);

private:
	NSRTranslator		*_translator;
	bb::cascades::Button	*_openButton;
	bb::cascades::Button	*_lastDocsButton;
	bb::cascades::Label	*_startLabel;
};

#endif /* NSRWELCOMEVIEW_H_ */
