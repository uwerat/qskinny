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

QskHintAnimator::QskHintAnimator() noexcept
    : m_index( -1 )
{
}

QskHintAnimator::QskHintAnimator( const QskAspect aspect, int index ) noexcept
    : m_aspect( aspect )
    , m_index( index )
{
}

QskHintAnimator::~QskHintAnimator()
{
}

void QskHintAnimator::setAspect( const QskAspect aspect ) noexcept
{
    m_aspect = aspect;
}

void QskHintAnimator::setIndex( int index ) noexcept
{
    m_index = index;
}

void QskHintAnimator::setUpdateFlags( QskAnimationHint::UpdateFlags flags ) noexcept
{
    m_updateFlags = flags;
}

void QskHintAnimator::setControl( QskControl* control ) noexcept
{
    m_control = control;
}

void QskHintAnimator::advance( qreal progress )
{
    const auto oldValue = currentValue();

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

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskHintAnimator& animator )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Animator" << "( ";

    debug << animator.aspect() << ", " << animator.endValue().typeName() << ", ";

    if ( animator.index() >= 0 )
        debug << animator.index() << ", ";

    if ( animator.isRunning() )
        debug << "R: " << animator.duration() << ", " << animator.elapsed();
    else
        debug << "S" << animator.duration();

    if ( auto control = animator.control() )
        debug << ", " << control->className() << ", " << (void*) control;

    debug << " )";

    return debug;
}

#endif

namespace
{
    class AnimatorMap : public std::vector< QskHintAnimator* >
    {
      public:
        ~AnimatorMap()
        {
            qDeleteAll( *this );
        }

        inline const QskHintAnimator* find( const QskAspect aspect, int index  ) const
        {
            const Key key { aspect, index };

            auto it = std::lower_bound( cbegin(), cend(), key, lessThan );
            if ( it != cend() )
            {
                if ( ( ( *it )->aspect() == aspect ) && ( ( *it )->index() == index ) )
                    return *it;
            }

            return nullptr;
        }

        inline QskHintAnimator* findOrInsert( const QskAspect aspect, int index )
        {
            const Key key { aspect, index };

            auto it = std::lower_bound( begin(), end(), key, lessThan );
            if ( it == end() || ( *it )->aspect() != aspect || ( *it )->index() != index )
            {
                it = insert( it, new QskHintAnimator( aspect, index ) );
            }

            return *it;
        }

      private:
        struct Key
        {
            QskAspect aspect;
            int index;
        };

        static inline bool lessThan( const QskHintAnimator* animator, const Key& key )
        {
            if ( animator->aspect() == key.aspect )
                return animator->index() < key.index;

            return animator->aspect() < key.aspect;
        }
    };

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
    AnimatorMap animators; // a flat map
};

QskHintAnimatorTable::QskHintAnimatorTable()
{
}

QskHintAnimatorTable::~QskHintAnimatorTable()
{
    if ( qskAnimatorGuard )
        qskAnimatorGuard->unregisterTable( this );

    delete m_data;
}

void QskHintAnimatorTable::start( QskControl* control,
    const QskAspect aspect, int index, QskAnimationHint animationHint,
    const QVariant& from, const QVariant& to )
{
    if ( m_data == nullptr )
    {
        m_data = new PrivateData();

        if ( qskAnimatorGuard )
            qskAnimatorGuard->registerTable( this );
    }

    auto animator = m_data->animators.findOrInsert( aspect, index );

    animator->setStartValue( from );
    animator->setEndValue( to );

    animator->setDuration( animationHint.duration );
    animator->setEasingCurve( animationHint.type );
    animator->setUpdateFlags( animationHint.updateFlags );

    animator->setControl( control );
    animator->setWindow( control->window() );

    animator->start();

    if ( qskCheckReceiverThread( control ) )
    {
        QskAnimatorEvent event( aspect, index, QskAnimatorEvent::Started );
        QCoreApplication::sendEvent( control, &event );
    }
}

const QskHintAnimator* QskHintAnimatorTable::animator( QskAspect aspect, int index ) const
{
    if ( m_data )
        return m_data->animators.find( aspect, index );

    return nullptr;
}

QVariant QskHintAnimatorTable::currentValue( QskAspect aspect, int index ) const
{
    if ( m_data )
    {
        if ( auto animator = m_data->animators.find( aspect, index ) )
        {
            if ( animator->isRunning() )
                return animator->currentValue();
        }
    }

    return QVariant();
}

bool QskHintAnimatorTable::cleanup()
{
    if ( m_data == nullptr )
        return true;

    auto& animators = m_data->animators;

    for ( auto it = animators.begin(); it != animators.end(); )
    {
        auto animator = *it;

        // remove all terminated animators
        if ( !animator->isRunning() )
        {
            const auto control = animator->control();
            const auto aspect = animator->aspect();
            const auto index = animator->index();

            delete animator;

            it = animators.erase( it );

            if ( control )
            {
                if ( qskCheckReceiverThread( control ) )
                {
                    auto event = new QskAnimatorEvent(
                        aspect, index, QskAnimatorEvent::Terminated );

                    QCoreApplication::postEvent( control, event );
                }
            }
        }
        else
        {
            ++it;
        }
    }

    if ( animators.empty() )
    {
        delete m_data;
        m_data = nullptr;

        return true;
    }

    return false;
}

#include "QskHintAnimator.moc"
