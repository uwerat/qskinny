/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSetup.h"

extern void qskUpdateItemFlags();

namespace
{
    inline bool hasEnvironment( const char* env )
    {
        bool ok;

        const int value = qEnvironmentVariableIntValue( env, &ok );
        if ( ok )
            return value != 0;

        // All other strings are true, apart from "false"
        auto result = qgetenv( env );
        return !result.isEmpty() && result != "false";
    }

    inline const QskItem::UpdateFlags environmentUpdateFlags()
    {
        QskItem::UpdateFlags flags;

        if ( !hasEnvironment( "QSK_PREFER_FBO_PAINTING" ) )
            flags |= QskItem::PreferRasterForTextures;

        if ( hasEnvironment( "QSK_FORCE_BACKGROUND" ) )
            flags |= QskItem::DebugForceBackground;

        return flags;
    }

    inline QskItem::UpdateFlags defaultUpdateFlags()
    {
        static QskItem::UpdateFlags flags;

        if ( flags == 0 )
        {
            flags |= QskItem::DeferredUpdate;
            flags |= QskItem::DeferredPolish;
            flags |= QskItem::DeferredLayout;
            flags |= QskItem::CleanupOnVisibility;
            flags |= environmentUpdateFlags();
        }

        return flags;
    }

    inline void propagateFlags()
    {
        qskUpdateItemFlags();
    }
}

static QskItem::UpdateFlags qskUpdateFlags = defaultUpdateFlags();

void QskSetup::setUpdateFlags( QskItem::UpdateFlags flags )
{
    if ( qskUpdateFlags != flags )
    {
        qskUpdateFlags = flags;
        propagateFlags();
    }
}

QskItem::UpdateFlags QskSetup::updateFlags()
{
    return qskUpdateFlags;
}

void QskSetup::resetUpdateFlags()
{
    setUpdateFlags( defaultUpdateFlags() );
}

void QskSetup::setUpdateFlag( QskItem::UpdateFlag flag, bool on )
{
    if ( qskUpdateFlags.testFlag( flag ) != on )
    {
        if ( on )
            qskUpdateFlags |= flag;
        else
            qskUpdateFlags &= ~flag;

        propagateFlags();
    }
}

void QskSetup::resetUpdateFlag( QskItem::UpdateFlag flag )
{
    setUpdateFlag( flag, flag & defaultUpdateFlags() );
}

bool QskSetup::testUpdateFlag( QskItem::UpdateFlag flag )
{
    return qskUpdateFlags.testFlag( flag );
}
