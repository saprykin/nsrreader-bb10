#include "nsrsettings.h"
#include "nsrthumbnailer.h"
#include "nsrbookmarksstorage.h"
#include "nsrabstractdocument.h"

#include <QDir>
#include <QDateTime>

#ifdef NSR_LITE_VERSION
#  define NSR_SETTINGS_CONFIG_DIR 	".nsrreaderlite"
#  define NSR_LITE_MAX_PAGES	30
#else
#  define NSR_SETTINGS_CONFIG_DIR 	".nsrreader"
#endif

#define NSR_SETTINGS_GLOBAL_SECTION		"Global"

#define NSR_SETTINGS_FULLSCREEN			"fullscreen-mode"
#define NSR_SETTINGS_WORD_WRAP			"word-wrap"
#define NSR_SETTINGS_TEXT_MODE_NOTED		"text-mode-noted"
#define NSR_SETTINGS_INVERTED_COLORS		"inverted-colors"
#define NSR_SETTINGS_AUTO_CROP			"auto-crop"
#define NSR_SETTINGS_PREVENT_SCREEN_LOCK	"prevent-screen-lock"
#define NSR_SETTINGS_ENCODING_AUTODETECTION	"encoding-autodetection"
#define NSR_SETTINGS_NEWS_SHOWN_VERSION		"news-shown-version"
#define NSR_SETTINGS_LAST_OPEN_DIR		"last-open-dir"
#define NSR_SETTINGS_FONT_FAMILY		"font-family"
#define NSR_SETTINGF_TEXT_ENCODING		"text-encoding"
#define NSR_SETTINGS_LAST_DOCUMENTS		"last-documents"
#define NSR_SETTINGS_FIRST_START		"first-start"
#define NSR_SETTINGS_VISUAL_STYLE		"visual-style"

#define NSR_SETTINGS_LOAD_LAST_DOC		"load-last-doc"
#define NSR_SETTINGS_LAST_CONFIG_CLEAN		"last-config-clean"

#define NSR_SETTINGS_LAST_SESSION		"last-session"
#define NSR_SETTINGS_S_FILE			"file"
#define NSR_SETTINGS_S_PAGE			"page"
#define NSR_SETTINGS_S_ZOOM_TEXT		"zoom-text"
#define NSR_SETTINGS_S_ZOOM_GRAPHIC		"zoom-graphic"
#define NSR_SETTINGS_S_FIT_TO_WIDTH		"fit-to-width"
#define NSR_SETTINGS_S_POSITION			"position"
#define NSR_SETTINGS_S_TEXT_POSITION		"text-position"
#define NSR_SETTINGS_S_ANGLE			"angle"

#define NSR_SETTINGS_DEFAULT_FONT		"Sans Serif"
#define NSR_SETTINGS_DEFAULT_ENCODING		"UTF-8"
#define NSR_SETTINGS_DEFAULT_PATH		QDir::currentPath () + "/shared"

using namespace bb::cascades;

NSRSettings * NSRSettings::_instance = NULL;

NSRSettings::NSRSettings () :
	QSettings (NSRSettings::getSettingsDirectory () + "/config.ini",
		   QSettings::IniFormat),
	_isStarting (false)
{
	QDir	dir;

	if (!dir.exists (NSRSettings::getSettingsDirectory ()))
		dir.mkpath (NSRSettings::getSettingsDirectory ());

	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);

	_isFullscreenMode = value(NSR_SETTINGS_FULLSCREEN, false).toBool ();
	_isWordWrap = value(NSR_SETTINGS_WORD_WRAP, false).toBool ();
	_isTextModeNoted = value(NSR_SETTINGS_TEXT_MODE_NOTED, false).toBool ();
	_isInvertedColors = value(NSR_SETTINGS_INVERTED_COLORS, false).toBool ();
	_isAutoCrop = value(NSR_SETTINGS_AUTO_CROP, false).toBool ();
	_isPreventScreenLock = value(NSR_SETTINGS_PREVENT_SCREEN_LOCK, true).toBool ();
	_isEncodingAutodetection = value(NSR_SETTINGS_ENCODING_AUTODETECTION, true).toBool ();
	_lastVersionNewsShown = (value(NSR_SETTINGS_NEWS_SHOWN_VERSION, "0.0.0").toString ());
	_lastOpenDir = value(NSR_SETTINGS_LAST_OPEN_DIR, NSR_SETTINGS_DEFAULT_PATH).toString ();
	_fontFamily = value(NSR_SETTINGS_FONT_FAMILY, NSR_SETTINGS_DEFAULT_FONT).toString ();
	_textEncoding = value(NSR_SETTINGF_TEXT_ENCODING, NSR_SETTINGS_DEFAULT_ENCODING).toString ();
	_lastDocuments = value(NSR_SETTINGS_LAST_DOCUMENTS, QStringList ()).toStringList ();
	_isFirstStart = value(NSR_SETTINGS_FIRST_START, true).toBool ();
	_visualStyle = (VisualStyle::Type) (value(NSR_SETTINGS_VISUAL_STYLE, (int) VisualStyle::Dark).toInt ());

	if (!QDir(_lastOpenDir).exists ())
		_lastOpenDir = NSR_SETTINGS_DEFAULT_PATH;

	/* TODO: find a way to check if a font from configuration file exists */
	_fontFamily = NSR_SETTINGS_DEFAULT_FONT;

	if (!getSupportedEncodingsShort().contains (_textEncoding))
		_textEncoding = NSR_SETTINGS_DEFAULT_ENCODING;

	if (_visualStyle < VisualStyle::Bright || _visualStyle > VisualStyle::Dark)
		_visualStyle = VisualStyle::Bright;

	endGroup ();
	cleanOldFiles ();
}

NSRSettings *
NSRSettings::instance ()
{
	if (_instance == NULL)
		_instance = new NSRSettings ();

	return _instance;
}

void
NSRSettings::release ()
{
	if (_instance != NULL) {
		delete _instance;
		_instance = NULL;
	}
}

NSRSession
NSRSettings::getLastSession ()
{
	NSRSession	session;
	QString		lastSession = value(QString (NSR_SETTINGS_GLOBAL_SECTION) +
					    QString ("/") +
					    QString (NSR_SETTINGS_LAST_SESSION),
					    "").toString ();

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
	setValue (NSR_SETTINGS_S_FILE, session->getFile ());
	setValue (NSR_SETTINGS_S_PAGE, session->getPage ());
	setValue (NSR_SETTINGS_S_ZOOM_TEXT, session->getZoomText ());
	setValue (NSR_SETTINGS_S_ZOOM_GRAPHIC, session->getZoomGraphic ());
	setValue (NSR_SETTINGS_S_FIT_TO_WIDTH, session->isFitToWidth ());
	setValue (NSR_SETTINGS_S_POSITION, session->getPosition ());
	setValue (NSR_SETTINGS_S_TEXT_POSITION, session->getTextPosition ());
	setValue (NSR_SETTINGS_S_ANGLE, session->getRotation ());
	endGroup ();

	setValue (QString (NSR_SETTINGS_GLOBAL_SECTION) +
		  QString ("/") +
		  QString (NSR_SETTINGS_LAST_SESSION),
		  formatName);

	sync ();
}

void
NSRSettings::saveLastOpenDir (const QString& dir)
{
	_lastOpenDir = dir;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_LAST_OPEN_DIR, _lastOpenDir);
	endGroup ();

	sync ();
}

void
NSRSettings::saveFullscreenMode (bool fullscreen)
{
	_isFullscreenMode = fullscreen;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_FULLSCREEN, _isFullscreenMode);
	endGroup ();

	sync ();
}

void
NSRSettings::saveWordWrap (bool wrap)
{
	_isWordWrap = wrap;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_WORD_WRAP, _isWordWrap);
	endGroup ();

	sync ();
}

void
NSRSettings::saveWordWrapWithoutSync (bool wrap)
{
	_isWordWrap = wrap;
}

void
NSRSettings::saveTextModeNoted ()
{
	_isTextModeNoted = true;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_TEXT_MODE_NOTED, true);
	endGroup ();

	sync ();
}

void
NSRSettings::saveInvertedColors (bool inverted)
{
	_isInvertedColors = inverted;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_INVERTED_COLORS, _isInvertedColors);
	endGroup ();

	sync ();
}

void
NSRSettings::saveInvertedColorsWithoutSync (bool inverted)
{
	_isInvertedColors = inverted;
}

void
NSRSettings::saveAutoCrop (bool crop)
{
	_isAutoCrop = crop;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_AUTO_CROP, _isAutoCrop);
	endGroup ();

	sync ();
}

void
NSRSettings::savePreventScreenLock (bool preventScreenLock)
{
	_isPreventScreenLock = preventScreenLock;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_PREVENT_SCREEN_LOCK, _isPreventScreenLock);
	endGroup ();

	sync ();
}

void
NSRSettings::saveEncodingAutodetection (bool autodetection)
{
	_isEncodingAutodetection = autodetection;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_ENCODING_AUTODETECTION, _isEncodingAutodetection);
	endGroup ();

	sync ();
}

void
NSRSettings::saveNewsShown (const QString& version)
{
	_lastVersionNewsShown = version;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_NEWS_SHOWN_VERSION, version);
	endGroup ();

	sync ();
}

void
NSRSettings::saveFontFamily (const QString &ff)
{
	_fontFamily = ff;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_FONT_FAMILY, ff);
	endGroup ();

	sync ();
}

void
NSRSettings::saveTextEncoding (const QString &textEnc)
{
	_textEncoding = textEnc;
	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGF_TEXT_ENCODING, textEnc);
	endGroup ();

	sync ();
}

QStringList
NSRSettings::getSupportedEncodings ()
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
		return QString (NSR_SETTINGS_DEFAULT_ENCODING);
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
	return QDir::homePath () + "/"NSR_SETTINGS_CONFIG_DIR;
}

#ifdef NSR_LITE_VERSION
int
NSRSettings::getMaxAllowedPages ()
{
	return NSR_LITE_MAX_PAGES;
}
#endif

QString
NSRSettings::formatFileName (const QString &name)
{
	return QString(name).replace(QRegExp ("[:\\\\/]"), "_");
}

void
NSRSettings::readSession (const QString &name, NSRSession &session)
{
	beginGroup (name);
	session.setFile (value(NSR_SETTINGS_S_FILE, "").toString ());
	session.setPage (value(NSR_SETTINGS_S_PAGE, 1).toInt ());
	session.setZoomText (value(NSR_SETTINGS_S_ZOOM_TEXT, 90).toInt ());
	session.setZoomGraphic (value(NSR_SETTINGS_S_ZOOM_GRAPHIC, 100.0).toDouble ());
	session.setFitToWidth (value(NSR_SETTINGS_S_FIT_TO_WIDTH, true).toBool ());
	session.setPosition (value(NSR_SETTINGS_S_POSITION, QPointF (0, 0)).toPointF ());
	session.setTextPosition (value(NSR_SETTINGS_S_TEXT_POSITION, QPointF (0, 0)).toPointF ());
	session.setRotation ((NSRAbstractDocument::NSRDocumentRotation) ((int) (value(NSR_SETTINGS_S_ANGLE, 0).toDouble () + 0.5)));
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

	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_LAST_DOCUMENTS, QVariant (_lastDocuments));
	endGroup ();

	sync ();
}

void
NSRSettings::addLastDocument (const QString& path)
{
	_lastDocuments.removeAll (path);
	_lastDocuments.prepend (path);

	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_LAST_DOCUMENTS, QVariant (_lastDocuments));
	endGroup ();

	sync ();
}

void
NSRSettings::saveFirstStart ()
{
	_isFirstStart = false;

	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_FIRST_START, _isFirstStart);
	endGroup ();

	sync ();
}

void
NSRSettings::saveVisualStyle (bb::cascades::VisualStyle::Type visualStyle)
{
	_visualStyle = visualStyle;

	beginGroup (NSR_SETTINGS_GLOBAL_SECTION);
	setValue (NSR_SETTINGS_VISUAL_STYLE, (int) _visualStyle);
	endGroup ();

	sync ();
}

void
NSRSettings::cleanOldFiles ()
{
	QDateTime lastClean = value(QString (NSR_SETTINGS_GLOBAL_SECTION) +
				    QString ("") +
				    QString (NSR_SETTINGS_LAST_CONFIG_CLEAN),
				    QDateTime::currentDateTime()).toDateTime ();

	if (lastClean.daysTo (QDateTime::currentDateTime ()) < 30)
		return;

	QStringList childs = childGroups ();
	int count = childs.count ();

	for (int i = 0; i < count; ++i)
		if (childs.at(i) != QString (NSR_SETTINGS_GLOBAL_SECTION) &&
		    !QFile::exists (value(childs.at (i) + "/" + NSR_SETTINGS_S_FILE, "").toString ()))
			remove (childs.at(i));

	QStringList docs = value(QString (NSR_SETTINGS_GLOBAL_SECTION) +
			    	 QString ("") +
			    	 QString (NSR_SETTINGS_LAST_DOCUMENTS),
			    	 QStringList ()).toStringList ();
	count = docs.count ();

	for (int i = count - 1; i >= 0; --i)
		if (!QFile::exists (docs.at (i)))
			docs.removeAt (i);

	remove (QString (NSR_SETTINGS_GLOBAL_SECTION) + QString ("") + QString (NSR_SETTINGS_LOAD_LAST_DOC));

	setValue (QString (NSR_SETTINGS_GLOBAL_SECTION) +
		  QString ("") +
		  QString (NSR_SETTINGS_LAST_CONFIG_CLEAN),
		  lastClean);
	setValue (QString (NSR_SETTINGS_GLOBAL_SECTION) +
		  QString ("") +
		  QString (NSR_SETTINGS_LAST_DOCUMENTS),
		  QVariant (docs));
	sync ();

	NSRThumbnailer::instance()->cleanOldFiles ();
	NSRBookmarksStorage::instance()->cleanOldFiles ();
}
