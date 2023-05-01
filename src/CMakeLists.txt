############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

list(APPEND HEADERS
    common/QskArcMetrics.h
    common/QskAspect.h
    common/QskBoxBorderColors.h
    common/QskBoxBorderMetrics.h
    common/QskBoxShapeMetrics.h
    common/QskBoxHints.h
    common/QskFunctions.h
    common/QskGlobal.h
    common/QskGradient.h
    common/QskGradientDirection.h
    common/QskGradientStop.h
    common/QskHctColor.h
    common/QskIntervalF.h
    common/QskLabelData.h
    common/QskMargins.h
    common/QskMetaFunction.h
    common/QskMetaFunction.hpp
    common/QskMetaInvokable.h
    common/QskNamespace.h
    common/QskObjectCounter.h
    common/QskPlacementPolicy.h
    common/QskPlatform.h
    common/QskRgbValue.h
    common/QskScaleEngine.h
    common/QskScaleTickmarks.h
    common/QskShadowMetrics.h
    common/QskSizePolicy.h
    common/QskStateCombination.h
    common/QskTextColors.h
    common/QskTextOptions.h
)

list(APPEND SOURCES
    common/QskArcMetrics.cpp
    common/QskAspect.cpp
    common/QskBoxBorderColors.cpp
    common/QskBoxBorderMetrics.cpp
    common/QskBoxShapeMetrics.cpp
    common/QskBoxHints.cpp
    common/QskFunctions.cpp
    common/QskGradient.cpp
    common/QskGradientDirection.cpp
    common/QskGradientStop.cpp
    common/QskHctColor.cpp
    common/QskIntervalF.cpp
    common/QskLabelData.cpp
    common/QskMargins.cpp
    common/QskMetaFunction.cpp
    common/QskMetaInvokable.cpp
    common/QskObjectCounter.cpp
    common/QskPlatform.cpp
    common/QskPlacementPolicy.cpp
    common/QskRgbValue.cpp
    common/QskScaleEngine.cpp
    common/QskScaleTickmarks.cpp
    common/QskShadowMetrics.cpp
    common/QskSizePolicy.cpp
    common/QskTextColors.cpp
    common/QskTextOptions.cpp
)

list(APPEND HEADERS
    graphic/QskColorFilter.h
    graphic/QskGraphic.h
    graphic/QskGraphicImageProvider.h
    graphic/QskGraphicIO.h
    graphic/QskGraphicPaintEngine.h
    graphic/QskGraphicProvider.h
    graphic/QskGraphicProviderMap.h
    graphic/QskGraphicTextureFactory.h
    graphic/QskIcon.h
    graphic/QskPainterCommand.h
    graphic/QskStandardSymbol.h
)

list(APPEND SOURCES
    graphic/QskColorFilter.cpp
    graphic/QskGraphic.cpp
    graphic/QskGraphicImageProvider.cpp
    graphic/QskGraphicIO.cpp
    graphic/QskGraphicPaintEngine.cpp
    graphic/QskGraphicProvider.cpp
    graphic/QskGraphicProviderMap.cpp
    graphic/QskGraphicTextureFactory.cpp
    graphic/QskIcon.cpp
    graphic/QskPainterCommand.cpp
    graphic/QskStandardSymbol.cpp
)

list(APPEND HEADERS
    nodes/QskArcNode.h
    nodes/QskBoxNode.h
    nodes/QskBoxClipNode.h
    nodes/QskBoxFillNode.h
    nodes/QskBoxRectangleNode.h
    nodes/QskBoxRenderer.h
    nodes/QskBoxMetrics.h
    nodes/QskBoxBasicStroker.h
    nodes/QskBoxGradientStroker.h
    nodes/QskBoxColorMap.h
    nodes/QskBoxShadowNode.h
    nodes/QskColorRamp.h
    nodes/QskGraphicNode.h
    nodes/QskPaintedNode.h
    nodes/QskPlainTextRenderer.h
    nodes/QskRichTextRenderer.h
    nodes/QskScaleRenderer.h
    nodes/QskSGNode.h
    nodes/QskStrokeNode.h
    nodes/QskShapeNode.h
    nodes/QskGradientMaterial.h
    nodes/QskTextNode.h
    nodes/QskTextRenderer.h
    nodes/QskTextureRenderer.h
    nodes/QskTickmarksNode.h
    nodes/QskVertex.h
)

list(APPEND SOURCES
    nodes/QskArcNode.cpp
    nodes/QskBoxNode.cpp
    nodes/QskBoxClipNode.cpp
    nodes/QskBoxFillNode.cpp
    nodes/QskBoxRectangleNode.cpp
    nodes/QskBoxRenderer.cpp
    nodes/QskBoxMetrics.cpp
    nodes/QskBoxBasicStroker.cpp
    nodes/QskBoxGradientStroker.cpp
    nodes/QskBoxShadowNode.cpp
    nodes/QskColorRamp.cpp
    nodes/QskGraphicNode.cpp
    nodes/QskPaintedNode.cpp
    nodes/QskPlainTextRenderer.cpp
    nodes/QskRectangleNode.cpp
    nodes/QskRichTextRenderer.cpp
    nodes/QskScaleRenderer.cpp
    nodes/QskSGNode.cpp
    nodes/QskStrokeNode.cpp
    nodes/QskShapeNode.cpp
    nodes/QskGradientMaterial.cpp
    nodes/QskTextNode.cpp
    nodes/QskTextRenderer.cpp
    nodes/QskTextureRenderer.cpp
    nodes/QskTickmarksNode.cpp
    nodes/QskVertex.cpp
)

qt_add_resources(SOURCES nodes/shaders.qrc)

list(APPEND HEADERS
    controls/QskAbstractButton.h
    controls/QskAnimationHint.h
    controls/QskAnimator.h
    controls/QskMainView.h
    controls/QskBoundedControl.h
    controls/QskBoundedInput.h
    controls/QskBoundedRangeInput.h
    controls/QskBoundedValueInput.h
    controls/QskBox.h
    controls/QskBoxSkinlet.h
    controls/QskCheckBox.h
    controls/QskCheckBoxSkinlet.h
    controls/QskComboBox.h
    controls/QskComboBoxSkinlet.h
    controls/QskControl.h
    controls/QskEvent.h
    controls/QskFlickAnimator.h
    controls/QskFocusIndicator.h
    controls/QskFocusIndicatorSkinlet.h
    controls/QskGesture.h
    controls/QskGestureRecognizer.h
    controls/QskGraphicLabel.h
    controls/QskGraphicLabelSkinlet.h
    controls/QskHintAnimator.h
    controls/QskInputGrabber.h
    controls/QskListView.h
    controls/QskListViewSkinlet.h
    controls/QskMenu.h
    controls/QskMenuSkinlet.h
    controls/QskObjectTree.h
    controls/QskPageIndicator.h
    controls/QskPageIndicatorSkinlet.h
    controls/QskPanGestureRecognizer.h
    controls/QskPopup.h
    controls/QskPopupSkinlet.h
    controls/QskPushButton.h
    controls/QskPushButtonSkinlet.h
    controls/QskProgressBar.h
    controls/QskProgressBarSkinlet.h
    controls/QskQuick.h
    controls/QskQuickItem.h
    controls/QskRadioBox.h
    controls/QskRadioBoxSkinlet.h
    controls/QskScrollArea.h
    controls/QskScrollBox.h
    controls/QskScrollView.h
    controls/QskScrollViewSkinlet.h
    controls/QskSegmentedBar.h
    controls/QskSegmentedBarSkinlet.h
    controls/QskSeparator.h
    controls/QskSeparatorSkinlet.h
    controls/QskSetup.h
    controls/QskShortcutMap.h
    controls/QskSimpleListBox.h
    controls/QskSkin.h
    controls/QskSkinFactory.h
    controls/QskSkinHintTable.h
    controls/QskSkinHintTableEditor.h
    controls/QskSkinManager.h
    controls/QskSkinStateChanger.h
    controls/QskSkinTransition.h
    controls/QskSkinlet.h
    controls/QskSkinnable.h
    controls/QskSlider.h
    controls/QskSliderSkinlet.h
    controls/QskStatusIndicator.h
    controls/QskStatusIndicatorSkinlet.h
    controls/QskSpinBox.h
    controls/QskSpinBoxSkinlet.h
    controls/QskSubWindowArea.h
    controls/QskSubWindowAreaSkinlet.h
    controls/QskSubWindow.h
    controls/QskSubWindowSkinlet.h
    controls/QskSwitchButton.h
    controls/QskSwitchButtonSkinlet.h
    controls/QskTabBar.h
    controls/QskTabButton.h
    controls/QskTabButtonSkinlet.h
    controls/QskTabView.h
    controls/QskTabViewSkinlet.h
    controls/QskTextInput.h
    controls/QskTextInputSkinlet.h
    controls/QskTextLabel.h
    controls/QskTextLabelSkinlet.h
    controls/QskVariantAnimator.h
    controls/QskWindow.h
)

list(APPEND PRIVATE_HEADERS
    controls/QskDirtyItemFilter.h
    controls/QskControlPrivate.h
    controls/QskQuickItemPrivate.h
)

list(APPEND SOURCES
    controls/QskAbstractButton.cpp
    controls/QskAnimator.cpp
    controls/QskAnimationHint.cpp
    controls/QskMainView.cpp
    controls/QskBoundedControl.cpp
    controls/QskBoundedInput.cpp
    controls/QskBoundedRangeInput.cpp
    controls/QskBoundedValueInput.cpp
    controls/QskBox.cpp
    controls/QskBoxSkinlet.cpp
    controls/QskCheckBox.cpp
    controls/QskCheckBoxSkinlet.cpp
    controls/QskComboBox.cpp
    controls/QskComboBoxSkinlet.cpp
    controls/QskControl.cpp
    controls/QskControlPrivate.cpp
    controls/QskDirtyItemFilter.cpp
    controls/QskEvent.cpp
    controls/QskFlickAnimator.cpp
    controls/QskFocusIndicator.cpp
    controls/QskFocusIndicatorSkinlet.cpp
    controls/QskGesture.cpp
    controls/QskGestureRecognizer.cpp
    controls/QskGraphicLabel.cpp
    controls/QskGraphicLabelSkinlet.cpp
    controls/QskHintAnimator.cpp
    controls/QskInputGrabber.cpp
    controls/QskListView.cpp
    controls/QskListViewSkinlet.cpp
    controls/QskMenuSkinlet.cpp
    controls/QskMenu.cpp
    controls/QskObjectTree.cpp
    controls/QskPageIndicator.cpp
    controls/QskPageIndicatorSkinlet.cpp
    controls/QskPanGestureRecognizer.cpp
    controls/QskPopup.cpp
    controls/QskPopupSkinlet.cpp
    controls/QskPushButton.cpp
    controls/QskPushButtonSkinlet.cpp
    controls/QskProgressBar.cpp
    controls/QskProgressBarSkinlet.cpp
    controls/QskQuick.cpp
    controls/QskQuickItem.cpp
    controls/QskQuickItemPrivate.cpp
    controls/QskScrollArea.cpp
    controls/QskScrollBox.cpp
    controls/QskScrollView.cpp
    controls/QskScrollViewSkinlet.cpp
    controls/QskRadioBox.cpp
    controls/QskRadioBoxSkinlet.cpp
    controls/QskSegmentedBar.cpp
    controls/QskSegmentedBarSkinlet.cpp
    controls/QskSeparator.cpp
    controls/QskSeparatorSkinlet.cpp
    controls/QskSetup.cpp
    controls/QskShortcutMap.cpp
    controls/QskSimpleListBox.cpp
    controls/QskSkin.cpp
    controls/QskSkinHintTable.cpp
    controls/QskSkinHintTableEditor.cpp
    controls/QskSkinFactory.cpp
    controls/QskSkinManager.cpp
    controls/QskSkinTransition.cpp
    controls/QskSkinlet.cpp
    controls/QskSkinnable.cpp
    controls/QskSlider.cpp
    controls/QskSliderSkinlet.cpp
    controls/QskSpinBox.cpp
    controls/QskSpinBoxSkinlet.cpp
    controls/QskStatusIndicator.cpp
    controls/QskStatusIndicatorSkinlet.cpp
    controls/QskSubWindowArea.cpp
    controls/QskSubWindowAreaSkinlet.cpp
    controls/QskSubWindow.cpp
    controls/QskSubWindowSkinlet.cpp
    controls/QskSwitchButton.cpp
    controls/QskSwitchButtonSkinlet.cpp
    controls/QskTabBar.cpp
    controls/QskTabButton.cpp
    controls/QskTabButtonSkinlet.cpp
    controls/QskTabView.cpp
    controls/QskTabViewSkinlet.cpp
    controls/QskTextInput.cpp
    controls/QskTextInputSkinlet.cpp
    controls/QskTextLabel.cpp
    controls/QskTextLabelSkinlet.cpp
    controls/QskVariantAnimator.cpp
    controls/QskWindow.cpp
)

list(APPEND HEADERS
    layouts/QskDrawer.h
    layouts/QskGridBox.h
    layouts/QskGridLayoutEngine.h
    layouts/QskIndexedLayoutBox.h
    layouts/QskLayoutChain.h
    layouts/QskLayoutEngine2D.h
    layouts/QskLayoutElement.h
    layouts/QskLayoutMetrics.h
    layouts/QskLinearBox.h
    layouts/QskLinearLayoutEngine.h
    layouts/QskStackBoxAnimator.h
    layouts/QskStackBox.h
)

list(APPEND PRIVATE_HEADERS
    layouts/QskSubcontrolLayoutEngine.h
)

list(APPEND SOURCES
    layouts/QskDrawer.cpp
    layouts/QskGridBox.cpp
    layouts/QskGridLayoutEngine.cpp
    layouts/QskIndexedLayoutBox.cpp
    layouts/QskLayoutChain.cpp
    layouts/QskLayoutEngine2D.cpp
    layouts/QskLayoutElement.cpp
    layouts/QskLayoutMetrics.cpp
    layouts/QskLinearBox.cpp
    layouts/QskLinearLayoutEngine.cpp
    layouts/QskStackBoxAnimator.cpp
    layouts/QskStackBox.cpp
    layouts/QskSubcontrolLayoutEngine.cpp
)

list(APPEND HEADERS
    dialogs/QskDialog.h
    dialogs/QskDialogButton.h
    dialogs/QskDialogButtonBox.h
    dialogs/QskDialogSubWindow.h
    dialogs/QskDialogWindow.h
    dialogs/QskMessageSubWindow.h
    dialogs/QskMessageWindow.h
    dialogs/QskSelectionSubWindow.h
    dialogs/QskSelectionWindow.h
)

list(APPEND SOURCES
    dialogs/QskDialogButton.cpp
    dialogs/QskDialogButtonBox.cpp
    dialogs/QskDialog.cpp
    dialogs/QskDialogSubWindow.cpp
    dialogs/QskDialogWindow.cpp
    dialogs/QskMessageSubWindow.cpp
    dialogs/QskMessageWindow.cpp
    dialogs/QskSelectionSubWindow.cpp
    dialogs/QskSelectionWindow.cpp
)

list(APPEND HEADERS
    inputpanel/QskTextPredictor.h
    inputpanel/QskInputContext.h
    inputpanel/QskInputPanel.h
    inputpanel/QskInputPanelBox.h
    inputpanel/QskInputPredictionBar.h
    inputpanel/QskVirtualKeyboard.h
)

list(APPEND SOURCES
    inputpanel/QskTextPredictor.cpp
    inputpanel/QskInputContext.cpp
    inputpanel/QskInputPanel.cpp
    inputpanel/QskInputPanelBox.cpp
    inputpanel/QskInputPredictionBar.cpp
    inputpanel/QskVirtualKeyboard.cpp
)

if(ENABLE_PINYIN)
    list(APPEND HEADERS inputpanel/QskPinyinTextPredictor.h)
    list(APPEND SOURCES inputpanel/QskPinyinTextPredictor.cpp)
endif()

if(ENABLE_HUNSPELL)
    list(APPEND HEADERS inputpanel/QskHunspellTextPredictor.h)
    list(APPEND SOURCES inputpanel/QskHunspellTextPredictor.cpp)
endif()

set(target qskinny)

if(BUILD_QSKDLL)
    qsk_add_library(${target} SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
else()
    qsk_add_library(${target} STATIC ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
endif()

if(BUILD_QSKDLL)
    target_compile_definitions(${target} PUBLIC QSK_DLL )
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL QSK_MAKEDLL)
endif()

target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controls>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dialogs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/graphic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inputpanel>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/layouts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/nodes>)

target_link_libraries(${target}
    PUBLIC Qt::Core Qt::CorePrivate Qt::Quick Qt::QuickPrivate)

if (QT_VERSION_MAJOR VERSION_GREATER 5)
    target_link_libraries(${target} PUBLIC Qt::OpenGL Qt::OpenGLPrivate)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(${target} PRIVATE hunspell)
endif()

if(ENABLE_PINYIN)
    target_compile_definitions(${target} PRIVATE PINYIN)
    target_link_libraries(${target} PRIVATE pinyin Fcitx5::Utils)

    target_include_directories(${target}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inputcontext>)
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)

# TODO hack for standalone qvg2svg
set_target_properties(${target}
    PROPERTIES AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/qskinny_autogen)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
set_target_properties(${target} PROPERTIES PUBLIC_HEADER "${HEADERS}")
set_target_properties(${target} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

install(TARGETS ${target} )
