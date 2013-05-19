#include "nsractionaggregator.h"

using namespace bb::cascades;

NSRActionAggregator::NSRActionAggregator (QObject *parent) :
	QObject (parent)
{
}

NSRActionAggregator::~NSRActionAggregator ()
{
}

void
NSRActionAggregator::addAction (const QString&				name,
                		bb::cascades::AbstractActionItem*	action)
{
	if (action == NULL || name.isEmpty ())
		return;

	_hash.insert (name, action);
}

void
NSRActionAggregator::setActionEnabled (const QString& name, bool enabled)
{
	AbstractActionItem *item = _hash.value (name);

	if (item == NULL)
		return;

	item->setEnabled (enabled);
}

bool
NSRActionAggregator::isActionEnabled (const QString& name) const
{
	AbstractActionItem *item = _hash.value (name);

	if (item == NULL)
		return false;

	return item->isEnabled ();
}

void
NSRActionAggregator::removeAction (const QString& name)
{
	_hash.remove (name);
}

void
NSRActionAggregator::clear ()
{
	_hash.clear ();
}

void
NSRActionAggregator::setAllEnabled (bool enabled)
{
	QList<AbstractActionItem *> list = _hash.values ();
	int count = list.count ();

	for (int i = 0; i < count; ++i)
		list.at(i)->setEnabled (enabled);
}




