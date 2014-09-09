#ifndef INSRSCENECOVER_H_
#define INSRSCENECOVER_H_

#include "nsrrenderedpage.h"

#include <QString>

class INSRSceneCover
{
public:
	virtual ~INSRSceneCover () {};

	virtual void setPageData (const NSRRenderedPage&	page,
			  	  const QString&		title,
			  	  int				pagesTotal) = 0;
	virtual void resetPageData () = 0;

	virtual void updateState (bool isStatic) = 0;
	virtual void setTextOnly (bool textOnly) = 0;
	virtual void setInvertedColors (bool invertedColors) = 0;
};

#endif /* INSRSCENECOVER_H_ */
