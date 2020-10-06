/*
 *  SPDX-FileCopyrightText: 2015 Marco Martin <notmart@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.1
import "private"
import "templates" as T

/**
 * An item delegate for the primitive ListView component.
 *
 * It's intended to make all listviews look coherent.
 *
 * @inherit QtQuick.Item
 */
T.AbstractListItem {
    id: listItem

    background: DefaultListItemBackground {}
}
