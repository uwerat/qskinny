############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

list(APPEND HEADERS
    common/QskArcMetrics.h
    common/QskAspect.h
    common/QskBoxBorderColors.h
    common/QskBoxBorderMetrics.h
    common/QskBoxShapeMetrics.h
    common/QskBoxHints.h
    common/QskFontRole.h
    common/QskFunctions.h
    common/QskGlobal.h
    common/QskGradient.h
    common/QskGradientDirection.h
    common/QskGradientStop.h
    common/QskGraduation.h
    common/QskGraduationMetrics.h
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
    common/QskShadowMetrics.h
    common/QskSizePolicy.h
    common/QskStateCombination.h
    common/QskStippleMetrics.h
    common/QskTextColors.h
    common/QskTextOptions.h
    common/QskTickmarks.h
)

list(APPEND SOURCES
    common/QskArcMetrics.cpp
    common/QskAspect.cpp
    common/QskBoxBorderColors.cpp
    common/QskBoxBorderMetrics.cpp
    common/QskBoxShapeMetrics.cpp
    common/QskBoxHints.cpp
    common/QskFontRole.cpp
    common/QskFunctions.cpp
    common/QskGradient.cpp
    common/QskGradientDirection.cpp
    common/QskGradientStop.cpp
    common/QskGraduation.cpp
    common/QskGraduationMetrics.cpp
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
    common/QskShadowMetrics.cpp
    common/QskSizePolicy.cpp
    common/QskStippleMetrics.cpp
    common/QskTextColors.cpp
    common/QskTextOptions.cpp
    common/QskTickmarks.cpp
)

list(APPEND PRIVATE_HEADERS
    common/QskInternalMacros.h
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
    nodes/QskArcRenderer.h
    nodes/QskArcRenderNode.h
    nodes/QskBasicLinesNode.h
    nodes/QskBoxNode.h
    nodes/QskBoxRectangleNode.h
    nodes/QskBoxRenderer.h
    nodes/QskBoxMetrics.h
    nodes/QskBoxBasicStroker.h
    nodes/QskBoxGradientStroker.h
    nodes/QskBoxShadowNode.h
    nodes/QskClipNode.h
    nodes/QskColorRamp.h
    nodes/QskFillNode.h
    nodes/QskGraduationNode.h
    nodes/QskGraduationRenderer.h
    nodes/QskGraphicNode.h
    nodes/QskTreeNode.h
    nodes/QskLinesNode.h
    nodes/QskPaintedNode.h
    nodes/QskPlainTextRenderer.h
    nodes/QskRichTextRenderer.h
    nodes/QskSceneTexture.h
    nodes/QskSGNode.h
    nodes/QskStrokeNode.h
    nodes/QskStippledLineRenderer.h
    nodes/QskShapeNode.h
    nodes/QskGradientMaterial.h
    nodes/QskTextNode.h
    nodes/QskTextRenderer.h
    nodes/QskTextureRenderer.h
    nodes/QskVertex.h
    nodes/QskVertexHelper.h
)

list(APPEND PRIVATE_HEADERS
    nodes/QskFillNodePrivate.h
)

list(APPEND SOURCES
    nodes/QskArcNode.cpp
    nodes/QskArcRenderer.cpp
    nodes/QskArcRenderNode.cpp
    nodes/QskBasicLinesNode.cpp
    nodes/QskBoxNode.cpp
    nodes/QskBoxRectangleNode.cpp
    nodes/QskBoxRenderer.cpp
    nodes/QskBoxMetrics.cpp
    nodes/QskBoxBasicStroker.cpp
    nodes/QskBoxGradientStroker.cpp
    nodes/QskBoxShadowNode.cpp
    nodes/QskClipNode.cpp
    nodes/QskColorRamp.cpp
    nodes/QskFillNode.cpp
    nodes/QskGraduationNode.cpp
    nodes/QskGraduationRenderer.cpp
    nodes/QskGraphicNode.cpp
    nodes/QskLinesNode.cpp
    nodes/QskPaintedNode.cpp
    nodes/QskPlainTextRenderer.cpp
    nodes/QskRichTextRenderer.cpp
    nodes/QskSceneTexture.cpp
    nodes/QskSGNode.cpp
    nodes/QskStrokeNode.cpp
    nodes/QskStippledLineRenderer.cpp
    nodes/QskShapeNode.cpp
    nodes/QskTreeNode.cpp
    nodes/QskGradientMaterial.cpp
    nodes/QskTextNode.cpp
    nodes/QskTextRenderer.cpp
    nodes/QskTextureRenderer.cpp
    nodes/QskVertex.cpp
)

if (QT_VERSION_MAJOR VERSION_LESS 6)
    qt_add_resources(SOURCES nodes/shaders.qrc)
else()
    list(APPEND SHADERS
        nodes/shaders/boxshadow-vulkan.vert
        nodes/shaders/boxshadow-vulkan.frag
        nodes/shaders/crisplines-vulkan.vert
        nodes/shaders/crisplines-vulkan.frag
        nodes/shaders/gradientconic-vulkan.vert
        nodes/shaders/gradientconic-vulkan.frag
        nodes/shaders/gradientlinear-vulkan.vert
        nodes/shaders/gradientlinear-vulkan.frag
        nodes/shaders/gradientradial-vulkan.vert
        nodes/shaders/gradientradial-vulkan.frag
    )
endif()

list(APPEND HEADERS
    controls/QskAbstractButton.h
    controls/QskAbstractTextInput.h
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
    controls/QskDrawer.h
    controls/QskDrawerSkinlet.h
    controls/QskEvent.h
    controls/QskFlickAnimator.h
    controls/QskFocusIndicator.h
    controls/QskFocusIndicatorSkinlet.h
    controls/QskGesture.h
    controls/QskGestureRecognizer.h
    controls/QskGraphicLabel.h
    controls/QskGraphicLabelSkinlet.h
    controls/QskHintAnimator.h
    controls/QskItem.h
    controls/QskItemAnchors.h
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
    controls/QskProgressBar.h
    controls/QskProgressBarSkinlet.h
    controls/QskProgressIndicator.h
    controls/QskProgressIndicatorSkinlet.h
    controls/QskProgressRing.h
    controls/QskProgressRingSkinlet.h
    controls/QskPushButton.h
    controls/QskPushButtonSkinlet.h
    controls/QskQuick.h
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
    controls/QskSwipeView.h
    controls/QskTabBar.h
    controls/QskTabButton.h
    controls/QskTabButtonSkinlet.h
    controls/QskTabView.h
    controls/QskTabViewSkinlet.h
    controls/QskTextArea.h
    controls/QskTextAreaSkinlet.h
    controls/QskTextEdit.h
    controls/QskTextField.h
    controls/QskTextFieldSkinlet.h
    controls/QskTextInput.h
    controls/QskTextLabel.h
    controls/QskTextLabelSkinlet.h
    controls/QskVariantAnimator.h
    controls/QskWindow.h
)

list(APPEND PRIVATE_HEADERS
    controls/QskDirtyItemFilter.h
    controls/QskInputGrabber.h
    controls/QskControlPrivate.h
    controls/QskItemPrivate.h
)

list(APPEND SOURCES
    controls/QskAbstractButton.cpp
    controls/QskAbstractTextInput.cpp
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
    controls/QskDrawer.cpp
    controls/QskDrawerSkinlet.cpp
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
    controls/QskItem.cpp
    controls/QskItemPrivate.cpp
    controls/QskItemAnchors.cpp
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
    controls/QskProgressBar.cpp
    controls/QskProgressBarSkinlet.cpp
    controls/QskProgressIndicator.cpp
    controls/QskProgressIndicatorSkinlet.cpp
    controls/QskProgressRing.cpp
    controls/QskProgressRingSkinlet.cpp
    controls/QskPushButton.cpp
    controls/QskPushButtonSkinlet.cpp
    controls/QskQuick.cpp
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
    controls/QskSwipeView.cpp
    controls/QskTabBar.cpp
    controls/QskTabButton.cpp
    controls/QskTabButtonSkinlet.cpp
    controls/QskTabView.cpp
    controls/QskTabViewSkinlet.cpp
    controls/QskTextArea.cpp
    controls/QskTextAreaSkinlet.cpp
    controls/QskTextEdit.cpp
    controls/QskTextField.cpp
    controls/QskTextFieldSkinlet.cpp
    controls/QskTextInput.cpp
    controls/QskTextLabel.cpp
    controls/QskTextLabelSkinlet.cpp
    controls/QskVariantAnimator.cpp
    controls/QskWindow.cpp
)

list(APPEND HEADERS
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

list(APPEND PRIVATE_HEADERS
    dialogs/QskColorPicker.h
    dialogs/QskColorPickerSkinlet.h
)

list(APPEND SOURCES
    dialogs/QskColorPicker.cpp
    dialogs/QskColorPickerSkinlet.cpp
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
    inputpanel/QskVirtualKeyboardLayouts.hpp
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
    target_compile_definitions(${target} PUBLIC QSK_DLL )
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL QSK_MAKEDLL)
else()
    qsk_add_library(${target} STATIC ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
endif()

if (QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
    qsk_add_shaders( ${target} FILES ${SHADERS} OUTPUT_TARGETS shader_target)
endif()

target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controls>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dialogs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/graphic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inputpanel>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/layouts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/nodes>)

target_include_directories(${target}
    INTERFACE
        $<INSTALL_INTERFACE:${QSK_INSTALL_HEADERS}>)

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
        PUBLIC $<BUILD_INTERFACE:${QSK_SOURCE_DIR}/inputcontext>)
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)

# TODO hack for standalone qvg2svg
set_target_properties(${target}
    PROPERTIES AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/qskinny_autogen)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
set_target_properties(${target} PROPERTIES PUBLIC_HEADER "${HEADERS}")
set_target_properties(${target} PROPERTIES EXPORT_NAME ${PROJECT_NAME})
set_target_properties(${target} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )

# packaging
set(PACKAGE_NAME ${PROJECT_NAME})

set(exported_targets ${target})
if(NOT BUILD_QSKDLL)
    list(APPEND exported_targets ${shader_target})
endif()


# setup destination file paths for qskinny target
install(TARGETS ${exported_targets} EXPORT ${PACKAGE_NAME}Targets
    LIBRARY DESTINATION       ${QSK_INSTALL_LIBS}
    ARCHIVE DESTINATION       ${QSK_INSTALL_LIBS}
    RUNTIME DESTINATION       ${QSK_INSTALL_LIBS}
    INCLUDES DESTINATION      ${QSK_INSTALL_HEADERS}
    PUBLIC_HEADER DESTINATION ${QSK_INSTALL_HEADERS})

# export QSkinnyTargets namespace
export(EXPORT ${PACKAGE_NAME}Targets
    NAMESPACE ${PACKAGE_NAMESPACE})

# install QSkinnyTargets under QSkinny namespace
install(EXPORT ${PACKAGE_NAME}Targets
    FILE
        ${PACKAGE_NAME}Targets.cmake
    NAMESPACE
        ${PACKAGE_NAMESPACE}
    DESTINATION
        ${PACKAGE_LOCATION})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES $<TARGET_PDB_FILE:${target}> 
        DESTINATION 
            ${QSK_INSTALL_LIBS} OPTIONAL)
endif()
