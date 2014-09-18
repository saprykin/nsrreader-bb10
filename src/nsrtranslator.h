#ifndef NSRTRANSLATOR_H_
#define NSRTRANSLATOR_H_

#include <bb/cascades/UIObject>

#include <QObject>
#include <QHash>
#include <QString>

class NSRTranslator : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRTranslatorType);
public:
	NSRTranslator (QObject *parent = 0);
	virtual ~NSRTranslator ();

	enum NSRTranslatorType {
		NSR_TRANSLATOR_TYPE_LABEL		= 0,
		NSR_TRANSLATOR_TYPE_BUTTON		= 1,
		NSR_TRANSLATOR_TYPE_ACTION		= 2,
		NSR_TRANSLATOR_TYPE_ACTIONSET_TITLE	= 3,
		NSR_TRANSLATOR_TYPE_ACTIONSET_SUBTITLE	= 4,
		NSR_TRANSLATOR_TYPE_OPTION		= 5,
		NSR_TRANSLATOR_TYPE_A11Y		= 6,
		NSR_TRANSLATOR_TYPE_SELECTHDL		= 7,
		NSR_TRANSLATOR_TYPE_TITLEBAR		= 8,
		NSR_TRANSLATOR_TYPE_TAB			= 9,
		NSR_TRANSLATOR_TYPE_HEADER		= 10,
		NSR_TRANSLATOR_TYPE_DROPDOWN_TITLE	= 11
	};

	void addTranslatable (bb::cascades::UIObject 	*obj,
			      NSRTranslatorType 	type,
			      const QString&		context,
			      const QString&		text,
			      const QString&		disamb = QString ());
	void removeTranslatable (bb::cascades::UIObject *obj);

	static QString translatePath (const QString& path);

public Q_SLOTS:
	void translate ();

private:
	struct NSRTranslatorInfo
	{
		QString			context;
		QString			text;
		QString			disamb;
		NSRTranslatorType	type;
	};

	QHash<bb::cascades::UIObject *, NSRTranslatorInfo *>	_hash;
};

#endif /* NSRTRANSLATOR_H_ */
