/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoundedValueInput.h"
#include "QskFunctions.h"

QskBoundedValueInput::QskBoundedValueInput( QQuickItem* parent )
    : QskBoundedInput( parent )
{
}

QskBoundedValueInput::~QskBoundedValueInput()
{
}

void QskBoundedValueInput::alignInput()
{
    auto value = alignedValue( m_value );
    value = fixupValue( value );

    setValueInternal( value );
}

qreal QskBoundedValueInput::fixupValue( qreal value ) const
{
    return value;
}

void QskBoundedValueInput::setValueAsRatio( qreal ratio )
{
    ratio = qBound( 0.0, ratio, 1.0 );
    setValue( minimum() + ratio * boundaryLength() );
}

qreal QskBoundedValueInput::valueAsRatio() const
{
    return valueAsRatio( m_value );
}

void QskBoundedValueInput::setValue( qreal value )
{
    if ( isComponentComplete() )
    {
        value = alignedValue( value );
        value = fixupValue( value );
    }

    setValueInternal( value );
}

qreal QskBoundedValueInput::value() const
{
    return m_value;
}

void QskBoundedValueInput::increment( qreal offset )
{
    setValue( m_value + offset );
}

void QskBoundedValueInput::setValueInternal( qreal value )
{
    if ( !qskFuzzyCompare( value, m_value ) )
    {
        m_value = value;
        Q_EMIT valueChanged( value );

        update();
    }
}

#include "moc_QskBoundedValueInput.cpp"
