#ifndef NSRBOOKMARKSPAGE_H_
#define NSRBOOKMARKSPAGE_H_

#include <bb/cascades/Page>
#include <bb/cascades/ListView>
#include <bb/cascades/Container>
#include <bb/cascades/Label>

#include "nsrtranslator.h"

class NSRBookmarksPage : public bb::cascades::Page
{
	Q_OBJECT
public:
	NSRBookmarksPage (QObject *parent = 0);
	virtual ~NSRBookmarksPage ();

private:
	NSRTranslator *			_translator;
	bb::cascades::ListView *	_listView;
	bb::cascades::Container *	_emptyContainer;
	bb::cascades::Label		*_noBookmarksLabel;
	bb::cascades::Label		*_noFileLabel;
};

#endif /* NSRBOOKMARKSPAGE_H_ */
