/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSetup.h"

QskSetup* QskSetup::s_instance = nullptr;

static inline bool qskHasEnvironment( const char* env )
{
    bool ok;

    const int value = qEnvironmentVariableIntValue( env, &ok );
    if ( ok )
        return value != 0;

    // All other strings are true, apart from "false"
    auto result = qgetenv( env );
    return !result.isEmpty() && result != "false";
}

static inline const QskItem::UpdateFlags qskEnvironmentUpdateFlags()
{
    QskItem::UpdateFlags flags;

    if ( qskHasEnvironment( "QSK_PREFER_RASTER" ) )
        flags |= QskItem::PreferRasterForTextures;

    if ( qskHasEnvironment( "QSK_FORCE_BACKGROUND" ) )
        flags |= QskItem::DebugForceBackground;

    return flags;
}

static inline QskItem::UpdateFlags qskDefaultUpdateFlags()
{
    static QskItem::UpdateFlags flags;

    if ( flags == 0 )
    {
        flags |= QskItem::DeferredUpdate;
        flags |= QskItem::DeferredPolish;
        flags |= QskItem::DeferredLayout;
        flags |= QskItem::CleanupOnVisibility;
        flags |= qskEnvironmentUpdateFlags();
    }

    return flags;
}

static void qskApplicationHook()
{
    QskSetup::setup();
    qAddPostRoutine( QskSetup::cleanup );
}

Q_CONSTRUCTOR_FUNCTION( qskApplicationHook )

class QskSetup::PrivateData
{
  public:
    PrivateData()
        : itemUpdateFlags( qskDefaultUpdateFlags() )
    {
    }

    QskItem::UpdateFlags itemUpdateFlags;
};

QskSetup::QskSetup()
    : m_data( new PrivateData() )
{
}

QskSetup::~QskSetup()
{
    s_instance = nullptr; // we might be destroyed from Qml !
}

void QskSetup::setup()
{
    if ( s_instance == nullptr )
        s_instance = new QskSetup();
}

void QskSetup::cleanup()
{
    delete s_instance;
    s_instance = nullptr;
}

void QskSetup::setItemUpdateFlags( QskItem::UpdateFlags flags )
{
    if ( m_data->itemUpdateFlags != flags )
    {
        m_data->itemUpdateFlags = flags;
        Q_EMIT itemUpdateFlagsChanged();
    }
}

QskItem::UpdateFlags QskSetup::itemUpdateFlags() const
{
    return m_data->itemUpdateFlags;
}

void QskSetup::resetItemUpdateFlags()
{
    setItemUpdateFlags( qskDefaultUpdateFlags() );
}

void QskSetup::setItemUpdateFlag( QskItem::UpdateFlag flag, bool on )
{
    if ( m_data->itemUpdateFlags.testFlag( flag ) != on )
    {
        if ( on )
            m_data->itemUpdateFlags |= flag;
        else
            m_data->itemUpdateFlags &= ~flag;

        Q_EMIT itemUpdateFlagsChanged();
    }
}

void QskSetup::resetItemUpdateFlag( QskItem::UpdateFlag flag )
{
    setItemUpdateFlag( flag, flag & qskDefaultUpdateFlags() );
}

bool QskSetup::testItemUpdateFlag( QskItem::UpdateFlag flag )
{
    return m_data->itemUpdateFlags.testFlag( flag );
}

#include "moc_QskSetup.cpp"
