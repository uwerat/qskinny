/*
 *  SPDX-FileCopyrightText: 2019 Carl-Lucien Schwan <carl@carlschwan.eu>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.1 as Controls
import org.kde.kirigami 2.7 as Kirigami

/**
 * This is a standard password text field.
 *
 * Example usage for the password field component:
 * @code
 * import org.kde.kirigami 2.8 as Kirigami
 *
 * Kirigami.PasswordField {
 *     id: passwordField
 *     onAccepted: // check if passwordField.text is valid
 * }
 * @endcode
 *
 * @inherit org.kde.kirgami.ActionTextField
 */
Kirigami.ActionTextField
{
    id: root

    property bool showPassword: false
    echoMode: root.showPassword ? TextInput.Normal : TextInput.Password
    placeholderText: qsTr("Password")
    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhSensitiveData
    rightActions: [
        Kirigami.Action {
            iconName: root.showPassword ? "password-show-off" : "password-show-on"
            onTriggered: root.showPassword = !root.showPassword
        }
    ]
}
