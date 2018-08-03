/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_OBJECT_COUNTER_H
#define QSK_OBJECT_COUNTER_H

#include "QskGlobal.h"

class QObject;
class QDebug;
class QskObjectCounterHook;

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
    friend class QskObjectCounterHook;

    void addObject( QObject* );
    void removeObject( QObject* );

    class Counter
    {
      public:
        Counter()
        {
            reset();
        }

        void reset()
        {
            created = destroyed = current = maximum = 0;
        }

        void increment()
        {
            created++;
            current++;

            if ( current > maximum )
                maximum = current;
        }

        void decrement()
        {
            destroyed++;
            current--;
        }

        int created;
        int destroyed;
        int current;
        int maximum;
    };

    Counter m_counter[ 2 ];
    const bool m_debugAtDestruction;
};

#ifndef QT_NO_DEBUG_STREAM
QSK_EXPORT QDebug operator<<( QDebug, const QskObjectCounter& );
#endif

#endif
