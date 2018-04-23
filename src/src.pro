QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET   = qskinny

QSK_VER_MAJ      = 0
QSK_VER_MIN      = 0
QSK_VER_PAT      = 1
QSK_VERSION      = $${QSK_VER_MAJ}.$${QSK_VER_MIN}.$${QSK_VER_PAT}

DESTDIR  = $${QSK_OUT_ROOT}/lib

QT += quick quick-private
CONFIG += no_private_qt_headers_warning

contains(QSK_CONFIG, QskDll) {

    CONFIG += dll
    DEFINES += QSK_DLL QSK_MAKEDLL
}
else {
    CONFIG += staticlib
}

QSK_SUBDIRS = common graphic nodes controls layouts dialogs inputpanel
INCLUDEPATH *= $${QSK_SUBDIRS}
DEPENDPATH *= $${QSK_SUBDIRS}

HEADERS += \
    common/QskAspect.h \
    common/QskBoxBorderColors.h \
    common/QskBoxBorderMetrics.h \
    common/QskBoxShapeMetrics.h \
    common/QskCorner.h \
    common/QskFlags.h \
    common/QskFunctions.h \
    common/QskGlobal.h \
    common/QskGradient.h \
    common/QskMargins.h \
    common/QskMetaFunction.h \
    common/QskMetaFunction.hpp \
    common/QskMetaInvokable.h \
    common/QskModule.h \
    common/QskNamespace.h \
    common/QskObjectCounter.h \
    common/QskSizePolicy.h \
    common/QskTextColors.h \
    common/QskTextOptions.h

SOURCES += \
    common/QskAspect.cpp \
    common/QskBoxBorderColors.cpp \
    common/QskBoxBorderMetrics.cpp \
    common/QskBoxShapeMetrics.cpp \
    common/QskCorner.cpp \
    common/QskFunctions.cpp \
    common/QskGradient.cpp \
    common/QskMargins.cpp \
    common/QskMetaFunction.cpp \
    common/QskMetaInvokable.cpp \
    common/QskModule.cpp \
    common/QskObjectCounter.cpp \
    common/QskSizePolicy.cpp \
    common/QskTextColors.cpp \
    common/QskTextOptions.cpp

HEADERS += \
    graphic/QskColorFilter.h \
    graphic/QskGraphic.h \
    graphic/QskGraphicImageProvider.h \
    graphic/QskGraphicIO.h \
    graphic/QskGraphicPaintEngine.h \
    graphic/QskGraphicProvider.h \
    graphic/QskGraphicProviderMap.h \
    graphic/QskGraphicTextureFactory.h \
    graphic/QskPainterCommand.h \
    graphic/QskStandardSymbol.h

SOURCES += \
    graphic/QskColorFilter.cpp \
    graphic/QskGraphic.cpp \
    graphic/QskGraphicImageProvider.cpp \
    graphic/QskGraphicIO.cpp \
    graphic/QskGraphicPaintEngine.cpp \
    graphic/QskGraphicProvider.cpp \
    graphic/QskGraphicProviderMap.cpp \
    graphic/QskGraphicTextureFactory.cpp \
    graphic/QskPainterCommand.cpp \
    graphic/QskStandardSymbol.cpp

HEADERS += \
    nodes/QskBoxNode.h \
    nodes/QskBoxClipNode.h \
    nodes/QskBoxRenderer.h \
    nodes/QskBoxRendererColorMap.h \
    nodes/QskGraphicNode.h \
    nodes/QskPlainTextRenderer.h \
    nodes/QskRichTextRenderer.h \
    nodes/QskTextRenderer.h \
    nodes/QskTextNode.h \
    nodes/QskTextureNode.h \
    nodes/QskVertex.h

SOURCES += \
    nodes/QskBoxNode.cpp \
    nodes/QskBoxClipNode.cpp \
    nodes/QskBoxRendererRect.cpp \
    nodes/QskBoxRendererEllipse.cpp \
    nodes/QskBoxRendererDEllipse.cpp \
    nodes/QskGraphicNode.cpp \
    nodes/QskPlainTextRenderer.cpp \
    nodes/QskRichTextRenderer.cpp \
    nodes/QskTextRenderer.cpp \
    nodes/QskTextNode.cpp \
    nodes/QskTextureNode.cpp \
    nodes/QskVertex.cpp

HEADERS += \
    controls/QskAbstractButton.h \
    controls/QskAnimationHint.h \
    controls/QskAnimator.h \
    controls/QskBox.h \
    controls/QskBoxSkinlet.h \
    controls/QskControl.h \
    controls/QskDirtyItemFilter.h \
    controls/QskEvent.h \
    controls/QskFlickAnimator.h \
    controls/QskFocusIndicator.h \
    controls/QskFocusIndicatorSkinlet.h \
    controls/QskGesture.h \
    controls/QskGestureRecognizer.h \
    controls/QskGraphicLabel.h \
    controls/QskGraphicLabelSkinlet.h \
    controls/QskHintAnimator.h \
    controls/QskListView.h \
    controls/QskListViewSkinlet.h \
    controls/QskObjectTree.h \
    controls/QskPageIndicator.h \
    controls/QskPageIndicatorSkinlet.h \
    controls/QskPanGestureRecognizer.h \
    controls/QskPopup.h \
    controls/QskPopupSkinlet.h \
    controls/QskPushButton.h \
    controls/QskPushButtonSkinlet.h \
    controls/QskRangeControl.h \
    controls/QskResizable.h \
    controls/QskRgbValue.h \
    controls/QskScrollArea.h \
    controls/QskScrollView.h \
    controls/QskScrollViewSkinlet.h \
    controls/QskSeparator.h \
    controls/QskSeparatorSkinlet.h \
    controls/QskSetup.h \
    controls/QskShortcut.h \
    controls/QskShortcutMap.h \
    controls/QskSimpleListBox.h \
    controls/QskSkin.h \
    controls/QskSkinFactory.h \
    controls/QskSkinHintTable.h \
    controls/QskSkinManager.h \
    controls/QskSkinTransition.h \
    controls/QskSkinlet.h \
    controls/QskSkinnable.h \
    controls/QskSlider.h \
    controls/QskSliderSkinlet.h \
    controls/QskStatusIndicator.h \
    controls/QskStatusIndicatorSkinlet.h \
    controls/QskSubWindowArea.h \
    controls/QskSubWindowAreaSkinlet.h \
    controls/QskSubWindow.h \
    controls/QskSubWindowSkinlet.h \
    controls/QskTabBar.h \
    controls/QskTabButton.h \
    controls/QskTabButtonSkinlet.h \
    controls/QskTabView.h \
    controls/QskTabViewSkinlet.h \
    controls/QskTextInput.h \
    controls/QskTextInputSkinlet.h \
    controls/QskTextLabel.h \
    controls/QskTextLabelSkinlet.h \
    controls/QskVariantAnimator.h \
    controls/QskWindow.h

SOURCES += \
    controls/QskAbstractButton.cpp \
    controls/QskAnimator.cpp \
    controls/QskBox.cpp \
    controls/QskBoxSkinlet.cpp \
    controls/QskControl.cpp \
    controls/QskDirtyItemFilter.cpp \
    controls/QskEvent.cpp \
    controls/QskFlickAnimator.cpp \
    controls/QskFocusIndicator.cpp \
    controls/QskFocusIndicatorSkinlet.cpp \
    controls/QskGesture.cpp \
    controls/QskGestureRecognizer.cpp \
    controls/QskGraphicLabel.cpp \
    controls/QskGraphicLabelSkinlet.cpp \
    controls/QskHintAnimator.cpp \
    controls/QskListView.cpp \
    controls/QskListViewSkinlet.cpp \
    controls/QskObjectTree.cpp \
    controls/QskPageIndicator.cpp \
    controls/QskPageIndicatorSkinlet.cpp \
    controls/QskPanGestureRecognizer.cpp \
    controls/QskPopup.cpp \
    controls/QskPopupSkinlet.cpp \
    controls/QskPushButton.cpp \
    controls/QskPushButtonSkinlet.cpp \
    controls/QskRangeControl.cpp \
    controls/QskResizable.cpp \
    controls/QskRgbValue.cpp \
    controls/QskScrollArea.cpp \
    controls/QskScrollView.cpp \
    controls/QskScrollViewSkinlet.cpp \
    controls/QskSeparator.cpp \
    controls/QskSeparatorSkinlet.cpp \
    controls/QskSetup.cpp \
    controls/QskShortcut.cpp \
    controls/QskShortcutMap.cpp \
    controls/QskSimpleListBox.cpp \
    controls/QskSkin.cpp \
    controls/QskSkinHintTable.cpp \
    controls/QskSkinFactory.cpp \
    controls/QskSkinManager.cpp \
    controls/QskSkinTransition.cpp \
    controls/QskSkinlet.cpp \
    controls/QskSkinnable.cpp \
    controls/QskSlider.cpp \
    controls/QskSliderSkinlet.cpp \
    controls/QskStatusIndicator.cpp \
    controls/QskStatusIndicatorSkinlet.cpp \
    controls/QskSubWindowArea.cpp \
    controls/QskSubWindowAreaSkinlet.cpp \
    controls/QskSubWindow.cpp \
    controls/QskSubWindowSkinlet.cpp \
    controls/QskTabBar.cpp \
    controls/QskTabButton.cpp \
    controls/QskTabButtonSkinlet.cpp \
    controls/QskTabView.cpp \
    controls/QskTabViewSkinlet.cpp \
    controls/QskTextInput.cpp \
    controls/QskTextInputSkinlet.cpp \
    controls/QskTextLabel.cpp \
    controls/QskTextLabelSkinlet.cpp \
    controls/QskVariantAnimator.cpp \
    controls/QskWindow.cpp

HEADERS += \
    layouts/QskGridBox.h \
    layouts/QskIndexedLayoutBox.h \
    layouts/QskLayoutEngine.h \
    layouts/QskLayout.h \
    layouts/QskLayoutConstraint.h \
    layouts/QskLayoutItem.h \
    layouts/QskLinearBox.h \
    layouts/QskStackBoxAnimator.h \
    layouts/QskStackBox.h

SOURCES += \
    layouts/QskGridBox.cpp \
    layouts/QskIndexedLayoutBox.cpp \
    layouts/QskLayout.cpp \
    layouts/QskLayoutConstraint.cpp \
    layouts/QskLayoutEngine.cpp \
    layouts/QskLayoutItem.cpp \
    layouts/QskLinearBox.cpp \
    layouts/QskStackBoxAnimator.cpp \
    layouts/QskStackBox.cpp

HEADERS += \
    dialogs/QskDialog.h \
    dialogs/QskDialogButton.h \
    dialogs/QskDialogButtonBox.h \
    dialogs/QskDialogSubWindow.h \
    dialogs/QskDialogWindow.h \
    dialogs/QskInputSubWindow.h \
    dialogs/QskInputWindow.h \
    dialogs/QskMessageSubWindow.h \
    dialogs/QskMessageWindow.h \
    dialogs/QskSelectionSubWindow.h \
    dialogs/QskSelectionWindow.h

SOURCES += \
    dialogs/QskDialogButton.cpp \
    dialogs/QskDialogButtonBox.cpp \
    dialogs/QskDialog.cpp \
    dialogs/QskDialogSubWindow.cpp \
    dialogs/QskDialogWindow.cpp \
    dialogs/QskInputSubWindow.cpp \
    dialogs/QskInputWindow.cpp \
    dialogs/QskMessageSubWindow.cpp \
    dialogs/QskMessageWindow.cpp \
    dialogs/QskSelectionSubWindow.cpp \
    dialogs/QskSelectionWindow.cpp

SOURCES += \
    inputpanel/QskInputCompositionModel.cpp \
    inputpanel/QskInputContext.cpp \
    inputpanel/QskInputPanel.cpp \
    inputpanel/QskInputSuggestionBar.cpp \
    inputpanel/QskVirtualKeyboard.cpp

HEADERS += \
    inputpanel/QskInputCompositionModel.h \
    inputpanel/QskInputContext.h \
    inputpanel/QskInputPanel.h \
    inputpanel/QskInputSuggestionBar.h \
    inputpanel/QskVirtualKeyboard.h
