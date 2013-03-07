#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrimageview.h"
#include "nsrreadercore.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Page>

#include <bb/system/SystemUiResult>
#include <bb/system/SystemPrompt>

#include <QObject>

namespace bb { namespace cascades { class Application; }}

class NSRReaderBB10 : public QObject
{
    Q_OBJECT
public:
    NSRReaderBB10 (bb::cascades::Application *app);
    virtual ~NSRReaderBB10 ();

private Q_SLOTS:
    void onFileSelected (const QStringList& files);
    void onOpenActionTriggered ();
    void onPrevPageActionTriggered ();
    void onNextPageActionTriggered ();
    void onPageRendered (int number);
    void onIndicatorRequested (bool enabled);
    void onPasswordRequested ();
    void onPasswordDialogFinished (bb::system::SystemUiResult::Type res);
    void onErrorWhileOpening (NSRAbstractDocument::DocumentError error);
    void onSystemLanguageChanged ();
    void onPageTapped ();

private:
    void updateVisualControls ();
    void disableVisualControls ();
    void reloadSettings ();
    void loadSession ();
    void saveSession ();

    NSRReaderCore			*_core;
    NSRImageView			*_imageView;
    bb::cascades::Page			*_page;
    bb::cascades::pickers::FilePicker	*_filePicker;
    bb::cascades::ActionItem		*_openAction;
    bb::cascades::ActionItem		*_prevPageAction;
    bb::cascades::ActionItem		*_nextPageAction;
    bb::cascades::ActivityIndicator	*_indicator;
    bb::system::SystemPrompt		*_prompt;
};

#endif /* NSRREADERBB10_H_ */
