#ifndef NSRREADPROGRESS_H_
#define NSRREADPROGRESS_H_

#include <QRectF>

#include <bb/cascades/Container>

class NSRReadProgress : public bb::cascades::Container
{
	Q_OBJECT
public:
	NSRReadProgress (bb::cascades::Container *parent = 0);
	virtual ~NSRReadProgress ();
	void setPagesCount (int count);
	int getPagesCount () const;
	void setCurrentPage (int page);
	int getCurrentPage () const;

private Q_SLOTS:
	void onLayoutFrameChanged (const QRectF& rect);

private:
	void updateProgress ();

	bb::cascades::Container	*_progressContainer;
	int			_pagesCount;
	int			_currentPage;
	double			_width;
};

#endif /* NSRREADPROGRESS_H_ */
