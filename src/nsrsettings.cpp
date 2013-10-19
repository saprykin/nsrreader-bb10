#include "nsrsettings.h"
#include "nsrthumbnailer.h"

#define NSR_READER_VERSION "1.3.1"

#ifdef NSR_LITE_VERSION
#  define NSR_CONFIG_DIR ".nsrreaderlite"
#else
#  define NSR_CONFIG_DIR ".nsrreader"
#endif

NSRSettings::NSRSettings (QObject *parent) :
	QSettings (NSRSettings::getSettingsDirectory () + "/config.ini",
		   QSettings::IniFormat,
		   parent)
{
	QString	defPath, defFont;
	QDir	dir;

	defPath = QDir::homePath ();
	defFont = QString ("Sans Serif");

	if (!dir.exists (QDir::homePath () + "/"NSR_CONFIG_DIR))
		dir.mkpath (QDir::homePath () + "/"NSR_CONFIG_DIR);

	beginGroup ("Global");

	_isFullscreenMode = value("fullscreen-mode", false).toBool ();
	_isWordWrap = value("word-wrap", false).toBool ();
	_isTextModeNoted = value("text-mode-noted", false).toBool ();
	_isInvertedColors = value("inverted-colors", false).toBool ();
	_isAutoCrop = value("auto-crop", false).toBool ();
	_lastOpenDir = value("last-open-dir", "C:").toString ();
	_isNewsShown = (value("news-shown-version", "1.0.0").toString () == NSR_READER_VERSION);
	_fontFamily = value("font-family", "Sans Serif").toString ();
	_textEncoding = value("text-encoding", "UTF-8").toString ();
	_lastDocuments = value("last-documents", QStringList ()).toStringList ();
	_isFirstStart = value("first-start", true).toBool ();

	if (!QDir(_lastOpenDir).exists ())
		_lastOpenDir = defPath;

	/* TODO: find a way to check if a font from configuration file exists */
	_fontFamily = defFont;

	if (!getSupportedEncodingsShort().contains (_textEncoding))
		_textEncoding = QString ("UTF-8");

	endGroup ();

	cleanOldFiles ();
}

NSRSession
NSRSettings::getLastSession()
{
	NSRSession	session;
	QString		lastSession = value("Global/last-session", "").toString ();

	if (!childGroups().contains (lastSession))
		return session;

	readSession (lastSession, session);

	return session;
}

NSRSession
NSRSettings::getSessionForFile (const QString &file)
{
	NSRSession	session;
	QString		formatName;

	formatName = formatFileName (file);
	session.setFile (file);
	session.setPage (1);
	session.setFitToWidth (true);

	if (!childGroups().contains (formatName))
		return session;

	readSession (formatName, session);

	return session;
}

void
NSRSettings::saveSession (NSRSession *session)
{
	QString formatName;

	if (session == NULL)
		return;

	formatName = formatFileName (session->getFile ());

	beginGroup (formatName);
	setValue ("file", session->getFile ());
	setValue ("page", session->getPage ());
	setValue ("zoom-text", session->getZoomText ());
	setValue ("zoom-graphic", session->getZoomGraphic ());
	setValue ("fit-to-width", session->isFitToWidth ());
	setValue ("position", session->getPosition ());
	setValue ("text-position", session->getTextPosition ());
	setValue ("angle", session->getRotation ());
	endGroup ();

	setValue ("Global/last-session", formatName);

	sync ();
}

void
NSRSettings::saveLastOpenDir (const QString& dir)
{
	_lastOpenDir = dir;
	beginGroup ("Global");
	setValue ("last-open-dir", _lastOpenDir);
	endGroup ();

	sync ();
}

void
NSRSettings::saveFullscreenMode (bool fullscreen)
{
	_isFullscreenMode = fullscreen;
	beginGroup ("Global");
	setValue ("fullscreen-mode", _isFullscreenMode);
	endGroup ();

	sync ();
}

void
NSRSettings::saveWordWrap (bool wrap)
{
	_isWordWrap = wrap;
	beginGroup ("Global");
	setValue ("word-wrap", _isWordWrap);
	endGroup ();

	sync ();
}

void
NSRSettings::saveTextModeNoted ()
{
	_isTextModeNoted = true;
	beginGroup ("Global");
	setValue ("text-mode-noted", true);
	endGroup ();

	sync ();
}

void
NSRSettings::saveInvertedColors (bool inverted)
{
	_isInvertedColors = inverted;
	beginGroup ("Global");
	setValue ("inverted-colors", _isInvertedColors);
	endGroup ();

	sync ();
}

void
NSRSettings::saveAutoCrop (bool crop)
{
	_isAutoCrop = crop;
	beginGroup ("Global");
	setValue ("auto-crop", _isAutoCrop);
	endGroup ();

	sync ();
}

void
NSRSettings::saveNewsShown ()
{
	_isNewsShown = true;
	beginGroup ("Global");
	setValue ("news-shown-version", NSR_READER_VERSION);
	endGroup ();

	sync ();
}

void
NSRSettings::saveFontFamily (const QString &ff)
{
	_fontFamily = ff;
	beginGroup ("Global");
	setValue ("font-family", ff);
	endGroup ();

	sync ();
}

void
NSRSettings::saveTextEncoding (const QString &textEnc)
{
	_textEncoding = textEnc;
	beginGroup ("Global");
	setValue ("text-encoding", textEnc);
	endGroup ();

	sync ();
}

QString
NSRSettings::getVersion()
{
#ifdef NSR_LITE_VERSION
	return  QString ("Lite ") + QString (NSR_READER_VERSION);
#else
	return QString (NSR_READER_VERSION);
#endif
}

QStringList
NSRSettings::getSupportedEncodings()
{
	QStringList codecs = QStringList ()
	       << trUtf8 ("Unicode (UTF-8)") << trUtf8 ("Western European (ISO-8859-1)")
	       << trUtf8 ("Western European (CP850)")
	       << trUtf8 ("Western European (CP1252)") << trUtf8 ("Western European (ISO-8859-15)")
	       << trUtf8 ("Central European (ISO-8859-2)") << trUtf8 ("Central European (CP1250)")
	       << trUtf8 ("South European (ISO-8859-3)") << trUtf8 ("Baltic (ISO-8859-4)")
	       << trUtf8 ("Baltic (ISO-8859-13)")
	       << trUtf8 ("Baltic (CP1257)") << trUtf8 ("Nordic (ISO-8859-10)") << trUtf8 ("Celtic (ISO-8859-14)")
	       << trUtf8 ("Romanian (ISO-8859-16)") << trUtf8 ("Greek (ISO-8859-7)") << trUtf8 ("Greek (CP1253)")
	       << trUtf8 ("Cyrillic (ISO-8859-5)") << trUtf8 ("Cyrillic (KOI8-R)") << trUtf8 ("Cyrillic (CP1251)")
	       << trUtf8 ("Cyrillic/Russia (CP866)") << trUtf8 ("Cyrillic/Ukraine (KOI8-U)")
	       << trUtf8 ("Hebrew (ISO-8859-8)") << trUtf8 ("Hebrew (ISO-8859-8-I)") << trUtf8 ("Hebrew (CP1255)")
	       << trUtf8 ("Turkish (ISO-8859-9)") << trUtf8 ("Turkish (CP1254)")
	       << trUtf8 ("Arabic (ISO-8859-6)") << trUtf8 ("Arabic (ISO-8859-6-I)") << trUtf8 ("Arabic (CP1256)")
	       << trUtf8 ("Latin/Thai (ISO-8859-11)") << trUtf8 ("Thai (CP874)")
	       << trUtf8 ("Korean (EUC-KR)") << trUtf8 ("Korean (CP949)")
	       << trUtf8 ("Japanese (Shift_JIS)") << trUtf8 ("Japanese (EUC-JP)") << trUtf8 ("Japanese (JIS7)")
	       << trUtf8 ("Japanese (ISO-2022-JP)")
	       << trUtf8 ("Chinese Simplified (GB18030)") << trUtf8 ("Chinese Simplified (GB2312)")
	       << trUtf8 ("Chinese Simplified (GBK)") << trUtf8 ("Chinese Simplified (CP936)")
	       << trUtf8 ("Chinese Traditional (Big5)") << trUtf8 ("Chinese Traditional (Big5-HKSCS)")
	       << trUtf8 ("Chinese Traditional (Big5-ETen)") << trUtf8 ("Chinese Traditional (CP950)")
	       << trUtf8 ("Tamil (TSCII)") << trUtf8 ("Vietnamese (CP1258)")
	       << trUtf8 ("Unicode (UTF-16)") << trUtf8 ("Unicode (UTF-16BE)") << trUtf8 ("Unicode (UTF-16LE)")
	       << trUtf8 ("Unicode (UTF-32)") << trUtf8 ("Unicode (UTF-32BE)") << trUtf8 ("Unicode (UTF-32LE)")
	       << trUtf8 ("Apple Roman") << trUtf8 ("WINSAMI2");

	return codecs;
}

QStringList
NSRSettings::getSupportedEncodingsShort ()
{
	static QStringList codecs = QStringList ()
	       << "UTF-8"<< "ISO-8859-1" << "CP850"
	       << "CP1252" << "ISO-8859-15"
	       << "ISO-8859-2" << "CP1250"
	       << "ISO-8859-3" << "ISO-8859-4" << "ISO-8859-13"
	       << "CP1257" << "ISO-8859-10" << "ISO-8859-14"
	       << "ISO-8859-16" << "ISO-8859-7" << "CP1253"
	       << "ISO-8859-5" << "KOI8-R" << "CP1251"
	       << "CP866" << "KOI8-U"
	       << "ISO-8859-8" << "ISO-8859-8-I" << "CP1255"
	       << "ISO-8859-9" << "CP1254"
	       << "ISO-8859-6" << "ISO-8859-6-I" << "CP1256"
	       << "ISO-8859-11" << "CP874"
	       << "EUC-KR" << "CP949"
	       << "Shift_JIS" << "EUC-JP" << "JIS7"
	       << "ISO-2022-JP"
	       << "GB18030" << "GB2312"
	       << "GBK" << "CP936"
	       << "Big5" << "Big5-HKSCS"
	       << "Big5-ETen" << "CP950"
	       << "TSCII" << "CP1258"
	       << "UTF-16" << "UTF-16BE" << "UTF-16LE" << "UTF-32"
	       << "UTF-32BE" << "UTF-32LE"
	       << "Apple Roman" << "WINSAMI2";

	return codecs;
}

QString
NSRSettings::mapIndexToEncoding (int index)
{
	QStringList list = getSupportedEncodingsShort ();

	if (index < 0 || index > list.count ())
		return QString ("UTF-8");
	else
		return list.at (index);
}

int
NSRSettings::mapEncodingToIndex (const QString& encoding)
{
	QStringList list = getSupportedEncodingsShort ();

	int index = list.indexOf (encoding, 0);

	return index >= 0 ? index : 0;
}

QString
NSRSettings::getSettingsDirectory ()
{
	return QDir::homePath () + "/"NSR_CONFIG_DIR;
}

QString
NSRSettings::formatFileName (const QString &name)
{
	return QString(name).replace(QRegExp ("[:\\\\/]"), "_");
}

void
NSRSettings::readSession (const QString &name, NSRSession &session)
{
	beginGroup (name);
	session.setFile (value("file", "").toString ());
	session.setPage (value("page", 1).toInt ());
	session.setZoomText (value("zoom-text", 90).toInt ());
	session.setZoomGraphic (value("zoom-graphic", 100.0).toDouble ());
	session.setFitToWidth (value("fit-to-width", true).toBool ());
	session.setPosition (value("position", QPointF (0, 0)).toPointF ());
	session.setTextPosition (value("text-position", QPointF (0, 0)).toPointF ());
	session.setRotation (value("angle", 0).toDouble ());
	endGroup ();
}

QStringList
NSRSettings::getLastDocuments () const
{
	return _lastDocuments;
}

void
NSRSettings::removeLastDocument (const QString& path)
{
	_lastDocuments.removeAll (path);

	beginGroup ("Global");
	setValue ("last-documents", QVariant (_lastDocuments));
	endGroup ();

	sync ();
}

void
NSRSettings::addLastDocument (const QString& path)
{
	_lastDocuments.removeAll (path);
	_lastDocuments.prepend (path);

	beginGroup ("Global");
	setValue ("last-documents", QVariant (_lastDocuments));
	endGroup ();

	sync ();
}

void
NSRSettings::saveFirstStart ()
{
	_isFirstStart = false;

	beginGroup ("Global");
	setValue ("first-start", _isFirstStart);
	endGroup ();

	sync ();
}

void
NSRSettings::cleanOldFiles ()
{
	QDateTime lastClean = value("Global/last-config-clean", QDateTime::currentDateTime()).toDateTime ();

	if (lastClean.daysTo (QDateTime::currentDateTime ()) < 30)
		return;

	QStringList childs = childGroups ();
	int count = childs.count ();

	for (int i = 0; i < count; ++i)
		if (childs.at(i) != QString ("Global") &&
		    !QFile::exists (value(childs.at (i) + "/file", "").toString ()))
			remove(childs.at(i));

	QStringList docs = value("Global/last-documents", QStringList ()).toStringList ();
	count = docs.count ();

	for (int i = count - 1; i >= 0; --i)
		if (!QFile::exists (docs.at (i)))
			docs.removeAt (i);

	remove ("Global/load-last-doc");

	setValue ("Global/last-config-clean", lastClean);
	setValue ("Global/last-documents", QVariant (docs));
	sync ();

	NSRThumbnailer::cleanOldFiles ();
}
