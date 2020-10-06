/*
 *  SPDX-FileCopyrightText: 2020 by Nate Graham <nate@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.4 as QQC2
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.12 as Kirigami

import "private"

/**
 * A placeholder message indicating that a list view is empty. The message
 * comprises a label with lightened text, an optional icon above the text, and
 * an optional button below the text which can be used to easily show the user
 * what to do next to add content to the view.
 *
 * The top-level component is a ColumnLayout, so additional components items can
 * simply be added as child items and they will be positioned sanely.
 *
 * Example usage:
 *
 * @code{.qml}
 ** used as a "this view is empty" message
 * import org.kde.kirigami 2.12 as Kirigami
 *
 * ListView {
 *     id: listView
 *     model: [...]
 *     delegate: [...]
 *
 *     Kirigami.PlaceholderMessage {
 *         anchors.centerIn: parent
 *         width: parent.width - (Kirigami.Units.largeSpacing * 4)
 *
 *         visible: listView.count == 0
 *
 *         text: "There are no items in this list"
 *     }
 * }
 * @endcode
 * @code{.qml}
 ** Used as a "here's how to proceed" message
 * import org.kde.kirigami 2.12 as Kirigami
 *
 * ListView {
 *     id: listView
 *     model: [...]
 *     delegate: [...]
 *
 *     Kirigami.PlaceholderMessage {
 *         anchors.centerIn: parent
 *         width: parent.width - (Kirigami.Units.largeSpacing * 4)
 *
 *         visible: listView.count == 0
 *
 *         text: "Add an item to proceed"
 *
 *         helpfulAction: Kirigami.Action {
 *             iconName: "list-add"
 *             text: "Add item..."
 *             onTriggered: {
 *                 [...]
 *             }
 *         }
 *     }
 *     [...]
 * }
 * @endcode
 * @code{.qml}
 ** Used as a "there was a problem here" message
 * import org.kde.kirigami 2.12 as Kirigami
 *
 * Kirigami.Page {
 *     id: root
 *     readonly property bool networkConnected: [...]
 *
 *     Kirigami.PlaceholderMessage {
 *         anchors.centerIn: parent
 *         width: parent.width - (Kirigami.Units.largeSpacing * 4)
 *
 *         visible: root.networkConnected
 *
 *         icon.name: "network-disconnect"
 *         text: "Network disconnected; unable to load content"
 *     }
 * }
 * @endcode
 * @code{.qml}
 * import org.kde.kirigami 2.12 as Kirigami
 *
 ** Used as a loading indicator
 * Kirigami.Page {
 *     id: root
 *     readonly property bool loading: [...]
 *     readonly property int completionStatus: [...]
 *
 *     Kirigami.PlaceholderMessage {
 *         anchors.centerIn: parent
 *         width: parent.width - (Kirigami.Units.largeSpacing * 4)
 *
 *         visible: root.loading
 *
 *         icon.name: "my-awesome-app-icon"
 *         text: "Loading this awesome app"
 *
 *         ProgressBar {
 *             Layout.preferredWidth: Kirigami.Units.gridUnit * 20
 *             value: root.completionStatus
 *             from: 0
 *             to: 100
 *         }
 *     }
 * }
 * @endcode
 * @code{.qml}
 * import org.kde.kirigami 2.12 as Kirigami
 *
 ** Used as a "Here's what you do next" button
 * Kirigami.Page {
 *     id: root
 *
 *     Kirigami.PlaceholderMessage {
 *         anchors.centerIn: parent
 *         width: parent.width - (Kirigami.Units.largeSpacing * 4)
 *
 *         visible: root.loading
 *
 *         helpfulAction: Kirigami.Action {
 *             iconName: "list-add"
 *             text: "Add item..."
 *             onTriggered: {
 *                 [...]
 *             }
 *         }
 *     }
 * }
 * @endcode
 * @since 2.12
 */
ColumnLayout {
    id: root

    /**
     * text: string
     * The text to show as a placeholder label
     *
     * Optional; if not defined, the message will have no text. Useful when you
     * only want to display an icon, action button, and/or other custom content
     *
     * @since 5.70
     */
    property alias text: label.text

    /**
     * icon: QVariant
     * The icon to show above the text label. Accepts "icon.name" and
     * "icon.source"
     *
     * Optional; if undefined, the message will have no icon.
     * Falls back to `undefined` if the specified icon is not valid or cannot
     * be loaded.
     *
     * @since 5.70
     * @see Icon::source
     */
    property ActionIconGroup icon: ActionIconGroup {}

    /**
     * helpfulAction: QtQuickControls2 Action
     * An action that helps the user proceed. Typically used to guide the user
     * to the next step for adding content or items to an empty view.
     *
     * Optional; if undefined, no button will appear below the text label.
     *
     * @since 5.70
     */
    property alias helpfulAction: actionButton.action

    spacing: Kirigami.Units.largeSpacing

    Kirigami.Icon {

        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: Kirigami.Units.iconSizes.huge
        Layout.preferredHeight: Kirigami.Units.iconSizes.huge

        source: {
            if (root.icon.source && root.icon.source.length > 0) {
                return root.icon.source
            } else if (root.icon.name && root.icon.name.length > 0) {
                return root.icon.name
            }
            return undefined
        }

        visible: source != undefined
        opacity: 0.5
    }

    Kirigami.Heading {
        id: label

        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        horizontalAlignment: Qt.AlignHCenter

        visible: text.length > 0

        level: 2
        opacity: 0.5

        wrapMode: Text.WordWrap
    }

    QQC2.Button {
        id: actionButton

        Layout.alignment: Qt.AlignHCenter

        visible: action && action.enabled
    }
}
