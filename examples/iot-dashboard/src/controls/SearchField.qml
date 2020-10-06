/*
 *  SPDX-FileCopyrightText: 2019 Carl-Lucien Schwan <carl@carlschwan.eu>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.1 as Controls
import org.kde.kirigami 2.7 as Kirigami

/**
 * This is a standard textfield following KDE HIG. Using Ctrl+F as focus
 * sequence and "Search..." as placeholder text.
 *
 * Example usage for the search field component:
 * @code
 * import org.kde.kirigami 2.8 as Kirigami
 *
 * Kirigami.SearchField {
 *     id: searchField
 *     onAccepted: console.log("Search text is " + searchField.text)
 * }
 * @endcode
 *
 * @inherit org.kde.kirgami.ActionTextField
 */
Kirigami.ActionTextField
{
    id: root

    placeholderText: qsTr("Search...")

    Accessible.name: qsTr("Search")
    Accessible.searchEdit: true

    focusSequence: "Ctrl+F"
    rightActions: [
        Kirigami.Action {
            icon.name: root.LayoutMirroring.enabled ? "edit-clear-locationbar-ltr" : "edit-clear-locationbar-rtl"
            visible: root.text.length > 0
            onTriggered: {
                root.text = ""
                root.accepted()
            }
        }
    ]
}
