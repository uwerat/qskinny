/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskStackBoxAnimator.h"
#include "QskStackBox.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskFunctions.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

static Qsk::Direction qskDirection(
    Qt::Orientation orientation, int from, int to, int itemCount )
{
    Qsk::Direction direction;

    if ( orientation == Qt::Horizontal )
    {
        direction = Qsk::RightToLeft;

        if ( to > from )
        {
            const bool isWrapping = ( from == 0 ) && ( to == itemCount - 1 );

            if ( !isWrapping )
                direction = Qsk::LeftToRight;
        }
        else
        {
            const bool isWrapping = ( to == 0 ) && ( from == itemCount - 1 );

            if ( isWrapping )
                direction = Qsk::LeftToRight;
        }
    }
    else
    {
        direction = Qsk::BottomToTop;

        if ( to > from )
        {
            const bool isWrapping = ( from == 0 ) && ( to == itemCount - 1 );

            if ( !isWrapping )
                direction = Qsk::TopToBottom;
        }
        else
        {
            const bool isWrapping = ( to == 0 ) && ( from == itemCount - 1 );

            if ( isWrapping )
                direction = Qsk::TopToBottom;
        }
    }

    return direction;
}

namespace
{
    class RotationTransform : public QQuickTransform
    {
        Q_OBJECT

      public:
        RotationTransform( Qt::Axis axis, qreal dx1, qreal dy1, qreal dx2,
                qreal dy2, qreal radians, QQuickItem* item )
            : QQuickTransform( item )
            , m_axis( axis )
            , m_dx1( dx1 )
            , m_dy1( dy1 )
            , m_dx2( dx2 )
            , m_dy2( dy2 )
            , m_radians( radians )
        {
            prependToItem( item );
        }

        void setRadians( qreal radians )
        {
            if ( m_radians != radians )
            {
                m_radians = radians;
                update();
            }
        }

        void setPreTransform( qreal dx1, qreal dy1 )
        {
            if( !qskFuzzyCompare( dx1, m_dx1 ) || !qskFuzzyCompare( dy1, m_dy1 ) )
            {
                m_dx1 = dx1;
                m_dy1 = dy1;
                update();
            }
        }

        void setPostTransform( qreal dx2, qreal dy2 )
        {
            if( !qskFuzzyCompare( dx2, m_dx2 ) || !qskFuzzyCompare( dy2, m_dy2 ) )
            {
                m_dx2 = dx2;
                m_dy2 = dy2;
                update();
            }
        }

        void applyTo( QMatrix4x4* matrix ) const override
        {
            if ( const auto item = qobject_cast< QQuickItem* >( parent() ) )
            {
                QTransform transform;
                transform.translate( m_dx1, m_dy1 );
                transform.rotateRadians( m_radians, m_axis );
                transform.translate( m_dx2, m_dy2 );

                *matrix *= transform;
            }
        }

      private:
        const Qt::Axis m_axis;
        qreal m_dx1;
        qreal m_dy1;
        qreal m_dx2;
        qreal m_dy2;
        qreal m_radians;
    };

    static RotationTransform* qskFindRotationTransform( const QQuickItem* item )
    {
        const auto& transforms = QQuickItemPrivate::get( item )->transforms;
        for ( const auto& t : transforms )
        {
            if ( auto transform = qobject_cast< RotationTransform* >( t ) )
                return transform;
        }

        return nullptr;
    }
}

QskStackBoxAnimator::QskStackBoxAnimator( QskStackBox* parent )
    : QObject( parent )
    , m_startIndex( -1 )
    , m_endIndex( -1 )
{
}

QskStackBoxAnimator::~QskStackBoxAnimator()
{
}

void QskStackBoxAnimator::setStartIndex( int index )
{
    m_transientIndex = m_startIndex = index;
}

void QskStackBoxAnimator::setEndIndex( int index )
{
    m_endIndex = index;
}

int QskStackBoxAnimator::startIndex() const
{
    return m_startIndex;
}

int QskStackBoxAnimator::endIndex() const
{
    return m_endIndex;
}

QskStackBox* QskStackBoxAnimator::stackBox() const
{
    return static_cast< QskStackBox* >( parent() );
}

QQuickItem* QskStackBoxAnimator::itemAt( int index ) const
{
    return stackBox()->itemAtIndex(
        ( index == 0 ) ? m_startIndex : m_endIndex );
}

qreal QskStackBoxAnimator::transientIndex() const
{
    return m_transientIndex;
}

void QskStackBoxAnimator::advance( qreal progress )
{
    qreal transientIndex;

    if ( qFuzzyIsNull( progress ) )
    {
        transientIndex = m_startIndex;
    }
    else if ( qFuzzyCompare( progress, 1.0 ) )
    {
        transientIndex = m_endIndex;
    }
    else
    {
        const auto box = stackBox();

        auto startIndex = m_startIndex;
        auto endIndex = m_endIndex;

        if ( startIndex == 0 )
        {
            if ( endIndex == box->itemCount() - 1 )
                startIndex += box->itemCount();
        }
        else if ( startIndex == box->itemCount() - 1 )
        {
            if ( endIndex == 0 )
                endIndex += box->itemCount();
        }

        transientIndex = startIndex + ( endIndex - startIndex ) * progress;
    }

    if ( !qskFuzzyCompare( m_transientIndex, transientIndex ) )
    {
        m_transientIndex = transientIndex;
        advanceIndex( progress );

        Q_EMIT stackBox()->transientIndexChanged( m_transientIndex );
    }
}

QskStackBoxAnimator1::QskStackBoxAnimator1( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
    , m_orientation( Qt::Horizontal )
    , m_isDirty( false )
    , m_hasClip( false )
{
    // catching geometryChanges to know about resizing
}

QskStackBoxAnimator1::~QskStackBoxAnimator1()
{
}

void QskStackBoxAnimator1::setOrientation( Qt::Orientation orientation )
{
    if ( m_orientation != orientation )
    {
        stop();
        m_orientation = orientation;
    }
}

Qt::Orientation QskStackBoxAnimator1::orientation() const
{
    return m_orientation;
}

void QskStackBoxAnimator1::setup()
{
    auto stackBox = this->stackBox();

    m_direction = qskDirection( m_orientation,
        startIndex(), endIndex(), stackBox->itemCount() );

    m_hasClip = stackBox->clip();
    if ( !m_hasClip )
        stackBox->setClip( true );

    stackBox->installEventFilter( this );
    m_isDirty = true;
}

void QskStackBoxAnimator1::advanceIndex( qreal value )
{
    auto stackBox = this->stackBox();
    const bool isHorizontal = m_orientation == Qt::Horizontal;

    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            QRectF rect = qskItemGeometry( item );

            if ( m_isDirty )
            {
                const int index = ( i == 0 ) ? startIndex() : endIndex();
                rect = stackBox->geometryForItemAt( index );

                m_itemOffset[ i ] = isHorizontal ? rect.x() : rect.y();
            }

            qreal x, y;

            if ( isHorizontal )
            {
                qreal off = stackBox->width() * ( value - i );
                if ( m_direction == Qsk::LeftToRight )
                    off = -off;

                x = m_itemOffset[ i ] + off;
                y = rect.y();
            }
            else
            {
                qreal off = stackBox->height() * ( value - i );
                if ( m_direction == Qsk::BottomToTop )
                    off = -off;

                x = rect.x();
                y = m_itemOffset[ i ] + off;
            }

            qskSetItemGeometry( item, x, y, rect.width(), rect.height() );

            if ( !item->isVisible() )
                item->setVisible( true );
        }
    }

    m_isDirty = false;
}

void QskStackBoxAnimator1::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            item->removeEventFilter( this );
            item->setVisible( i == 1 );
        }
    }

    if ( !m_hasClip )
        stackBox()->setClip( false );
}

bool QskStackBoxAnimator1::eventFilter( QObject* object, QEvent* event )
{
    if ( !m_isDirty && object == stackBox() )
    {
        switch( static_cast< int >( event->type() ) )
        {
            case QskEvent::GeometryChange:
            case QskEvent::ContentsRectChange:
            case QskEvent::LayoutRequest:
            {
                m_isDirty = true;
                break;
            }
        }
    }

    return QObject::eventFilter( object, event );
}

QskStackBoxAnimator2::QskStackBoxAnimator2( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
    , m_orientation( Qt::Horizontal )
    , m_inverted( false )
{
}

QskStackBoxAnimator2::~QskStackBoxAnimator2()
{
}

void QskStackBoxAnimator2::setOrientation( Qt::Orientation orientation )
{
    if ( m_orientation != orientation )
    {
        stop();
        m_orientation = orientation;
    }
}

Qt::Orientation QskStackBoxAnimator2::orientation() const
{
    return m_orientation;
}

void QskStackBoxAnimator2::setInverted( bool on )
{
    if ( m_inverted != on )
    {
        stop();
        m_inverted = on;
    }
}

bool QskStackBoxAnimator2::isInverted() const
{
    return m_inverted;
}

void QskStackBoxAnimator2::setup()
{
    const auto axis = ( m_orientation == Qt::Horizontal )
        ? Qt::YAxis : Qt::XAxis;
    if ( auto item = itemAt( 0 ) )
    {
        const auto dx = 0.5 * item->width();
        const auto dy = 0.5 * item->height();
        ( void ) new RotationTransform( axis, dx, dy, -dx, -dy, 0.0, item );
    }

    if ( auto item = itemAt( 1 ) )
    {
        const auto dx = 0.5 * item->width();
        const auto dy = 0.5 * item->height();
        ( void ) new RotationTransform( axis, dx, dy, -dx, -dy, M_PI_2, item );
    }
}

void QskStackBoxAnimator2::advanceIndex( qreal value )
{
    auto radians = value * M_PI;

    if ( radians < M_PI_2 && radians > -M_PI_2 )
    {
        if ( auto item = itemAt( 0 ) )
        {
            if ( !m_inverted )
                radians = 2 * M_PI - radians;

            auto rotation = qskFindRotationTransform( item );
            rotation->setRadians( radians );

            item->setVisible( true );
        }

        if ( auto item = itemAt( 1 ) )
        {
            item->setVisible( false );
        }
    }
    else
    {
        if ( auto item = itemAt( 0 ) )
        {
            item->setVisible( false );
        }

        if ( auto item = itemAt( 1 ) )
        {
            radians = radians - M_PI;
            if ( !m_inverted )
                radians = 2 * M_PI - radians;

            auto rotation = qskFindRotationTransform( item );
            rotation->setRadians( radians );

            item->setVisible( true );
        }
    }
}

void QskStackBoxAnimator2::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            delete qskFindRotationTransform( item );
            item->setVisible( i == 1 );
        }
    }
}

QskStackBoxAnimator3::QskStackBoxAnimator3( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
{
}

QskStackBoxAnimator3::~QskStackBoxAnimator3()
{
}

void QskStackBoxAnimator3::setup()
{
    if ( auto item = itemAt( 1 ) )
    {
        item->setOpacity( 0.0 );
        item->setVisible( true );
    }
}

void QskStackBoxAnimator3::advanceIndex( qreal value )
{
    if ( auto item1 = itemAt( 0 ) )
        item1->setOpacity( 1.0 - value );

    if ( auto item2 = itemAt( 1 ) )
        item2->setOpacity( value );
}

void QskStackBoxAnimator3::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            item->setOpacity( 1.0 );
            item->setVisible( i == 1 ); // not here !!
        }
    }
}

QskStackBoxAnimator4::QskStackBoxAnimator4( QskStackBox* parent )
    : QskStackBoxAnimator( parent )
    , m_orientation( Qt::Horizontal )
    , m_inverted( false )
{
}

QskStackBoxAnimator4::~QskStackBoxAnimator4()
{
}

void QskStackBoxAnimator4::setOrientation( Qt::Orientation orientation )
{
    if ( m_orientation != orientation )
    {
        stop();
        m_orientation = orientation;
    }
}

Qt::Orientation QskStackBoxAnimator4::orientation() const
{
    return m_orientation;
}

void QskStackBoxAnimator4::setInverted( bool on )
{
    if ( m_inverted != on )
    {
        stop();
        m_inverted = on;
    }
}

bool QskStackBoxAnimator4::isInverted() const
{
    return m_inverted;
}

void QskStackBoxAnimator4::setup()
{
    const auto axis = ( m_orientation == Qt::Horizontal )
        ? Qt::YAxis : Qt::XAxis;

    if ( auto item = itemAt( 0 ) )
    {
        ( void ) new RotationTransform( axis, 0.0, 0.0, 0.0, 0.0, 0.0, item );
        item->setVisible( true );
    }

    if ( auto item = itemAt( 1 ) )
    {
        ( void ) new RotationTransform( axis, 0.0, 0.0, 0.0, 0.0, -M_PI_2, item );
        item->setVisible( true );
    }
}

void QskStackBoxAnimator4::advanceIndex( qreal value )
{
    if ( auto item = itemAt( 0 ) )
    {
        auto rotation = qskFindRotationTransform( item );

        qreal dx1, dy1, radians, dx2, dy2;

        if( orientation() == Qt::Horizontal )
        {
            const auto w = item->parentItem() ? item->parentItem()->width() : item->width();

            if( isInverted() )
            {
                dx1 = ( w - item->x() ) * value;
                radians = -M_PI_2 * value;
                dx2 = 0.0;
            }
            else
            {
                dx1 = w * ( 1 - value ) - item->x() * value;
                radians = M_PI_2 * value;
                dx2 = -w;
            }

            dy1 = 0.5 * item->height();
            dy2 = -dy1;
        }
        else
        {
            const auto h = item->parentItem() ? item->parentItem()->height() : item->height();

            dx1 = 0.5 * item->width();
            dx2 = -dx1;

            if( isInverted() )
            {
                dy1 = ( h - item->y() ) * value;
                radians = -M_PI_2 * value;
                dy2 = 0.0;
            }
            else
            {
                dy1 = h * ( 1 - value ) - item->y() * value;
                radians = M_PI_2 * value;
                dy2 = -h;
            }
        }

        rotation->setPreTransform( dx1, dy1 );
        rotation->setRadians( radians );
        rotation->setPostTransform( dx2, dy2 );
    }

    if ( auto item = itemAt( 1 ) )
    {
        auto rotation = qskFindRotationTransform( item );

        qreal dx1, dy1, radians, dx2, dy2;

        if( orientation() == Qt::Horizontal )
        {
            const auto w = item->parentItem() ? item->parentItem()->width() : item->width();

            if( isInverted() )
            {
                dx1 = w * value - item->x() * ( 1 - value );
                radians = -M_PI_2 * ( value - 1 );
                dx2 = -w;
            }
            else
            {
                dx1 = ( item->width() + item->x() ) * ( 1 - value );
                radians = M_PI_2 * ( value - 1 );
                dx2 = 0.0;
            }

            dy1 = 0.5 * item->height();
            dy2 = -dy1;
        }
        else
        {
            const auto h = item->parentItem() ? item->parentItem()->height() : item->height();

            dx1 = 0.5 * item->width();
            dx2 = -dx1;

            if( isInverted() )
            {
                dy1 = h * value - item->y() * ( 1 - value );
                radians = -M_PI_2 * ( value - 1 );
                dy2 = -h;
            }
            else
            {
                dy1 = ( item->height() + item->y() ) * ( 1 - value );
                radians = M_PI_2 * ( value - 1 );
                dy2 = 0.0;
            }
        }

        rotation->setPreTransform( dx1, dy1 );
        rotation->setRadians( radians );
        rotation->setPostTransform( dx2, dy2 );
    }
}

void QskStackBoxAnimator4::done()
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( auto item = itemAt( i ) )
        {
            delete qskFindRotationTransform( item );
            item->setVisible( i == 1 );
        }
    }
}

#include "moc_QskStackBoxAnimator.cpp"
#include "QskStackBoxAnimator.moc"
