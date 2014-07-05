#ifndef NSRSETTINGS_H
#define NSRSETTINGS_H

#include <QSettings>
#include <QStringList>

#include "insrsettings.h"
#include "nsrsession.h"

class NSRSettings : public QSettings, public INSRSettings
{
	Q_OBJECT
public:
	static NSRSettings * instance ();
	static void release ();

	/* INSRSettings interface */
	bool isWordWrap () const {return _isWordWrap;}
	bool isInvertedColors () const {return _isInvertedColors;}
	bool isAutoCrop () const {return _isAutoCrop;}
	QString getTextEncoding () const {return _textEncoding;}
	bool isStarting () const {return _isStarting;}

	void saveSession (NSRSession *session);
	NSRSession getLastSession ();
	NSRSession getSessionForFile (const QString& file);

	void saveFullscreenMode (bool fullscreen);
	bool isFullscreenMode () {return _isFullscreenMode;}
	void saveLastOpenDir (const QString& dir);
	QString getLastOpenDir () const {return _lastOpenDir;}
	void saveWordWrap (bool wrap);
	void saveWordWrapWithoutSync (bool wrap);
	void saveTextModeNoted ();
	bool isTextModeNoted () const {return _isTextModeNoted;}
	void saveInvertedColors (bool inverted);
	void saveInvertedColorsWithoutSync (bool inverted);
	void saveAutoCrop (bool crop);
	void savePreventScreenLock (bool preventScreenLock);
	bool isPreventScreenLock () const {return _isPreventScreenLock;}
	bool isNewsShown (const QString& version) const {return _lastVersionNewsShown == version;}
	void saveNewsShown (const QString& version);
	QString getFontFamily () const {return _fontFamily;}
	void saveFontFamily (const QString& ff);
	void saveTextEncoding (const QString& textEnc);
	QStringList getLastDocuments () const;
	void removeLastDocument (const QString& path);
	void addLastDocument (const QString& path);
	bool isFirstStart () const {return _isFirstStart;}
	void saveFirstStart ();
	inline void setStarting (bool starting) {_isStarting = starting;}

	static QStringList getSupportedEncodings ();
	static QString mapIndexToEncoding (int index);
	static int mapEncodingToIndex (const QString& encoding);
	static QString getSettingsDirectory ();
#ifdef NSR_CORE_LITE_VERSION
	static int getMaxAllowedPages ();
#endif

private:
	NSRSettings ();
	virtual ~NSRSettings () {}

	static QStringList	getSupportedEncodingsShort ();
	QString			formatFileName (const QString& name);
	void			readSession (const QString& name, NSRSession& session);
	void			cleanOldFiles ();

	static NSRSettings *	_instance;
	bool			_isFullscreenMode;
	bool			_isWordWrap;
	bool			_isTextModeNoted;
	bool			_isInvertedColors;
	bool			_isAutoCrop;
	bool			_isPreventScreenLock;
	bool			_isFirstStart;
	bool			_isStarting;
	QString			_lastVersionNewsShown;
	QString			_lastOpenDir;
	QString			_fontFamily;
	QString			_textEncoding;
	QStringList		_lastDocuments;
};

#endif // NSRSETTINGS_H
