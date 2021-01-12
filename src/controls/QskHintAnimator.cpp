/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskHintAnimator.h"
#include "QskAnimationHint.h"
#include "QskControl.h"
#include "QskEvent.h"

#include <qobject.h>
#include <qthread.h>

#include <algorithm>
#include <map>
#include <vector>

#define ALIGN_VALUES 0

#if ALIGN_VALUES

static inline qreal qskAligned05( qreal value )
{
    // aligned to 0.5
    return qRound( 2.0 * value ) / 2.0;
}

static inline QSizeF qskAligned05( const QSizeF& size )
{
    return QSizeF( qskAligned05( size.width() ), qskAligned05( size.height() ) );
}

static inline QskMargins qskAligned05( const QskMargins& margins )
{
    const qreal left = qskAligned05( margins.left() );
    const qreal top = qskAligned05( margins.top() );
    const qreal right = qskAligned05( margins.right() );
    const qreal bottom = qskAligned05( margins.bottom() );

    return QskMargins( left, top, right, bottom );
}

static inline QVariant qskAligned05( const QVariant& value )
{
    if ( value.canConvert< QskBoxBorderMetrics >() )
    {
        auto metrics = value.value< QskBoxBorderMetrics >();

        if ( metrics.sizeMode() == Qt::AbsoluteSize )
        {
            metrics.setWidths( qskAligned05( metrics.widths() ) );
            return QVariant::fromValue( metrics );
        }
    }
    else if ( value.canConvert< QskBoxShapeMetrics >() )
    {
        auto metrics = value.value< QskBoxShapeMetrics >();
        if ( metrics.sizeMode() == Qt::AbsoluteSize )
        {
            for ( int i = Qt::TopLeftCorner; i <= Qt::BottomRightCorner; i++ )
            {
                const auto corner = static_cast< Qt::Corner >( i );
                metrics.setRadius( corner, qskAligned05( metrics.radius( corner ) ) );
            }

            return QVariant::fromValue( metrics );
        }
    }
    else if ( value.canConvert< QskMargins >() )
    {
        const auto margins = value.value< QskMargins >();
        return QVariant::fromValue( qskAligned05( margins ) );
    }

    return value;
}

#endif

static inline bool qskCheckReceiverThread( const QObject* receiver )
{
    /*
        QskInputPanelSkinlet changes the skin state, what leads to
        sending events from the wrong thread. Until we have fixed it
        let's block sending the event to avoid running into assertions
        in QCoreApplication::sendEvent
     */

    const QThread* thread = receiver->thread();
    if ( thread == nullptr )
        return true;

    return ( thread == QThread::currentThread() );
}

QskHintAnimator::QskHintAnimator()
{
}

QskHintAnimator::~QskHintAnimator()
{
}

void QskHintAnimator::setAspect( QskAspect aspect )
{
    m_aspect = aspect;
}

void QskHintAnimator::setUpdateFlags( QskAnimationHint::UpdateFlags flags )
{
    m_updateFlags = flags;
}

void QskHintAnimator::setControl( QskControl* control )
{
    m_control = control;
}

void QskHintAnimator::advance( qreal progress )
{
    const QVariant oldValue = currentValue();

    Inherited::advance( progress );

#if ALIGN_VALUES
    setCurrentValue( qskAligned05( currentValue() ) );
#endif

    if ( m_control && ( currentValue() != oldValue ) )
    {
        if ( m_updateFlags == QskAnimationHint::UpdateAuto )
        {
            if ( m_aspect.isMetric() )
            {
                m_control->resetImplicitSize();

                if ( !m_control->childItems().isEmpty() )
                    m_control->polish();
            }

            m_control->update();
        }
        else
        {
            if ( m_updateFlags & QskAnimationHint::UpdateSizeHint )
                m_control->resetImplicitSize();

            if ( m_updateFlags & QskAnimationHint::UpdatePolish )
                m_control->polish();

            if ( m_updateFlags & QskAnimationHint::UpdateNode )
                m_control->update();
        }
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
                SLOT(cleanup()), Qt::QueuedConnection );
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
            if ( it != m_tables.end() && *it == table )
                m_tables.erase( it );
        }

      private Q_SLOTS:
        void cleanup()
        {
            for ( auto it = m_tables.begin(); it != m_tables.end(); )
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
    std::map< QskAspect, QskHintAnimator > map;
};

QskHintAnimatorTable::QskHintAnimatorTable()
    : m_data( nullptr )
{
}

QskHintAnimatorTable::~QskHintAnimatorTable()
{
    qskAnimatorGuard->unregisterTable( this );
    delete m_data;
}

void QskHintAnimatorTable::start( QskControl* control,
    QskAspect aspect, QskAnimationHint animationHint,
    const QVariant& from, const QVariant& to )
{
    if ( m_data == nullptr )
    {
        m_data = new PrivateData();
        qskAnimatorGuard->registerTable( this );
    }

    auto& animator = m_data->map[ aspect ];

    animator.setAspect( aspect );
    animator.setStartValue( from );
    animator.setEndValue( to );

    animator.setDuration( animationHint.duration );
    animator.setEasingCurve( animationHint.type );
    animator.setUpdateFlags( animationHint.updateFlags );

    animator.setControl( control );
    animator.setWindow( control->window() );

    animator.start();

    if ( qskCheckReceiverThread( control ) )
    {
        QskAnimatorEvent event( aspect, QskAnimatorEvent::Started );
        QCoreApplication::sendEvent( control, &event );
    }
}

const QskHintAnimator* QskHintAnimatorTable::animator( QskAspect aspect ) const
{
    if ( m_data == nullptr )
        return nullptr;

    auto it = m_data->map.find( aspect );
    if ( it == m_data->map.end() )
        return nullptr;

    return &( it->second );
}

QVariant QskHintAnimatorTable::currentValue( QskAspect aspect ) const
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

    for ( auto it = m_data->map.begin(); it != m_data->map.end(); )
    {
        // remove all terminated animators
        if ( !it->second.isRunning() )
        {
            auto control = it->second.control();
            auto aspect = it->first;

            it = m_data->map.erase( it );

            if ( control )
            {
                if ( qskCheckReceiverThread( control ) )
                {
                    auto event = new QskAnimatorEvent( aspect, QskAnimatorEvent::Terminated );
                    QCoreApplication::postEvent( control, event );
                }
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
