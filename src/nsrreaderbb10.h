#ifndef NSRREADERBB10_H_
#define NSRREADERBB10_H_

#include "nsrpageview.h"
#include "nsrreadercore.h"
#include "nsrpagestatus.h"
#include "nsrwelcomeview.h"
#include "nsractionaggregator.h"
#include "nsraboutpage.h"
#include "nsrpageslider.h"
#include "nsrbpseventhandler.h"
#include "nsrtranslator.h"
#include "nsrbookmarkspage.h"
#include "insrscenecover.h"

#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/Application>
#include <bb/cascades/ImageView>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Page>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/UIOrientation>

#include <bb/system/SystemUiResult>
#include <bb/system/SystemPrompt>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/CardDoneMessage>

#include <bb/multimedia/MediaKeyWatcher>

#include <QObject>

class NSRReaderBB10 : public QObject
{
    Q_OBJECT
public:
    NSRReaderBB10 (bb::cascades::Application *app);
    virtual ~NSRReaderBB10 ();

    static QString getVersion ();

private Q_SLOTS:
    void onFileSelected (const QStringList& files);
    void onOpenActionTriggered ();
    void onPrevPageActionTriggered ();
    void onNextPageActionTriggered ();
    void onGotoActionTriggered ();
    void onReflowActionTriggered ();
    void onInvertActionTriggered ();
    void onPrefsActionTriggered ();
    void onHelpActionTriggered ();
    void onShareActionTriggered ();
    void onBookmarkActionTriggered ();
    void onPageRendered (int number);
    void onIndicatorRequested (bool enabled);
    void onIndicatorStopped ();
    void onRecentDocumentsRequested ();
    void onPasswordDialogFinished (bb::system::SystemUiResult::Type res);
    void onAddBookmarkDialogFinished (bb::system::SystemUiResult::Type res);
    void onPermissionDialogFinished (bb::system::SystemUiResult::Type res);
    void onErrorWhileOpening (NSRAbstractDocument::NSRDocumentError error);
    void onSystemLanguageChanged ();
    void onPageTapped ();
    void onPopTransitionEnded (bb::cascades::Page *page);
    void onLastDocumentRequested (const QString& path);
    void onDocumentToBeDeleted (const QString& path);
    void onZoomChanged (double zoom, NSRRenderRequest::NSRRenderReason reason);
    void onManualExit ();
    void onPrevPageRequested ();
    void onNextPageRequested ();
    void onFitToWidthRequested ();
    void onInvoke (const bb::system::InvokeRequest& req);
    void onRotateLeftRequested ();
    void onRotateRightRequested ();
    void onFullscreenSwitchRequested (bool isFullscreen);
    void onPreventScreenLockSwitchRequested (bool isPreventScreenLock);
    void onCardPooled (const bb::system::CardDoneMessage& message);
    void onCardResize (const bb::system::CardResizeMessage& message);
    void onThumbnail ();
    void onFullscreen ();
    void onOrientationAboutToChange (bb::cascades::UIOrientation::Type orientation);
    void onPageViewSizeChanged (const QSize& size);
    void onPageSliderInteractionStarted ();
    void onPageSliderInteractionEnded ();
    void onPageSliderValueChanged (int value);
    void onVkbVisibilityChanged (bool visible);
    void onBookmarkChanged (int page, bool removed);
    void onBookmarkPageRequested (int page);
    void onDocumentOpened (const QString &path);
    void onDocumentClosed ();
#ifdef NSR_LITE_VERSION
    void onLiteVersionPagesLimitPassed ();
    void onBuyActionTriggered ();
#endif
    void retranslateUi ();
    void retranslateBookmarkAction (bool hasBookmark);
    void setVolumeKeysEnabled (bool enabled);

private:
    void initFullUI ();
    void updateVisualControls ();
    void disableVisualControls ();
    void loadSession (const QString& path = "", int page = -1);
    void saveSession ();
    void resetState ();
    void showAboutPage (NSRAboutPage::NSRAboutSection section);
    void showToast (const QString& text, bool reset);
    int getActionBarHeight ();
    int getActionBarHeightForOrientation (bb::cascades::UIOrientation::Type orientation);
    NSRBookmarksPage * getBookmarksPage () const;

    NSRReaderCore				*_core;
    NSRPageView					*_pageView;
    NSRPageStatus				*_pageStatus;
    NSRWelcomeView				*_welcomeView;
    NSRActionAggregator				*_actionAggregator;
    NSRPageSlider				*_slider;

    INSRSceneCover				*_sceneCover;
    NSRBpsEventHandler				*_bpsHandler;
    NSRTranslator				*_translator;
    QTranslator					*_qtranslator;
    bb::cascades::NavigationPane		*_naviPane;
    bb::cascades::Page				*_page;
    bb::cascades::pickers::FilePicker		*_filePicker;
    bb::cascades::ActivityIndicator		*_indicator;
    bb::system::SystemPrompt			*_prompt;
    bb::system::SystemToast			*_toast;
    bb::system::SystemDialog			*_dialog;
    bb::system::InvokeManager			*_invokeManager;
    bb::multimedia::MediaKeyWatcher		*_mediaKeys[2];
    bb::system::ApplicationStartupMode::Type	_startMode;
    bool					_isFullscreen;
    bool					_isActiveFrame;
    bool					_isWaitingForFirstPage;
    bool					_wasSliderVisible;
};

#endif /* NSRREADERBB10_H_ */
