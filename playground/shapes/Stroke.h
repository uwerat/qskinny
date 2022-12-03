/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QMetaType>
#include <QColor>

class QPen;

class Stroke
{
    Q_GADGET

    Q_PROPERTY( QColor color READ color WRITE setColor )

    Q_PROPERTY( LineStyle lineStyle READ lineStyle WRITE setLineStyle )
    Q_PROPERTY( qreal width READ width WRITE setWidth )
    Q_PROPERTY( JoinStyle joinStyle READ joinStyle WRITE setJoinStyle )
    Q_PROPERTY( int miterLimit READ miterLimit WRITE setMiterLimit )
    Q_PROPERTY( CapStyle capStyle READ capStyle WRITE setCapStyle )

    Q_PROPERTY( bool cosmetic READ isCosmetic WRITE setCosmetic )

  public:
    enum LineStyle
    {
        SolidLine = Qt::SolidLine,
        DashLine = Qt::DashLine
    };
    Q_ENUM( LineStyle )

    enum JoinStyle
    {
        MiterJoin = Qt::MiterJoin,
        BevelJoin = Qt::BevelJoin,
        RoundJoin = Qt::RoundJoin
    };
    Q_ENUM( JoinStyle )

    enum CapStyle
    {
        FlatCap = Qt::FlatCap,
        SquareCap = Qt::SquareCap,
        RoundCap = Qt::RoundCap
    };
    Q_ENUM( CapStyle )

    Stroke() noexcept = default;
    Stroke( const QColor&, qreal width = 1.0 ) noexcept;
    Stroke( const QPen& ) noexcept;

    bool operator==( const Stroke& ) const noexcept;
    bool operator!=( const Stroke& ) const noexcept;

    void setColor( const QColor& ) noexcept;
    QColor color() const noexcept;

    void setWidth( qreal ) noexcept;
    qreal width() const noexcept;

    void setLineStyle( LineStyle );
    LineStyle lineStyle() const noexcept;

    void setCapStyle( CapStyle ) noexcept;
    CapStyle capStyle() const noexcept;

    void setJoinStyle( JoinStyle ) noexcept;
    JoinStyle joinStyle() const noexcept;

    void setMiterLimit( int ) noexcept;
    int miterLimit() const noexcept;

    void setCosmetic( bool ) noexcept;
    bool isCosmetic() const noexcept;

    bool isVisible() const;

    QPen toPen() const;

  private:
    qreal m_width = 1.0;

    int m_miterLimit = 2;

    QColor m_color = Qt::black;

    LineStyle m_lineStyle = SolidLine;
    JoinStyle m_joinStyle = BevelJoin;
    CapStyle m_capStyle = SquareCap;

    bool m_cosmetic = false;
};

inline Stroke::Stroke( const QColor& color, qreal width ) noexcept
    : m_width( width )
    , m_color( color )
{
}

inline QColor Stroke::color() const noexcept
{
    return m_color;
}

inline qreal Stroke::width() const noexcept
{
    return m_width;
}

inline Stroke::LineStyle Stroke::lineStyle() const noexcept
{
    return m_lineStyle;
}

inline Stroke::CapStyle Stroke::capStyle() const noexcept
{
    return m_capStyle;
}

inline Stroke::JoinStyle Stroke::joinStyle() const noexcept
{
    return m_joinStyle;
}

inline int Stroke::miterLimit() const noexcept
{
    return m_miterLimit;
}

inline bool Stroke::isCosmetic() const noexcept
{
    return m_cosmetic;
}

inline bool Stroke::operator==( const Stroke& other ) const noexcept
{
    return ( m_width == other.m_width )
        && ( m_miterLimit == other.m_miterLimit )
        && ( m_color == other.m_color )
        && ( m_lineStyle == other.m_lineStyle )
        && ( m_joinStyle == other.m_joinStyle )
        && ( m_capStyle == other.m_capStyle )
        && ( m_cosmetic == other.m_cosmetic );
}

inline bool Stroke::operator!=( const Stroke& other ) const noexcept
{
    return !( *this == other );
}
