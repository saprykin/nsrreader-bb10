#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrpageview.h"
#include "nsrreadercore.h"
#include "nsrpagestatus.h"
#include "nsrreadprogress.h"
#include "nsrwelcomeview.h"
#include "nsractionaggregator.h"
#include "nsraboutpage.h"
#include "nsrpageslider.h"
#include "nsrbpseventhandler.h"
#include "nsrtranslator.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/UIOrientation>

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
    void onReflowActionTriggered ();
    void onPrefsActionTriggered ();
    void onRecentDocsTriggered ();
    void onHelpActionTriggered ();
    void onShareActionTriggered ();
    void onPageRendered (int number);
    void onIndicatorRequested (bool enabled);
    void onIndicatorStopped ();
    void onPasswordDialogFinished (bb::system::SystemUiResult::Type res);
    void onErrorWhileOpening (NSRAbstractDocument::NSRDocumentError error);
    void onSystemLanguageChanged ();
    void onPageTapped ();
    void onViewModeRequested (NSRAbstractDocument::NSRDocumentStyle mode);
    void onPopTransitionEnded (bb::cascades::Page *page);
    void onLastDocumentRequested (const QString& path);
    void onDocumentToBeDeleted (const QString& path);
    void onZoomChanged (double zoom, NSRRenderedPage::NSRRenderReason reason);
    void onManualExit ();
    void onPrevPageRequested ();
    void onNextPageRequested ();
    void onFitToWidthRequested ();
    void onInvoke (const bb::system::InvokeRequest& req);
    void onRotateLeftRequested ();
    void onRotateRightRequested ();
    void onFullscreenSwitchRequested (bool isFullscreen);
    void onCardPooled (const bb::system::CardDoneMessage& message);
    void onCardResize (const bb::system::CardResizeMessage& message);
    void onBackButtonTriggered ();
    void onTopPagePeeked (bool isPeeked);
    void onThumbnail ();
    void onFullscreen ();
    void onOrientationAboutToChange (bb::cascades::UIOrientation::Type orientation);
    void onPageSliderInteractionStarted ();
    void onPageSliderInteractionEnded ();
    void onPageSliderValueChanged (int value);
    void onVkbVisibilityChanged (bool visible);
#ifdef NSR_LITE_VERSION
    void onLiteVersionOverPage ();
    void onBuyActionTriggered ();
#endif
    void retranslateUi ();

private:
    void initFullUI ();
    void initCardUI ();
    void updateVisualControls ();
    void disableVisualControls ();
    void reloadSettings ();
    void loadSession (const QString& path = "", int page = -1);
    void saveSession ();
    void resetState ();
    void showAboutPage (NSRAboutPage::NSRAboutSection section);
    void showToast (const QString& text, bool reset);
    int getActionBarHeight ();
    int getActionBarHeightForOrientation (bb::cascades::UIOrientation::Type orientation);

    NSRReaderCore				*_core;
    NSRPageView					*_pageView;
    NSRPageStatus				*_pageStatus;
    NSRReadProgress				*_readProgress;
    NSRWelcomeView				*_welcomeView;
    NSRActionAggregator				*_actionAggregator;
    NSRPageSlider				*_slider;
    NSRBpsEventHandler				*_bpsHandler;
    NSRTranslator				*_translator;
    QTranslator					*_qtranslator;
    bb::cascades::NavigationPane		*_naviPane;
    bb::cascades::Page				*_page;
    bb::cascades::pickers::FilePicker		*_filePicker;
    bb::cascades::ActivityIndicator		*_indicator;
    bb::system::SystemPrompt			*_prompt;
    bb::system::SystemToast			*_toast;
    bb::system::InvokeManager			*_invokeManager;
    bb::system::ApplicationStartupMode::Type	_startMode;
    bool					_isFullscreen;
    bool					_isActiveFrame;
    bool					_wasSliderVisible;
};

#endif /* NSRREADERBB10_H_ */
