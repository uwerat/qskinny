/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ICON_H
#define QSK_ICON_H

#include "QskGlobal.h"
#include "QskGraphic.h"

#include <qurl.h>
#include <qmetatype.h>
#include <qshareddata.h>

/*
   Most controls offer 2 way to pass an icon:

    - URL
    - QskGraphic

   For the vast majority of the application icons URLs are used
   that will be loaded at runtime by a QskGraphicProvider.
   ( QML code always uses URLs )

   QskIcon implements a lazy loading strategy, that avoids unsatisfying
   startup performance from loading to many icons in advance.
 */

class QSK_EXPORT QskIcon
{
    Q_GADGET

    Q_PROPERTY( QskGraphic graphic READ graphic WRITE setGraphic )
    Q_PROPERTY( QUrl source READ source WRITE setSource )

  public:
    QskIcon();

    QskIcon( const QString& );
    QskIcon( const QUrl& );
    QskIcon( const QskGraphic& );

    bool operator==( const QskIcon& ) const noexcept;
    bool operator!=( const QskIcon& ) const noexcept;

    bool isNull() const;

    void setSource( const QUrl& );
    QUrl source() const noexcept;

    void setGraphic( const QskGraphic& );
    QskGraphic graphic() const;

    QskGraphic maybeGraphic() const noexcept;

    QskHashValue hash( QskHashValue ) const;

  private:
    QUrl m_source;

    class Data : public QSharedData
    {
      public:
        ~Data() { delete graphic; }
        QskGraphic* graphic = nullptr;
    };

    mutable QExplicitlySharedDataPointer< Data > m_data;
};

Q_DECLARE_METATYPE( QskIcon )

inline QskIcon::QskIcon( const QString& source )
    : QskIcon( QUrl( source ) )
{
}

inline bool QskIcon::isNull() const
{
    if ( m_data->graphic )
        return m_data->graphic->isNull();

    return m_source.isEmpty();
}

inline QUrl QskIcon::source() const noexcept
{
    return m_source;
}

inline QskGraphic QskIcon::maybeGraphic() const noexcept
{
    return m_data->graphic ? *m_data->graphic : QskGraphic();
}

inline bool QskIcon::operator!=( const QskIcon& other ) const noexcept
{
    return ( !( *this == other ) );
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskIcon& );

#endif

#endif
