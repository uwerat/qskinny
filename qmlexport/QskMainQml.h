/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SETUP_QML_H
#define QSK_SETUP_QML_H

#include "QskQmlGlobal.h"

#include <QskSetup.h>

#include <qobject.h>
#include <qqmllist.h>

class QskSetupFlagsQml : public QskQuickItem::UpdateFlags
{
    Q_GADGET

  public:
    template< typename... Args >
    QskSetupFlagsQml( Args&&... args )
        : QskQuickItem::UpdateFlags( std::forward< Args >( args )... )
    {
    }

    Q_INVOKABLE int valueOf() const { return int( *this ); }
};

class QskMain : public QObject
{
  public:
    Q_OBJECT

    Q_PRIVATE_PROPERTY( setup(), QString skin READ skinName
        WRITE setSkin NOTIFY skinChanged )

    Q_PROPERTY( QStringList skinList READ skinList NOTIFY skinListChanged )

    Q_PRIVATE_PROPERTY( setup(), QskSetupFlagsQml itemUpdateFlags
        READ itemUpdateFlags WRITE setItemUpdateFlags NOTIFY itemUpdateFlagsChanged )

    Q_PROPERTY( QQmlListProperty< QObject > data READ data )
    Q_CLASSINFO( "DefaultProperty", "data" )

  public:
    QskMain( QObject* parent = nullptr );

    QStringList skinList() const;
    QQmlListProperty< QObject > data();

  Q_SIGNALS:
    void skinListChanged(); // never emitted
    void skinChanged();
    void inputPanelChanged();
    void itemUpdateFlagsChanged();

  private:
    static inline QskSetup* setup() { return QskSetup::instance(); }

    QObjectList m_data;
};

Q_DECLARE_METATYPE( QskSetupFlagsQml )

#endif
