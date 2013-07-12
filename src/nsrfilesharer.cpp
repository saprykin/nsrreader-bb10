#include "nsrfilesharer.h"

#include <bb/cascades/Invocation>
#include <bb/cascades/InvokeQuery>

#include <QFileInfo>

using namespace bb::cascades;

NSRFileSharer::NSRFileSharer () :
	QObject (NULL)
{
}

NSRFileSharer *
NSRFileSharer::getInstance ()
{
	static NSRFileSharer *sharer = NULL;

	if (sharer == NULL)
		sharer = new NSRFileSharer ();

	return sharer;
}

void
NSRFileSharer::shareFiles (const QStringList& list)
{
	if (list.isEmpty ())
		return;

	QUrl	uri;
	QString	data;
	QString	mimeType;

	if (list.count () == 1) {
		QString	extension = QFileInfo(list.first ()).suffix().toLower ();

		if (extension == "pdf")
			mimeType = "application/pdf";
		else if (extension == "djvu" || extension == "djv")
			mimeType = "image/vnd.djvu";
		else if (extension == "tiff" || extension == "tif")
			mimeType = "image/tiff";
		else
			mimeType = "text/plain";

		uri = QUrl::fromLocalFile (list.first ());
	} else {
		mimeType = "filelist/mixed";
		uri = QUrl ("list://");

		int count = list.count ();
		for (int i = 0; i < count; ++i)
			data += "{ \"uri\" : \"file://" + QUrl(list.at (i)).toLocalFile () + "\" },";

		data = "[ " + data.left (data.count () - 1) + " ]";
	}

	Invocation *invocation = Invocation::create (InvokeQuery::create().parent(this)
									  .uri(uri)
									  .data(data.toUtf8 ())
									  .mimeType(mimeType));

	Q_ASSERT (connect (invocation, SIGNAL (armed ()), this, SLOT (onArmed ())));
	Q_ASSERT (connect (invocation, SIGNAL (finished ()), invocation, SLOT (deleteLater ())));
}

bool
NSRFileSharer::isSharable (const QString& path)
{
	return !path.startsWith ("app/native/assets", Qt::CaseSensitive);
}

void
NSRFileSharer::onArmed ()
{
	if (sender () == NULL)
		return;

	Invocation *invocation = static_cast<Invocation *> (sender ());

	invocation->trigger ("bb.action.SHARE");
}
