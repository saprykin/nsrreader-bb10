#ifndef NSRBOOKMARKITEM_H_
#define NSRBOOKMARKITEM_H_

#include "nsrtranslator.h"

#include <bb/cascades/StandardListItem>
#include <bb/cascades/Container>

#include <QObject>

class NSRBookmarkItem : public bb::cascades::StandardListItem
{
	Q_OBJECT
public:
	NSRBookmarkItem (bb::cascades::Container *parent = 0);
	virtual ~NSRBookmarkItem ();

	inline NSRTranslator * getTranslator () const {
		return _translator;
	}

private:
	NSRTranslator *	_translator;
};

#endif /* NSRBOOKMARKITEM_H_ */
