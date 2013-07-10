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
NSRFileSharer::shareFile (const QString& path)
{
	if (path.isEmpty ())
		return;

	QString			extension = QFileInfo(path).suffix().toLower ();
	QString			mimeType;

	if (extension == "pdf")
		mimeType = "application/pdf";
	else if (extension == "djvu" || extension == "djv")
		mimeType = "image/vnd.djvu";
	else if (extension == "tiff" || extension == "tif")
		mimeType = "image/tiff";
	else
		mimeType = "text/plain";

	Invocation *invocation = Invocation::create (InvokeQuery::create().parent(this)
									  .mimeType(mimeType)
									  .uri(QUrl::fromLocalFile (path)));

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
