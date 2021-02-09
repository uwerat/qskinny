/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SETUP_H
#define QSK_SETUP_H

#include "QskGlobal.h"
#include "QskQuickItem.h"

#include <qobject.h>
#include <memory>

class QskSkin;
class QQuickItem;
class QskGraphicProvider;

class QLocale;

#if defined( qskSetup )
#undef qskSetup
#endif

#define qskSetup QskSetup::instance()

class QSK_EXPORT QskSetup : public QObject
{
    Q_OBJECT

  public:

    static QskSetup* instance();

    void setItemUpdateFlags( QskQuickItem::UpdateFlags );
    void resetItemUpdateFlags();
    QskQuickItem::UpdateFlags itemUpdateFlags() const;

    void setItemUpdateFlag( QskQuickItem::UpdateFlag, bool on = true );
    void resetItemUpdateFlag( QskQuickItem::UpdateFlag );
    bool testItemUpdateFlag( QskQuickItem::UpdateFlag );

    QskSkin* setSkin( const QString& );
    QString skinName() const;

    QskSkin* skin();

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;

    static void setup();
    static void cleanup();

    QLocale inheritedLocale( const QObject* );
    void inheritLocale( QObject*, const QLocale& );

    static QskSetup* qmlAttachedProperties( QObject* );

  Q_SIGNALS:
    void skinChanged( QskSkin* );
    void itemUpdateFlagsChanged();

  private:
    QskSetup();
    ~QskSetup() override;

    bool eventFilter( QObject*, QEvent* ) override final;

    static QskSetup* s_instance;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline QskSetup* QskSetup::instance()
{
    Q_ASSERT( s_instance );
    return s_instance;
}

#endif
