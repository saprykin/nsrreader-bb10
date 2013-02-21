#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrimageview.h"
#include "nsrreadercore.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>

#include <QObject>

namespace bb { namespace cascades { class Application; }}

class NSRReaderBB10 : public QObject
{
    Q_OBJECT
public:
    NSRReaderBB10(bb::cascades::Application *app);
    virtual ~NSRReaderBB10 () {}

private slots:
    void onFileSelected (const QStringList& files);
    void onOpenActionTriggered ();

private:
    NSRImageView			*_imageView;
    bb::cascades::pickers::FilePicker	*_filePicker;
    NSRReaderCore			*_core;
};

#endif /* NSRREADERBB10_H_ */
