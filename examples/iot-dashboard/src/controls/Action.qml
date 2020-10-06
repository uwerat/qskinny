/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.7
import QtQuick.Controls 2.4 as Controls
import "private"

import org.kde.kirigami 2.14 as Kirigami

/**
 * An item that represents an abstract Action
 *
 * @inherit QtQuick.Controls.Action
 */
Controls.Action {
    id: root

    /**
     * Hints for implementations using Actions indicating preferences about how to display the action.
     *
     * @deprected since 2.14, use Kirigami.DisplayHint instead.
     */
    enum DisplayHint {
        /**
         * Indicates there is no specific preference.
         */
        NoPreference = 0,
        /**
         * Only display an icon for this Action.
         */
        IconOnly = 1,
        /**
         * Try to keep the action visible even when space constrained.
         * Mutually exclusive with AlwaysHide, KeepVisible has priority.
         */
        KeepVisible = 2,
        /**
         * If possible, hide the action in an overflow menu or similar location.
         * Mutually exclusive with KeepVisible, KeepVisible has priority.
         */
        AlwaysHide = 4,
        /**
         * When this action has children, do not display any indicator (like a
         * menu arrow) for this action.
         */
        HideChildIndicator = 8
    }

    /**
     * visible: bool
     * True (default) when the graphic representation of the action
     * is supposed to be visible.
     * It's up to the action representation to honor this property.
     */
    property bool visible: true

    /**
     * iconName: string
     * Sets the icon name for the action. This will pick the icon with the given name from the current theme.
     */
    property alias iconName: root.icon.name

    /**
     * iconSource: string
     * Sets the icon file or resource url for the action. Defaults to the empty URL. Use this if you want a specific file rather than an icon from the theme
     */
    property alias iconSource: root.icon.source

     /**
     * A tooltip text to be shown when hovering the control bound to this action. Not all controls support tooltips on all platforms
     */
    property string tooltip

    /**
     * children: list<Action>
     * A list of children actions.
     * Useful for tree-like menus
     * @code
     * Action {
     *    text: "Tools"
     *    Action {
     *        text: "Action1"
     *    }
     *    Action {
     *        text: "Action2"
     *    }
     * }
     * @endcode
     */

    /**
     * separator: bool
     * Whether the action is is a separator action; defaults to false.
     */
    property bool separator: false

    /**
     * expandible: bool
     * When true, actions in globalDrawers and contextDrawers will become titles displaying te child actions as sub items
     * @since 2.6
     */
    property bool expandible: false

    property Controls.Action parent

    /**
     * displayHint: int
     *
     * A combination of values from the Action.DisplayHint enum. These are provided to implementations to indicate
     * a preference for certain display styles. The default is DisplayHint.NoPreference.
     *
     * Note that these are only preferences, implementations may choose to disregard them.
     *
     * @since 2.12
     */
    property int displayHint: Kirigami.DisplayHint.NoPreference

    /**
     * Helper function to check if a certain display hint has been set.
     *
     * This function is mostly convenience to enforce the mutual exclusivity of KeepVisible and AlwaysHide.
     *
     * @param hint The display hint to check if it is set.
     *
     * @return true if the hint was set for this action, false if not.
     *
     * @since 2.12
     *
     * @deprecated since 2.14, Use DisplayHint.displayHintSet(action, hint) instead.
     */
    function displayHintSet(hint) {
        print("Action::displayHintSet is deprecated, use DisplayHint.displayHintSet(action, hint)")
        return Kirigami.DisplayHint.displayHintSet(root, hint);
    }

    /**
     * A Component that should be preferred when displaying this Action.
     *
     * @since 5.65
     * @since 2.12
     */
    property Component displayComponent: null

    default property alias children: root.__children
    property list<QtObject> __children

    onChildrenChanged: {
        var child;
        for (var i in children) {
            child = children[i];
            if (child.hasOwnProperty("parent")) {
                child.parent = root
            }
        }
    }

    /**
     * visibleChildren: list<Action>
     * All child actions that are visible
     */
    readonly property var visibleChildren: {
        var visible = [];
        var child;
        for (var i in children) {
            child = children[i];
            if (!child.hasOwnProperty("visible") || child.visible) {
                visible.push(child)
            }
        }
        return visible;
    }
}
