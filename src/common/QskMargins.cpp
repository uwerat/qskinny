#include "QskMargins.h"
#include <qvariant.h>

static void qskRegisterConverter()
{
    QMetaType::registerConverter< int, QskMargins >(
        []( int value ) { return QskMargins( value ); } );

    QMetaType::registerConverter< qreal, QskMargins >(
        []( qreal value ) { return QskMargins( value ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterConverter )

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline QskMargins qskInterpolateMargins(
    const QskMargins& m1, const QskMargins& m2, qreal progress )
{
    const qreal left = qskInterpolated( m1.left(), m2.left(), progress );
    const qreal top = qskInterpolated( m1.top(), m2.top(), progress );
    const qreal right = qskInterpolated( m1.right(), m2.right(), progress );
    const qreal bottom = qskInterpolated( m1.bottom(), m2.bottom(), progress );

    return QskMargins( left, top, right, bottom );
}

QskMargins QskMargins::interpolated(
    const QskMargins& to, qreal progress ) const noexcept
{
    return qskInterpolateMargins( *this, to, progress );
}

QVariant QskMargins::interpolate(
    const QskMargins& m1, const QskMargins& m2, qreal progress ) noexcept
{
    return QVariant::fromValue( qskInterpolateMargins( m1, m2, progress ) );
}

void QskMargins::setMarginsAt( Qt::Edges edges, qreal value ) noexcept
{
    if ( edges & Qt::LeftEdge )
        setLeft( value );

    if ( edges & Qt::TopEdge )
        setTop( value );

    if ( edges & Qt::RightEdge )
        setRight( value );

    if ( edges & Qt::BottomEdge )
        setBottom( value );
}

qreal QskMargins::marginAt( Qt::Edge edge ) const noexcept
{
    switch ( edge )
    {
        case Qt::LeftEdge:
            return left();

        case Qt::TopEdge:
            return top();

        case Qt::RightEdge:
            return right();

        case Qt::BottomEdge:
            return bottom();
    }

    return 0.0;
}

#include "moc_QskMargins.cpp"
