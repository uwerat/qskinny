############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    label/LabelPage.h label/LabelPage.cpp
    inputs/InputPage.h inputs/InputPage.cpp
    progressbar/ProgressBarPage.h progressbar/ProgressBarPage.cpp
    button/ButtonPage.h button/ButtonPage.cpp
    selector/SelectorPage.h selector/SelectorPage.cpp
    dialog/DialogPage.h dialog/DialogPage.cpp
    listbox/ListBoxPage.h listbox/ListBoxPage.cpp
    Page.h Page.cpp
    FocusIndicator.h FocusIndicator.cpp
    main.cpp
)
qt_add_resources(SOURCES icons.qrc)

qsk_add_example(gallery ${SOURCES})

