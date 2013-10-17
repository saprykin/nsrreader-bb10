#ifndef NSRGLOBALNOTIFIER_H_
#define NSRGLOBALNOTIFIER_H_

#include <QObject>

class NSRGlobalNotifier : public QObject
{
	Q_OBJECT
public:
	static NSRGlobalNotifier * instance ();

Q_SIGNALS:
	void languageChanged ();

public Q_SLOTS:
	void languageChangedSignal ();

private:
	NSRGlobalNotifier ();
	virtual ~NSRGlobalNotifier ();

	static NSRGlobalNotifier	*_instance;
};

#endif /* NSRGLOBALNOTIFIER_H_ */
