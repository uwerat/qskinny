#include "QskLayoutHint.h"
#include "QskLayoutConstraint.h"
#include <qnamespace.h>

QskLayoutHint::QskLayoutHint()
    : QskLayoutHint( 0.0, 0.0, QskLayoutConstraint::unlimited )
{
}

QskLayoutHint::QskLayoutHint( qreal minimum, qreal preferred, qreal maximum )
    : m_minimum( minimum )
    , m_preferred( preferred )
    , m_maximum( maximum )
{
}

qreal QskLayoutHint::size( int which ) const
{
    switch( which )
    {
        case Qt::MinimumSize:
            return m_minimum;

        case Qt::PreferredSize:
            return m_preferred;

        case Qt::MaximumSize:
            return m_maximum;

        default:
            return 0.0;
    }
}

void QskLayoutHint::setSize( int which, qreal size )
{
    switch (which)
    {
        case Qt::MinimumSize:
            m_minimum = size;
            break;

        case Qt::PreferredSize:
            m_preferred = size;
            break;

        case Qt::MaximumSize:
            m_maximum = size;
            break;

        default:
            break;
    }
}

void QskLayoutHint::intersect( const QskLayoutHint& other )
{
    m_minimum = qMax( m_minimum, other.m_minimum );
    m_preferred = qMax( m_preferred, other.m_preferred );

    if ( other.m_maximum != QskLayoutConstraint::unlimited )
    {
        if ( m_maximum == QskLayoutConstraint::unlimited )
            m_maximum = other.m_maximum;
        else
            m_maximum = qMax( m_maximum, other.m_maximum );
    }

    normalize();
}

void QskLayoutHint::normalize()
{
    m_minimum = qMax( m_minimum, qreal( 0.0 ) );
    m_maximum = qMax( m_minimum, m_maximum );
    m_preferred = qBound( m_minimum, m_preferred, m_maximum );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

static inline QString qskHintValueString( qreal value )
{
    if ( value >= QskLayoutConstraint::unlimited )
        return QStringLiteral( "unlimited" );
    else
        return QString::number( value );
}

QDebug operator<<( QDebug debug, const QskLayoutHint& hint )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "LayoutHint" << "( " 
        << qskHintValueString( hint.minimum() ) << ", "
        << qskHintValueString( hint.preferred() ) << ", "
        << qskHintValueString( hint.maximum() ) << " )";

    return debug;
}

#endif
