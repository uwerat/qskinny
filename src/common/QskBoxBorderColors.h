/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_BORDER_COLORS_H
#define QSK_BOX_BORDER_COLORS_H

#include "QskGradient.h"
#include "QskNamespace.h"

#include <qcolor.h>
#include <qmetatype.h>

class QSK_EXPORT QskBoxBorderColors
{
    Q_GADGET

    Q_PROPERTY( QskGradient left READ left WRITE setLeft )
    Q_PROPERTY( QskGradient top READ top WRITE setTop )
    Q_PROPERTY( QskGradient right READ right WRITE setRight )
    Q_PROPERTY( QskGradient bottom READ bottom WRITE setBottom )

  public:
    QskBoxBorderColors();

    QskBoxBorderColors( const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom );

    QskBoxBorderColors( Qt::GlobalColor );
    QskBoxBorderColors( QRgb );
    QskBoxBorderColors( const QColor& );
    QskBoxBorderColors( const QskGradient& );

    ~QskBoxBorderColors();

    bool operator==( const QskBoxBorderColors& ) const;
    bool operator!=( const QskBoxBorderColors& ) const;

    void setAlpha( int alpha );

    void setGradients( const QskGradient& );
    void setGradients( const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom );

    void setGradientAt( Qt::Edges, const QskGradient& );
    const QskGradient& gradientAt( Qt::Edge ) const;

    void setLeft( const QskGradient& );
    const QskGradient& left() const;

    void setTop( const QskGradient& );
    const QskGradient& top() const;

    void setRight( const QskGradient& );
    const QskGradient& right() const;

    void setBottom( const QskGradient& );
    const QskGradient& bottom() const;

    QskBoxBorderColors interpolated( const QskBoxBorderColors&, qreal value ) const;

    static QVariant interpolate( const QskBoxBorderColors&,
        const QskBoxBorderColors&, qreal ratio );

    QskHashValue hash( QskHashValue seed = 0 ) const;

    bool isMonochrome() const;
    bool isVisible() const;
    bool isValid() const;

  private:
    enum
    {
        // in order of Qt::Edge
        Top    = 0,
        Left   = 1,
        Right  = 2,
        Bottom = 3
    };
    QskGradient m_gradients[ 4 ];
};

inline QskBoxBorderColors::QskBoxBorderColors( Qt::GlobalColor color )
    : QskBoxBorderColors( QskGradient( QColor( color ) ) )
{
}

inline QskBoxBorderColors::QskBoxBorderColors( QRgb rgb )
    : QskBoxBorderColors( QskGradient( QColor::fromRgba( rgb ) ) )
{
}

inline QskBoxBorderColors::QskBoxBorderColors( const QColor& color )
    : QskBoxBorderColors( QskGradient( color ) )
{
}

inline bool QskBoxBorderColors::operator!=( const QskBoxBorderColors& other ) const
{
    return !( *this == other );
}

inline const QskGradient& QskBoxBorderColors::left() const
{
    return m_gradients[ Left ];
}

inline const QskGradient& QskBoxBorderColors::top() const
{
    return m_gradients[ Top ];
}

inline const QskGradient& QskBoxBorderColors::right() const
{
    return m_gradients[ Right ];
}

inline const QskGradient& QskBoxBorderColors::bottom() const
{
    return m_gradients[ Bottom ];
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskBoxBorderColors& );

#endif

Q_DECLARE_METATYPE( QskBoxBorderColors )

#endif
