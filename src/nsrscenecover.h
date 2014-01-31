#ifndef NSRSCENECOVER_H_
#define NSRSCENECOVER_H_

#include "nsrrenderedpage.h"
#include "nsrpagestatus.h"

#include <bb/cascades/SceneCover>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/TextArea>

#include <QObject>

class NSRSceneCover : public bb::cascades::SceneCover
{
	Q_OBJECT
public:
	NSRSceneCover (QObject *parent = 0);
	virtual ~NSRSceneCover ();

	void setPageData (const NSRRenderedPage&	page,
			  const QString&		title,
			  int				pagesTotal);
	void resetPageData ();

	void updateState (bool isStatic);
	inline void setTextOnly (bool textOnly) {
		_isTextOnly = textOnly;
	}
	inline void setInvertedColors (bool invertedColors) {
		_isInvertedColors = invertedColors;
	}

private Q_SLOTS:
	void retranslateUi ();

private:
	NSRPageStatus *			_pageStatus;
	bb::cascades::Container *	_titleContainer;
	bb::cascades::Label *		_titleLabel;
	bb::cascades::ImageView *	_backView;
	bb::cascades::ImageView	*	_logoView;
	bb::cascades::ImageView *	_pageView;
	bb::cascades::TextArea *	_textView;
	bb::cascades::Container *	_textContainer;
	bool				_isTextOnly;
	bool				_isInvertedColors;
	bool				_isEmptyText;
};

#endif /* NSRSCENECOVER_H_ */
