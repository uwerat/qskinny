/*
 *  SPDX-FileCopyrightText: 2017 by Marco Martin <mart@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KIRIGAMIPLUGINFACTORY_H
#define KIRIGAMIPLUGINFACTORY_H

#include "platformtheme.h"
#include <QObject>

#ifndef KIRIGAMI_BUILD_TYPE_STATIC
#include <kirigami2_export.h>
#endif

namespace Kirigami {

/**
 * @class KirigamiPluginFactory kirigamipluginfactory.h KirigamiPluginFactory
 *
 * This class is reimpleented by plugins to provide different implementations
 * of PlatformTheme
 */
#ifdef KIRIGAMI_BUILD_TYPE_STATIC
class KirigamiPluginFactory : public QObject
#else
class KIRIGAMI2_EXPORT KirigamiPluginFactory : public QObject
#endif
{
    Q_OBJECT

public:
    explicit KirigamiPluginFactory(QObject *parent = nullptr);
    ~KirigamiPluginFactory();

    /**
     * Creates an instance of PlatformTheme which can come out from
     * an implementation provided by a plugin
     * @param parent the parent object of the created PlatformTheme
     */
    virtual PlatformTheme *createPlatformTheme(QObject *parent) = 0;
};

}

QT_BEGIN_NAMESPACE
#define KirigamiPluginFactory_iid "org.kde.kirigami.KirigamiPluginFactory"
Q_DECLARE_INTERFACE(Kirigami::KirigamiPluginFactory, KirigamiPluginFactory_iid)
QT_END_NAMESPACE

#endif //KIRIGAMIPLUGINFACTORY_H
