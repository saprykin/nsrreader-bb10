#ifndef NSRSCENECOVER_H_
#define NSRSCENECOVER_H_

#include "insrscenecover.h"
#include "nsrrenderedpage.h"
#include "nsrpagestatus.h"

#include <bb/cascades/SceneCover>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/TextArea>

#include <QObject>

class NSRSceneCover : public bb::cascades::SceneCover,
		      public INSRSceneCover

{
	Q_OBJECT
public:
	enum NSRCoverMode {
		NSR_COVER_MODE_FULL	= 1,
		NSR_COVER_MODE_COMPACT	= 2
	};

	NSRSceneCover (NSRCoverMode mode, QObject *parent = 0);
	virtual ~NSRSceneCover ();

	inline NSRCoverMode getCoverMode () const {
		return _mode;
	}

	/* INSRSceneCover interface */
	void setPageData (const NSRRenderedPage&	page,
			  const QString&		title,
			  int				pagesTotal);
	void resetPageData ();

	void updateState (bool isStatic);
	void setTextOnly (bool textOnly);
	void setInvertedColors (bool invertedColors);

private Q_SLOTS:
	void retranslateUi ();

private:
	NSRPageStatus *			_pageStatus;
	bb::cascades::Container *	_titleContainer;
	bb::cascades::Label *		_titleLabel;
	bb::cascades::ImageView	*	_logoView;
	bb::cascades::ImageView *	_pageView;
	bb::cascades::TextArea *	_textView;
	bb::cascades::Container *	_textContainer;
	NSRCoverMode			_mode;
	bool				_isTextOnly;
	bool				_isInvertedColors;
	bool				_isEmptyText;
};

#endif /* NSRSCENECOVER_H_ */
