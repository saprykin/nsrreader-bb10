#ifndef NSRLASTDOCSPAGE_H_
#define NSRLASTDOCSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/ListView>

class NSRLastDocsPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRLastDocsPage (QObject *parent = 0);
	virtual ~NSRLastDocsPage ();

private:
	void loadData ();

	bb::cascades::ListView	*_listView;
};

#endif /* NSRLASTDOCSPAGE_H_ */
