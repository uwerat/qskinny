/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

class ApplicationHeaderStyle : public QObject
{
    Q_OBJECT

public:
    enum Status {
        Auto = 0,
        Breadcrumb,
        Titles,
        TabBar,
        ToolBar, ///@since 5.48
        None ///@since 5.48
    };
    Q_ENUM(Status)

    enum NavigationButton {
        NoNavigationButtons = 0,
        ShowBackButton = 0x1,
        ShowForwardButton = 0x2
    };
    Q_ENUM(NavigationButton)
    Q_DECLARE_FLAGS(NavigationButtons, NavigationButton)
};

class MessageType : public QObject
{
    Q_OBJECT

public:
    enum Type {
        Information = 0,
        Positive,
        Warning,
        Error
    };
    Q_ENUM(Type)
};

class DisplayHint : public QObject
{
    Q_OBJECT

public:
    /**
     * Hints for implementations using Actions indicating preferences about how to display the action.
     */
    enum Hint : uint {
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
    };
    Q_DECLARE_FLAGS(DisplayHints, Hint)
    Q_ENUM(Hint)
    Q_FLAG(DisplayHints)

    // Note: These functions are instance methods because they need to be
    // exposed to QML. Unfortunately static methods are not supported.

    /**
     * Helper function to check if a certain display hint has been set.
     *
     * This function is mostly convenience to enforce certain behaviour of the
     * various display hints, primarily the mutual exclusivity of KeepVisible
     * and AlwaysHide.
     *
     * @param values The display hints to check.
     * @param hint The display hint to check if it is set.
     *
     * @return true if the hint was set for this action, false if not.
     *
     * @since 2.14
     */
    Q_INVOKABLE bool displayHintSet(DisplayHints values, Hint hint);

    /**
     * Check if a certain display hint has been set on an object.
     *
     * This overloads @f displayHintSet(DisplayHints, Hint) to accept a QObject
     * instance. This object is checked to see if it has a displayHint property
     * and if so, if that property has @p hint set.
     *
     * @param object The object to check.
     * @param hint The hint to check for.
     *
     * @return false if object is null, object has no displayHint property or
     *         the hint was not set. true if it has the property and the hint is
     *         set.
     */
    Q_INVOKABLE bool displayHintSet(QObject *object, Hint hint);

    /**
     * Static version of \f displayHintSet(DisplayHints, Hint) that can be
     * called from C++ code.
     */
    static bool isDisplayHintSet(DisplayHints values, Hint hint);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DisplayHint::DisplayHints)

#endif // ENUMS_H
