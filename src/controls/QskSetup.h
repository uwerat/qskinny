/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SETUP_H
#define QSK_SETUP_H 1

#include "QskGlobal.h"
#include <QObject>
#include <qqml.h>
#include <memory>

class QskVirtualKeyboard;
class QskSkin;
class QskSkinlet;
class QskControl;
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
    enum Flag
    {
        DeferredUpdate          =  1 << 0,
        DeferredPolish          =  1 << 1,
        DeferredLayout          =  1 << 2,
        CleanupOnVisibility     =  1 << 3,

        PreferRasterForTextures =  1 << 4,

        DebugForceBackground    =  1 << 7
    };

    Q_ENUM( Flag )
    Q_DECLARE_FLAGS( Flags, Flag )

    static QskSetup * instance();

    Q_INVOKABLE void setControlFlags( Flags );
    Q_INVOKABLE void resetControlFlags();
    Q_INVOKABLE Flags controlFlags() const;

    Q_INVOKABLE void setControlFlag( Flag, bool on = true );
    Q_INVOKABLE void resetControlFlag( Flag );
    Q_INVOKABLE bool testControlFlag( Flag );

    Q_INVOKABLE QskSkin* setSkin( const QString& );
    Q_INVOKABLE QString skinName() const;

    QskSkin* skin();

    void setInputPanel( QskVirtualKeyboard* );
    QskVirtualKeyboard* inputPanel();

    void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QskGraphicProvider* graphicProvider( const QString& providerId ) const;

    static void setup();
    static void cleanup();

    QLocale inheritedLocale( const QObject* );
    void inheritLocale( QObject*, const QLocale& );

    static QskSetup* qmlAttachedProperties( QObject* );

Q_SIGNALS:
    void skinChanged( QskSkin* );
    void inputPanelChanged( QskVirtualKeyboard* );
    void controlFlagsChanged();

private:
    QskSetup();
    virtual ~QskSetup();

    virtual bool eventFilter( QObject*, QEvent* ) override final;

    static QskSetup* s_instance;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

QML_DECLARE_TYPEINFO( QskSetup, QML_HAS_ATTACHED_PROPERTIES )

inline QskSetup * QskSetup::instance()
{
    Q_ASSERT( s_instance );
    return s_instance;
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskSetup::Flags )
Q_DECLARE_METATYPE( QskSetup::Flags )

#endif
