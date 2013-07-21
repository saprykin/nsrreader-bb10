#ifndef NSRSCENECOVER_H_
#define NSRSCENECOVER_H_

#include <bb/cascades/SceneCover>
#include <bb/cascades/ImageView>
#include <bb/cascades/Label>
#include <bb/cascades/Image>
#include <bb/cascades/Container>

#include <QObject>

class NSRSceneCover : public bb::cascades::SceneCover
{
	Q_OBJECT
public:
	NSRSceneCover (QObject *parent = 0);
	virtual ~NSRSceneCover ();

	void setPageData (const bb::cascades::Image&	image,
			  const QString&		title,
			  int				pageNumber,
			  int				pagesTotal);
	void resetPageData ();

	void setStatic (bool isStatic);

private:
	bb::cascades::Container *	_titleContainer;
	bb::cascades::Label *		_titleLabel;
	bb::cascades::ImageView *	_backView;
	bb::cascades::ImageView	*	_logoView;
	bb::cascades::ImageView *	_pageView;
	bb::cascades::Container *	_pageNumContainer;
	bb::cascades::Label *		_pageNumLabel;
};

#endif /* NSRSCENECOVER_H_ */
