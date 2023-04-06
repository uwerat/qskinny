/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LABEL_DATA_H
#define QSK_LABEL_DATA_H

#include "QskGlobal.h"
#include "QskIcon.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qvector.h>
#include <qmetatype.h>
#include <qdebug.h>

class QSK_EXPORT QskLabelData
{
    Q_GADGET

    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( QUrl iconSource READ iconSource WRITE setIconSource )

  public:
    QskLabelData() = default; 

    QskLabelData( const char* );
    QskLabelData( const QString& );
    QskLabelData( const QskIcon& );
    QskLabelData( const QString&, const QskIcon& );

    bool operator==( const QskLabelData& ) const noexcept;
    bool operator!=( const QskLabelData& ) const noexcept;

    void setText( const QString& );
    QString text() const noexcept;

    void setIconSource( const QUrl& );
    QUrl iconSource() const noexcept;

    void setIcon( const QskIcon& );
    QskIcon icon() const noexcept;

    QskHashValue hash( QskHashValue ) const;

  private:
    QString m_text;
    QskIcon m_icon;
};

Q_DECLARE_METATYPE( QskLabelData )

inline QString QskLabelData::text() const noexcept
{
    return m_text;
}

inline QskIcon QskLabelData::icon() const noexcept
{
    return m_icon;
}

inline QUrl QskLabelData::iconSource() const noexcept
{
    return m_icon.source();
}

inline bool QskLabelData::operator!=( const QskLabelData& other ) const noexcept
{
    return ( !( *this == other ) );
}

QSK_EXPORT QVector< QskLabelData > qskCreateLabelData( const QStringList& );

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskLabelData& );

#endif

#endif
