#include "nsrfilesharer.h"

#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeManager>

#include <QFileInfo>

using namespace bb::system;

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
	InvokeManager		invokeManager;
	InvokeRequest		invokeRequest;
	InvokeTargetReply	*invokeReply;

	if (extension == "pdf")
		mimeType = "application/pdf";
	else if (extension == "djvu" || extension == "djv")
		mimeType = "image/vnd.djvu";
	else if (extension == "tiff" || extension == "tif")
		mimeType = "image/tiff";
	else
		mimeType = "text/plain";

	invokeRequest.setMimeType (mimeType);
	invokeRequest.setUri (QUrl::fromLocalFile (path));
	invokeRequest.setAction ("bb.action.SHARE");

	invokeReply = invokeManager.invoke (invokeRequest);

	if (invokeReply != NULL) {
		invokeReply->setParent (this);
		connect (invokeReply, SIGNAL (finished ()), invokeReply, SLOT (deleteLater ()));
	}
}

bool
NSRFileSharer::isSharable (const QString& path)
{
	return !path.startsWith ("app/native/assets", Qt::CaseSensitive);
}
