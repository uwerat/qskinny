/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRangeControl.h"
#include "QskFunctions.h"
#include "QskIntervalF.h"

QSK_SYSTEM_STATE( QskRangeControl, ReadOnly, ( QskAspect::FirstSystemState << 1 ) )

class QskRangeControl::PrivateData
{
  public:
    PrivateData()
        : minimum( 0.0 )
        , maximum( 1.0 )
        , value( 0.0 )
        , stepSize( 0.1 )
        , pageSize( 1 )
        , snap( false )
    {
    }

    qreal minimum;
    qreal maximum;
    qreal value;
    qreal stepSize;
    int pageSize;
    bool snap : 1;
};

QskRangeControl::QskRangeControl( QQuickItem* parent )
    : QskControl( parent )
    , m_data( new PrivateData() )
{
    setFocusPolicy( Qt::StrongFocus );
    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
}

QskRangeControl::~QskRangeControl()
{
}

void QskRangeControl::setMinimum( qreal minimum )
{
    if ( m_data->minimum == minimum )
        return;

    const auto oldRange = range();

    m_data->minimum = minimum;
    Q_EMIT minimumChanged( minimum );

    if ( isComponentComplete() )
        adjustRangeAndValue( false );

    const auto newRange = range();
    if ( oldRange != newRange )
        Q_EMIT rangeChanged( newRange );

    update();
}

qreal QskRangeControl::minimum() const
{
    return m_data->minimum;
}

void QskRangeControl::setMaximum( qreal maximum )
{
    if ( m_data->maximum == maximum )
        return;

    const auto oldRange = range();

    m_data->maximum = maximum;
    Q_EMIT maximumChanged( maximum );

    if ( isComponentComplete() )
        adjustRangeAndValue( true );

    const auto newRange = range();
    if ( oldRange != newRange )
        Q_EMIT rangeChanged( newRange );

    update();
}

qreal QskRangeControl::maximum() const
{
    return m_data->maximum;
}

void QskRangeControl::setInterval( qreal min, qreal max )
{
    if ( max < min )
        max = min;

    if ( min == m_data->minimum && max == m_data->maximum )
        return;

    const auto oldMin = m_data->minimum;
    const auto oldMax = m_data->maximum;

    m_data->minimum = min;
    m_data->maximum = max;

    if ( isComponentComplete() )
        adjustRangeAndValue( false );

    if ( m_data->minimum != oldMin )
        Q_EMIT minimumChanged( m_data->minimum );

    if ( m_data->maximum != oldMax )
        Q_EMIT maximumChanged( m_data->maximum );

    const auto newRange = range();
    if ( newRange != oldMax - oldMin )
        Q_EMIT rangeChanged( newRange );

    update();
}

void QskRangeControl::setInterval( const QskIntervalF& interval )
{
    setInterval( interval.lowerBound(), interval.upperBound() );
}

QskIntervalF QskRangeControl::interval() const
{
    return QskIntervalF( m_data->minimum, m_data->maximum );
}

void QskRangeControl::adjustRangeAndValue( bool increasing )
{
    if ( m_data->maximum < m_data->minimum )
    {
        if ( increasing )
        {
            m_data->minimum = m_data->maximum;
            Q_EMIT minimumChanged( m_data->minimum );
        }
        else
        {
            m_data->maximum = m_data->minimum;
            Q_EMIT maximumChanged( m_data->maximum );
        }
    }

    qreal newValue = m_data->value;

    if ( m_data->snap && m_data->stepSize != 0.0 )
        newValue = qRound( newValue / m_data->stepSize ) * m_data->stepSize;

    newValue = qBound( m_data->minimum, newValue, m_data->maximum );

    if ( newValue != m_data->value )
    {
        m_data->value = newValue;
        Q_EMIT valueChanged( newValue );
    }
}

qreal QskRangeControl::range() const
{
    return m_data->maximum - m_data->minimum;
}

qreal QskRangeControl::position() const
{
    return ( value() - minimum() ) / range();
}

qreal QskRangeControl::fixupValue( qreal value ) const
{
    return value;
}

void QskRangeControl::setValue( qreal value )
{
    if ( isComponentComplete() )
    {
        if ( m_data->snap && m_data->stepSize != 0.0 )
            value = qRound( value / m_data->stepSize ) * m_data->stepSize;

        value = qBound( m_data->minimum, value, m_data->maximum );
    }

    value = fixupValue( value );

    if ( !qskFuzzyCompare( value, m_data->value ) )
    {
        m_data->value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

qreal QskRangeControl::value() const
{
    return m_data->value;
}

void QskRangeControl::setStepSize( qreal stepSize )
{
    if ( qskFuzzyCompare( m_data->stepSize, stepSize ) )
        return;

    m_data->stepSize = stepSize;
    Q_EMIT stepSizeChanged( stepSize );

    update();
}

qreal QskRangeControl::stepSize() const
{
    return m_data->stepSize;
}

void QskRangeControl::setPageSize( int pageSize )
{
    if ( m_data->pageSize == pageSize )
        return;

    m_data->pageSize = pageSize;
    Q_EMIT pageSizeChanged( pageSize );

    update();
}

int QskRangeControl::pageSize() const
{
    return m_data->pageSize;
}

void QskRangeControl::setSnap( bool snap )
{
    if ( m_data->snap == snap )
        return;

    m_data->snap = snap;
    Q_EMIT snapChanged( snap );

    if ( snap )
    {
        // Re-align value
    }
}

bool QskRangeControl::snap() const
{
    return m_data->snap;
}

void QskRangeControl::setReadOnly( bool readOnly )
{
    if ( readOnly == isReadOnly() )
        return;

    setSkinStateFlag( ReadOnly, readOnly );

    // we are killing user settings here !!
    setFocusPolicy( readOnly ? Qt::NoFocus : Qt::StrongFocus );
    setAcceptedMouseButtons( readOnly ? Qt::NoButton : Qt::LeftButton );
    setWheelEnabled( !readOnly );

    Q_EMIT readOnlyChanged( readOnly );
}

bool QskRangeControl::isReadOnly() const
{
    return skinState() & ReadOnly;
}

void QskRangeControl::stepUp()
{
    setValue( m_data->value + m_data->stepSize );
}

void QskRangeControl::stepDown()
{
    setValue( m_data->value - m_data->stepSize );
}

void QskRangeControl::pageUp()
{
    setValue( m_data->value + m_data->stepSize * m_data->pageSize );
}

void QskRangeControl::pageDown()
{
    setValue( m_data->value - m_data->stepSize * m_data->pageSize );
}

void QskRangeControl::keyPressEvent( QKeyEvent* event )
{
    if ( !isReadOnly() )
    {
        if ( event->key() == Qt::Key_Up || event->matches( QKeySequence::MoveToNextChar ) )
        {
            stepUp();
            return;
        }

        if ( event->key() == Qt::Key_Down || event->matches( QKeySequence::MoveToPreviousChar ) )
        {
            stepDown();
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

#ifndef QT_NO_WHEELEVENT

void QskRangeControl::wheelEvent( QWheelEvent* event )
{
    if ( isReadOnly() )
        return;

#if QT_VERSION < 0x050e00
    const int wheelDelta = event->delta();
#else
    const auto delta = event->angleDelta();
    const int wheelDelta = ( qAbs( delta.x() ) > qAbs( delta.y() ) )
        ? delta.x() : delta.y();
#endif

    const int steps = wheelDelta / QWheelEvent::DefaultDeltasPerStep;
    setValue( m_data->value + steps * m_data->stepSize );
}

#endif

void QskRangeControl::componentComplete()
{
    Inherited::componentComplete();
    adjustRangeAndValue( true );
}

#include "moc_QskRangeControl.cpp"
