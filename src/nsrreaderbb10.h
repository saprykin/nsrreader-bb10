#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrpageview.h"
#include "nsrreadercore.h"
#include "nsrpagestatus.h"
#include "nsrreadprogress.h"
#include "nsrwelcomeview.h"
#include "nsractionaggregator.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>

#include <bb/system/SystemUiResult>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemToast>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/CardDoneMessage>

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
    void onHelpActionTriggered ();
    void onShareActionTriggered ();
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
    void onZoomChanged (double zoom, bool toWidth);
    void onManualExit ();
    void onPrevPageRequested ();
    void onNextPageRequested ();
    void onFitToWidthRequested ();
    void onSystemShortcutTriggered ();
    void onInvoke (const bb::system::InvokeRequest& req);
    void onRotateLeftRequested ();
    void onRotateRightRequested ();
    void onFullscreenSwitchRequested (bool isFullscreen);
    void onCardPooled (const bb::system::CardDoneMessage& message);
    void onCardResize (const bb::system::CardResizeMessage& message);

private:
    void initFullUI ();
    void initCardUI ();
    void updateVisualControls ();
    void disableVisualControls ();
    void reloadSettings ();
    void loadSession (const QString& path = "", int page = -1);
    void saveSession ();
    void zoomIn ();
    void zoomOut ();
    void resetState ();

    NSRReaderCore				*_core;
    NSRPageView					*_pageView;
    NSRPageStatus				*_pageStatus;
    NSRReadProgress				*_readProgress;
    NSRWelcomeView				*_welcomeView;
    NSRActionAggregator				*_actionAggregator;
    bb::cascades::NavigationPane		*_naviPane;
    bb::cascades::Page				*_page;
    bb::cascades::pickers::FilePicker		*_filePicker;
    bb::cascades::ActivityIndicator		*_indicator;
    bb::system::SystemPrompt			*_prompt;
    bb::system::SystemToast			*_toast;
    bb::system::InvokeManager			*_invokeManager;
    bb::system::ApplicationStartupMode::Type	_startMode;
    bool					_isFullscreen;
};

#endif /* NSRREADERBB10_H_ */
