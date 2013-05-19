#ifndef NSRACTIONAGGREGATOR_H_
#define NSRACTIONAGGREGATOR_H_

#include <QObject>
#include <QString>
#include <QHash>

#include <bb/cascades/AbstractActionItem>

class NSRActionAggregator : public QObject
{
	Q_OBJECT
public:
	NSRActionAggregator (QObject *parent = 0);
	virtual ~NSRActionAggregator ();

	void addAction (const QString& name, bb::cascades::AbstractActionItem *action);
	void setActionEnabled (const QString& name, bool enabled);
	bool isActionEnabled (const QString& name) const;
	void removeAction (const QString& name);
	void clear ();
	void setAllEnabled (bool enabled);

private:
	QHash < QString, bb::cascades::AbstractActionItem * >	_hash;
};

#endif /* NSRACTIONAGGREGATOR_H_ */
