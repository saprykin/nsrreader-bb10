#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrpageview.h"
#include "nsrreadercore.h"
#include "nsrpagestatus.h"
#include "nsrreadprogress.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActionItem>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/SettingsActionItem>

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
    void onGotoActionTriggered ();
    void onPrefsActionTriggered ();
    void onRecentDocsTriggered ();
    void onFitToWidthTriggered ();
    void onPageRendered (int number);
    void onIndicatorRequested (bool enabled);
    void onPasswordRequested ();
    void onPasswordDialogFinished (bb::system::SystemUiResult::Type res);
    void onGotoDialogFinished (bb::system::SystemUiResult::Type res);
    void onErrorWhileOpening (NSRAbstractDocument::DocumentError error);
    void onSystemLanguageChanged ();
    void onPageTapped ();
    void onViewModeRequested (NSRPageView::NSRViewMode mode);
    void onPopTransitionEnded (bb::cascades::Page *page);
    void onLastDocumentRequested (const QString& path);

private:
    void updateVisualControls ();
    void disableVisualControls ();
    void reloadSettings ();
    void loadSession ();
    void saveSession ();

    NSRReaderCore			*_core;
    NSRPageView				*_pageView;
    NSRPageStatus			*_pageStatus;
    NSRReadProgress			*_readProgress;
    bb::cascades::NavigationPane	*_naviPane;
    bb::cascades::Page			*_page;
    bb::cascades::pickers::FilePicker	*_filePicker;
    bb::cascades::ActionItem		*_openAction;
    bb::cascades::ActionItem		*_prevPageAction;
    bb::cascades::ActionItem		*_nextPageAction;
    bb::cascades::ActionItem		*_gotoAction;
    bb::cascades::SettingsActionItem	*_prefsAction;
    bb::cascades::ActionItem		*_recentDocsAction;
    bb::cascades::ActionItem		*_fitToWidthAction;
    bb::cascades::ActivityIndicator	*_indicator;
    bb::system::SystemPrompt		*_prompt;
    bool				_isFullscreen;
};

#endif /* NSRREADERBB10_H_ */
