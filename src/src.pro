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

QSK_SUBDIRS = common graphic nodes controls layouts dialogs skins
INCLUDEPATH *= $${QSK_SUBDIRS}
DEPENDPATH *= $${QSK_SUBDIRS}

HEADERS += \
    common/QskAspect.h \
    common/QskBorderMetrics.h \
    common/QskBoxColors.h \
    common/QskBoxOptions.h \
    common/QskCorner.h \
    common/QskFlags.h \
    common/QskFunctions.h \
    common/QskGlobal.h \
    common/QskGradient.h \
    common/QskMargins.h \
    common/QskModule.h \
    common/QskNamespace.h \
    common/QskObjectCounter.h \
    common/QskSizePolicy.h \
    common/QskTextOptions.h

SOURCES += \
    common/QskAspect.cpp \
    common/QskBorderMetrics.cpp \
    common/QskBoxColors.cpp \
    common/QskBoxOptions.cpp \
    common/QskCorner.cpp \
    common/QskFunctions.cpp \
    common/QskGradient.cpp \
    common/QskMargins.cpp \
    common/QskModule.cpp \
    common/QskObjectCounter.cpp \
    common/QskSizePolicy.cpp \
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
    nodes/QskArcIterator.h \
    nodes/QskBorderGeometry.h \
    nodes/QskBoxMaterial.h \
    nodes/QskBoxMaterialVM.h \
    nodes/QskBoxNode.h \
    nodes/QskClipNode.h \
    nodes/QskFrameNode.h \
    nodes/QskGraphicNode.h \
    nodes/QskPlainTextRenderer.h \
    nodes/QskRectNode.h \
    nodes/QskTextNode.h \
    nodes/QskTextRenderer.h \
    nodes/QskTextureNode.h \
    nodes/QskVertexColor.h \
    nodes/QskVertexPen.h \
    nodes/QskVertexRendererColored.h \
    nodes/QskVertexRenderer.h

SOURCES += \
    nodes/QskArcIterator.cpp \
    nodes/QskBorderGeometry.cpp \
    nodes/QskBoxMaterial.cpp \
    nodes/QskBoxMaterialVM.cpp \
    nodes/QskBoxNode.cpp \
    nodes/QskClipNode.cpp \
    nodes/QskFrameNode.cpp \
    nodes/QskGraphicNode.cpp \
    nodes/QskPlainTextRenderer.cpp \
    nodes/QskRectNode.cpp \
    nodes/QskTextNode.cpp \
    nodes/QskTextRenderer.cpp \
    nodes/QskTextureNode.cpp \
    nodes/QskVertexColor.cpp \
    nodes/QskVertexPen.cpp \
    nodes/QskVertexRendererColored.cpp \
    nodes/QskVertexRenderer.cpp

HEADERS += \
    controls/QskAbstractButton.h \
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
    controls/QskInputPanel.h \
    controls/QskInputPanelSkinlet.h \
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
    controls/QskSimpleListBox.h \
    controls/QskSkinFactory.h \
    controls/QskSkin.h \
    controls/QskSkinHintTable.h \
    controls/QskAnimationHint.h \
    controls/QskSkinlet.h \
    controls/QskSkinnable.h \
    controls/QskSkinRenderer.h \
    controls/QskSkinTransition.h \
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
    controls/QskInputPanel.cpp \
    controls/QskInputPanelSkinlet.cpp \
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
    controls/QskSimpleListBox.cpp \
    controls/QskSkin.cpp \
    controls/QskSkinHintTable.cpp \
    controls/QskSkinFactory.cpp \
    controls/QskSkinlet.cpp \
    controls/QskSkinnable.cpp \
    controls/QskSkinRenderer.cpp \
    controls/QskSkinTransition.cpp \
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
    controls/QskTextLabel.cpp \
    controls/QskTextLabelSkinlet.cpp \
    controls/QskVariantAnimator.cpp \
    controls/QskWindow.cpp

HEADERS += \
    skins/material/QskMaterialSkin.h \
    skins/squiek/QskSquiekSkin.h

SOURCES += \
    skins/material/QskMaterialSkin.cpp \
    skins/squiek/QskSquiekSkin.cpp

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
