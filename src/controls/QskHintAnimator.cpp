/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskHintAnimator.h"
#include "QskAnimationHint.h"
#include "QskControl.h"
#include "QskEvent.h"

#include <QObject>

#include <map>
#include <vector>
#include <algorithm>

static inline QVariant qskAdjustedValue(
    QskAspect::Aspect aspect, const QVariant& value )
{
#if 1
    if( value.type() == QVariant::Double )
    {
        if ( aspect.metricPrimitive() != QskAspect::Position )
        {
            // all beside QskAspect::Position are real metrics,
            // that will be aligned to the resolution of the paint device
            // so we can avoid pointless operations by rounding
            return qRound( 2.0 * value.toReal() ) / 2.0;
        }
    }
#endif

    return value;
}


QskHintAnimator::QskHintAnimator()
{
}

QskHintAnimator::~QskHintAnimator()
{
}

void QskHintAnimator::setAspect( QskAspect::Aspect aspect )
{
    m_aspect = aspect;
}

void QskHintAnimator::setControl( QskControl* control )
{
    m_control = control;
}

void QskHintAnimator::advance( qreal progress )
{
    const QVariant oldValue = currentValue();

    Inherited::advance( progress );

    setCurrentValue( qskAdjustedValue( m_aspect, currentValue() ) );

    if ( m_control && ( currentValue() != oldValue ) )
    {
        if ( m_aspect.type() == QskAspect::Metric )
        {
            m_control->resetImplicitSize();
            m_control->polish();
        }

        m_control->update();
    }
}

namespace
{
    class AnimatorGuard final : public QObject
    {
        Q_OBJECT

    public:
        AnimatorGuard()
        {
            QskAnimator::addCleanupHandler( this,
                SLOT( cleanup() ), Qt::QueuedConnection );
        }

        void registerTable( QskHintAnimatorTable* table )
        {
            auto it = std::lower_bound( m_tables.begin(), m_tables.end(), table );
            if ( it == m_tables.end() || *it != table )
                m_tables.insert( it, table );
        }

        void unregisterTable( QskHintAnimatorTable* table )
        {
            auto it = std::lower_bound( m_tables.begin(), m_tables.end(), table );
            if ( it != m_tables.end() )
                m_tables.erase( it );
        }

    private Q_SLOTS:
        void cleanup()
        {
            for( auto it = m_tables.begin(); it != m_tables.end(); )
            {
                if ( ( *it )->cleanup() )
                    it = m_tables.erase( it );
                else
                    ++it;
            }
        }

    private:
        // a vector as iteration is more important than insertion
        std::vector< QskHintAnimatorTable* > m_tables;
    };

    Q_GLOBAL_STATIC( AnimatorGuard, qskAnimatorGuard )
}

class QskHintAnimatorTable::PrivateData
{
public:
    // we won't have many entries, so we prefer less memory over
    // using a hash table
    std::map< QskAspect::Aspect, QskHintAnimator > map;
};

QskHintAnimatorTable::QskHintAnimatorTable():
    m_data( nullptr )
{
}

QskHintAnimatorTable::~QskHintAnimatorTable()
{
    qskAnimatorGuard->unregisterTable( this );

    if ( m_data )
        delete m_data;
}

void QskHintAnimatorTable::start( QskControl* control,
    QskAspect::Aspect aspect, QskAnimationHint animationHint,
    const QVariant& from, const QVariant& to )
{
    if ( m_data == nullptr )
    {
        m_data = new PrivateData();
        qskAnimatorGuard->registerTable( this );
    }

    auto it = m_data->map.find( aspect );
    if ( it == m_data->map.end() )
        it = m_data->map.emplace( aspect, QskHintAnimator() ).first;

    auto& animator = it->second;

    animator.setAspect( aspect );
    animator.setStartValue( from );
    animator.setEndValue( to );

    animator.setDuration( animationHint.duration );
    animator.setEasingCurve( animationHint.type );

    animator.setControl( control );
    animator.setWindow( control->window() );

    animator.start();

    QskAnimatorEvent event( aspect, QskAnimatorEvent::Started );
    QCoreApplication::sendEvent( control, &event );
}

const QskHintAnimator* QskHintAnimatorTable::animator( QskAspect::Aspect aspect ) const
{
    if ( m_data == nullptr )
        return nullptr;

    auto it = m_data->map.find( aspect );
    if ( it == m_data->map.end() )
        return nullptr;

    return &( it->second );
}

QVariant QskHintAnimatorTable::currentValue( QskAspect::Aspect aspect ) const
{
    if ( m_data )
    {
        const auto it = m_data->map.find( aspect );
        if ( it != m_data->map.cend() )
        {
            const auto& animator = it->second;
            if ( animator.isRunning() )
                return animator.currentValue();
        }
    }

    return QVariant();
}

bool QskHintAnimatorTable::cleanup()
{
    if ( m_data == nullptr )
        return true;

    for( auto it = m_data->map.begin(); it != m_data->map.end(); )
    {
        // remove all terminated animators
        if ( !it->second.isRunning() )
        {
            auto control = it->second.control();
            auto aspect = it->first;

            it = m_data->map.erase( it );

            if ( control )
            {
                QskAnimatorEvent event( aspect, QskAnimatorEvent::Terminated );
                QCoreApplication::sendEvent( control, &event );
            }
        }
        else
        {
            ++it;
        }
    }

    if ( m_data->map.empty() )
    {
        delete m_data;
        m_data = nullptr;

        return true;
    }

    return false;
}

#include "QskHintAnimator.moc"
