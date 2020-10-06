/*
 *  SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import org.kde.kirigami 2.12 as Kirigami

StackView {
    popEnter: Transition {
        OpacityAnimator {
            from: 0
            to: 1
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutCubic
        }
    }
    popExit: Transition {
        ParallelAnimation {
            OpacityAnimator {
                from: 1
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutCubic
            }
            YAnimator {
                from: 0
                to: height/2
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InCubic
            }
        }
    }

    pushEnter: Transition {
        ParallelAnimation {
            //NOTE: It's a PropertyAnimation instead of an Animator because with an animator the item will be visible for an instant before starting to fade
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutCubic
            }
            YAnimator {
                from: height/2
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
    }


    pushExit: Transition {
        OpacityAnimator {
            from: 1
            to: 0
            duration: Kirigami.Units.longDuration
            easing.type: Easing.InOutCubic
        }
    }

    replaceEnter: Transition {
        ParallelAnimation {
            OpacityAnimator {
                from: 0
                to: 1
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutCubic
            }
            YAnimator {
                from: height/2
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
    }

    replaceExit: Transition {
        ParallelAnimation {
            OpacityAnimator {
                from: 1
                to: 0
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InCubic
            }
            YAnimator {
                from: 0
                to: -height/2
                duration: Kirigami.Units.longDuration
                easing.type: Easing.InOutCubic
            }
        }
    }
}