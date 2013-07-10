#ifndef NSRFILESHARER_H_
#define NSRFILESHARER_H_

#include <QObject>

class NSRFileSharer : public QObject
{
	Q_OBJECT
public:
	static NSRFileSharer * getInstance ();
	void shareFile (const QString& path);

	static bool isSharable (const QString& path);

private Q_SLOTS:
	void onArmed ();

private:
	NSRFileSharer ();
};

#endif /* NSRFILESHARER_H_ */
