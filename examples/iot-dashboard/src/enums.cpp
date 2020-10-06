/*
 *  SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "enums.h"

#include "moc_enums.cpp"

#include <QVariant>

bool DisplayHint::displayHintSet(DisplayHints values, Hint hint)
{
    return isDisplayHintSet(values, hint);
}

bool DisplayHint::displayHintSet(QObject* object, DisplayHint::Hint hint)
{
    if (!object) {
        return false;
    }

    auto property = object->property("displayHint");
    if (property.isValid()) {
        return isDisplayHintSet(DisplayHints{property.toInt()}, hint);
    } else {
        return false;
    }
}

bool DisplayHint::isDisplayHintSet(DisplayHint::DisplayHints values, DisplayHint::Hint hint)
{
    if (hint == DisplayHint::AlwaysHide && (values & DisplayHint::KeepVisible)) {
        return false;
    }

    return values & hint;
}
