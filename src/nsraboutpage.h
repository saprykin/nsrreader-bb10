#ifndef NSRABOUTPAGE_H_
#define NSRABOUTPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/Container>
#include <bb/cascades/ScrollView>

#include <QObject>

class NSRAboutPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRAboutPage (QObject *parent = 0);
	virtual ~NSRAboutPage ();

private Q_SLOTS:
	void onSelectedIndexChanged (int index);

private:
	bb::cascades::Container		*_aboutContainer;
	bb::cascades::Container		*_helpContainer;
	bb::cascades::ScrollView	*_scrollView;
};

#endif /* NSRABOUTPAGE_H_ */
