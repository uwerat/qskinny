/*
 *  SPDX-FileCopyrightText: 2017 by Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kirigamipluginfactory.h"
#include "platformtheme.h"

#include <QDebug>


namespace Kirigami {

KirigamiPluginFactory::KirigamiPluginFactory(QObject *parent)
    : QObject(parent)
{
}

KirigamiPluginFactory::~KirigamiPluginFactory()
{
}

}

#include "moc_kirigamipluginfactory.cpp"
