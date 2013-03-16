#include "nsrsettings.h"
#include "nsrthumbnailer.h"

#define NSR_READER_VERSION "1.0.0"

#ifdef NSR_LITE_VERSION
#  define NSR_CONFIG_DIR ".nsrreaderlite"
#else
#  define NSR_CONFIG_DIR ".nsrreader"
#endif

NSRSettings::NSRSettings(QObject *parent) :
	QSettings (NSRSettings::getSettingsDirectory() + "/config.ini",
		   QSettings::IniFormat,
		   parent)
{
	QString	defPath, defFont;
	QDir	dir;

	defPath = QDir::homePath ();
	defFont = QString ("Sans Serif");

	if (!dir.exists (QDir::homePath () + "/"NSR_CONFIG_DIR))
		dir.mkpath (QDir::homePath () + "/"NSR_CONFIG_DIR);

	beginGroup("Global");

	_isLoadLastDoc = value("load-last-doc", true).toBool();
	_isFullscreenMode = value("fullscreen-mode", false).toBool();
	_isWordWrap = value("word-wrap", false).toBool();
	_isTextModeNoted = value("text-mode-noted", false).toBool();
	_isInvertedColors = value("inverted-colors", false).toBool();
	_lastOpenDir = value("last-open-dir", "C:").toString();
	_isNewsShown = (value("news-shown-version", "1.0.0").toString() == NSR_READER_VERSION);
	_fontFamily = value("font-family", "Nokia Sans S60").toString();
	_textEncoding = value("text-encoding", "UTF-8").toString();
	_lastDocuments = value("last-documents", QStringList ()).toStringList();

	if (!QDir(_lastOpenDir).exists())
		_lastOpenDir = defPath;

	/* TODO: find a way to check if a font from configuration file exists */
	_fontFamily = defFont;

	if (!getSupportedEncodings().contains(_textEncoding))
		_textEncoding = QString ("UTF-8");

	endGroup();

	cleanOldFiles();
}

NSRSession NSRSettings::getLastSession()
{
	NSRSession session;

	QString lastSession = value("Global/last-session", "").toString();

	if (!childGroups().contains(lastSession))
		return session;

	readSession (lastSession, session);

	return session;
}

NSRSession NSRSettings::getSessionForFile (const QString &file)
{
	NSRSession	session;
	QString		formatName;

	formatName = formatFileName (file);
	session.setFile (file);
	session.setPage (1);

	if (!childGroups().contains(formatName))
		return session;

	readSession (formatName, session);

	return session;
}

void NSRSettings::saveSession(NSRSession *session)
{
	QString formatName;

	if (session == NULL)
		return;

	formatName = formatFileName(session->getFile());

	beginGroup(formatName);
	setValue("file", session->getFile());
	setValue("page", session->getPage());
	setValue("zoom-text", session->getZoomText());
	setValue("zoom-graphic", session->getZoomGraphic());
	setValue("fit-to-width", session->isFitToWidth());
	setValue("position", session->getPosition());
	setValue("angle", session->getRotation());
	endGroup();

	setValue("Global/last-session", formatName);

	sync();
}

void NSRSettings::saveLoadLastDoc(bool load)
{
	beginGroup("Global");
	setValue("load-last-doc", load);
	endGroup();

	sync();
}

void NSRSettings::saveLastOpenDir(const QString& dir)
{
	_lastOpenDir = dir;
	beginGroup("Global");
	setValue("last-open-dir", _lastOpenDir);
	endGroup();

	sync();
}

void NSRSettings::saveFullscreenMode(bool fullscreen)
{
	_isFullscreenMode = fullscreen;
	beginGroup("Global");
	setValue("fullscreen-mode", _isFullscreenMode);
	endGroup();

	sync();
}

void NSRSettings::saveWordWrap(bool wrap)
{
	_isWordWrap = wrap;
	beginGroup("Global");
	setValue("word-wrap", _isWordWrap);
	endGroup();

	sync();
}

void NSRSettings::saveTextModeNoted()
{
	_isTextModeNoted = true;
	beginGroup("Global");
	setValue("text-mode-noted", true);
	endGroup();

	sync();
}

void NSRSettings::saveInvertedColors(bool inverted)
{
	_isInvertedColors = inverted;
	beginGroup("Global");
	setValue("inverted-colors", _isInvertedColors);
	endGroup();

	sync();
}

void NSRSettings::saveNewsShown()
{
	_isNewsShown = true;
	beginGroup("Global");
	setValue("news-shown-version", NSR_READER_VERSION);
	endGroup();

	sync();
}

void NSRSettings::saveFontFamily (const QString &ff)
{
	_fontFamily = ff;
	beginGroup("Global");
	setValue("font-family", ff);
	endGroup();

	sync();
}

void NSRSettings::saveTextEncoding (const QString &textEnc)
{
	_textEncoding = textEnc;
	beginGroup("Global");
	setValue("text-encoding", textEnc);
	endGroup();

	sync();
}

QString NSRSettings::getVersion()
{
#ifdef NSR_LITE_VERSION
	return  QString ("Lite ") + QString (NSR_READER_VERSION);
#else
	return QString (NSR_READER_VERSION);
#endif
}

QStringList NSRSettings::getSupportedEncodings()
{
	QStringList codecs;

	codecs << "UTF-8" << "ISO-8859-1" << "ISO-8859-2" << "ISO-8859-3" << "ISO-8859-4"
	       << "ISO-8859-5" << "ISO-8859-6" << "ISO-8859-7" << "ISO-8859-8" << "ISO-8859-9"
	       << "ISO-8859-10" << "ISO-8859-11" << "ISO-8859-13" << "ISO-8859-14" << "ISO-8859-15"
	       << "ISO-8859-16" << "Shift JIS" << "EUC-JP" << "EUC-KR" << "ISO-2022-JP" << "TSCII"
	       << "GB18030" << "GB2312" << "KOI8-R" << "KOI8-U" << "CP850" << "CP866" <<  "CP874"
	       << "CP936" << "CP950" << "CP1250" << "CP1251" << "CP1252" << "CP1253" << "CP1254"
	       << "CP1255" << "CP1256" << "CP1257" << "CP1258"
	       << "UTF-16" << "UTF-16BE" << "UTF-16LE" << "UTF-32" << "UTF-32BE" << "UTF-32LE"
	       << "Apple Roman" << "WINSAMI2";

	return codecs;
}

QString
NSRSettings::getSettingsDirectory ()
{
	return QDir::homePath () + "/"NSR_CONFIG_DIR;
}

QString NSRSettings::formatFileName (const QString &name)
{
	return QString(name).replace(QRegExp ("[:\\\\/]"), "_");
}

void NSRSettings::readSession (const QString &name, NSRSession &session)
{
	beginGroup(name);
	session.setFile(value("file", "").toString());
	session.setPage(value("page", 0).toInt());
	session.setZoomText(value("zoom-text", 100).toInt());
	session.setZoomGraphic(value("zoom-graphic", 100).toInt());
	session.setFitToWidth(value("fit-to-width", false).toBool());
	session.setPosition(value("position", QPoint (0, 0)).toPoint());
	session.setRotation(value("angle", 0).toDouble());
	endGroup();
}

QStringList
NSRSettings::getLastDocuments () const
{
	return _lastDocuments;
}

void
NSRSettings::addLastDocument (const QString& path)
{
	if (_lastDocuments.contains (path))
		_lastDocuments.removeAll (path);

	_lastDocuments.prepend (path);

	beginGroup ("Global");
	setValue ("last-documents", QVariant (_lastDocuments));
	endGroup ();

	sync ();
}

void NSRSettings::cleanOldFiles ()
{
	QDateTime lastClean = value("Global/last-config-clean", QDateTime::currentDateTime()).toDateTime();

	if (lastClean.daysTo(QDateTime::currentDateTime()) < 30)
		return;

	QStringList childs = childGroups();
	int count = childs.count();

	for (int i = 0; i < count; ++i)
		if (childs.at(i) != QString ("Global") &&
		    !QFile::exists(value(childs.at(i) + "/file", "").toString()))
			remove(childs.at(i));

	QStringList docs = value("Global/last-documents", QStringList ()).toStringList ();
	count = docs.count ();

	for (int i = count - 1; i >= 0; --i)
		if (!QFile::exists (docs.at (i)))
			docs.removeAt (i);

	setValue ("Global/last-config-clean", lastClean);
	setValue ("Global/last-documents", QVariant (docs));
	sync();

	NSRThumbnailer::cleanOldFiles ();
}
