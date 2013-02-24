#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrimageview.h"
#include "nsrreadercore.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ActivityIndicator>

#include <QObject>

namespace bb { namespace cascades { class Application; }}

class NSRReaderBB10 : public QObject
{
    Q_OBJECT
public:
    NSRReaderBB10(bb::cascades::Application *app);
    virtual ~NSRReaderBB10 () {}

private Q_SLOTS:
    void onFileSelected (const QStringList& files);
    void onOpenActionTriggered ();
    void onPrevPageActionTriggered ();
    void onNextPageActionTriggered ();
    void onPageRendered (int number);
    void setIndicatorEnabled (bool enabled);

private:
    void updateVisualControls ();

    NSRReaderCore			*_core;
    NSRImageView			*_imageView;
    bb::cascades::pickers::FilePicker	*_filePicker;
    bb::cascades::ActionItem		*_prevPageAction;
    bb::cascades::ActionItem		*_nextPageAction;
    bb::cascades::ActivityIndicator	*_indicator;

};

#endif /* NSRREADERBB10_H_ */
