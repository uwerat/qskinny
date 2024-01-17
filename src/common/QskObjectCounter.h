/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_OBJECT_COUNTER_H
#define QSK_OBJECT_COUNTER_H

#include "QskGlobal.h"
#include <memory>

class QObject;

class QSK_EXPORT QskObjectCounter
{
  public:
    enum ObjectType
    {
        Objects,
        Items
    };

    QskObjectCounter( bool debugAtDestruction = false );
    ~QskObjectCounter();

    void setActive( bool );
    bool isActive() const;

    void reset();

    int created( ObjectType = Objects ) const;
    int destroyed( ObjectType = Objects ) const;
    int current( ObjectType = Objects ) const;
    int maximum( ObjectType = Objects ) const;

    void debugStatistics( QDebug, ObjectType = Objects ) const;
    void dump() const;

  private:
    Q_DISABLE_COPY( QskObjectCounter )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskObjectCounter& );

#endif

#endif
