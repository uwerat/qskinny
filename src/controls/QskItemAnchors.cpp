/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskItemAnchors.h"
#include "QskMargins.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickanchors_p.h>
#include <private/qquickanchors_p_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

QQuickAnchors* qskGetOrCreateAnchors( QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->anchors();
}

QQuickAnchors* qskGetAnchors( QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->_anchors;
}

const QQuickAnchors* qskGetAnchors( const QQuickItem* item )
{
    return qskGetAnchors( const_cast< QQuickItem* >( item ) );
}

namespace
{
    inline QQuickAnchors::Anchor toQuickAnchor( Qt::AnchorPoint edge )
    {
        using A = QQuickAnchors;

        switch( edge )
        {
            case Qt::AnchorLeft:             return A::LeftAnchor;
            case Qt::AnchorHorizontalCenter: return A::HCenterAnchor;
            case Qt::AnchorRight:            return A::RightAnchor;
            case Qt::AnchorTop:              return A::TopAnchor;
            case Qt::AnchorVerticalCenter:   return A::VCenterAnchor;
            case Qt::AnchorBottom:           return A::BottomAnchor;
            default:                         return A::InvalidAnchor;
        }
    }

    inline Qt::AnchorPoint toAnchorPoint( QQuickAnchors::Anchor anchor )
    {
        using A = QQuickAnchors;

        switch( anchor )
        {
            case A::LeftAnchor:     return Qt::AnchorLeft;
            case A::HCenterAnchor:  return Qt::AnchorHorizontalCenter;
            case A::RightAnchor:    return Qt::AnchorRight;
            case A::TopAnchor:      return Qt::AnchorTop;
            case A::VCenterAnchor:  return Qt::AnchorVerticalCenter;
            case A::BottomAnchor:   return Qt::AnchorBottom;
            case A::BaselineAnchor: return Qt::AnchorTop; // not supported
            default:                return Qt::AnchorLeft;
        }
    }

    bool isCenterAnchorPoint( Qt::AnchorPoint edge )
    {
        return ( edge == Qt::AnchorHorizontalCenter )
            || ( edge == Qt::AnchorVerticalCenter );
    }

    struct AnchorOperators
    {
        QQuickAnchorLine ( QQuickAnchors::*line ) () const;
        void ( QQuickAnchors::*setLine )( const QQuickAnchorLine& );
        void ( QQuickAnchors::*resetLine )();
    };

    const AnchorOperators& operators( Qt::AnchorPoint edge )
    {
        using A = QQuickAnchors;

        static constexpr AnchorOperators table[] =
        {
            // in order of Qt::AnchorPoint

            { &A::left, &A::setLeft, &A::resetLeft },
            { &A::horizontalCenter, &A::setHorizontalCenter, &A::resetHorizontalCenter },
            { &A::right, &A::setRight, &A::resetRight },
            { &A::top, &A::setTop, &A::resetTop },
            { &A::verticalCenter, &A::setVerticalCenter, &A::resetVerticalCenter },
            { &A::bottom, &A::setBottom, &A::resetBottom }
        };

        return table[edge];
    }
}

QskItemAnchors::QskItemAnchors( QQuickItem* attachedItem )
    : m_attachedItem( attachedItem )
{
}

QskItemAnchors::~QskItemAnchors()
{
}

bool QskItemAnchors::operator==( const QskItemAnchors& other ) const noexcept
{
    return m_attachedItem.data() == other.m_attachedItem.data();
}

QQuickItem* QskItemAnchors::attachedItem() const
{
    return m_attachedItem;
}

QMarginsF QskItemAnchors::margins() const
{
    const auto anchors = qskGetAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return QMarginsF();

    const auto d = QQuickAnchorsPrivate::get( anchors );

    const auto left = d->leftMarginExplicit ? d->leftMargin : d->margins;
    const auto right = d->rightMarginExplicit ? d->rightMargin : d->margins;
    const auto top = d->rightMarginExplicit ? d->rightMargin : d->margins;
    const auto bottom = d->bottomMarginExplicit ? d->bottomMargin : d->margins;

    return QMarginsF( left, top, right, bottom );
}

void QskItemAnchors::setMargins( const QMarginsF& margins )
{
    auto anchors = qskGetOrCreateAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return;

    if ( margins != this->margins() )
    {
        anchors->setLeftMargin( margins.left() );
        anchors->setRightMargin( margins.right() );
        anchors->setTopMargin( margins.top() );
        anchors->setBottomMargin( margins.bottom() );
    }
}

void QskItemAnchors::setCenterOffsets(
    qreal horizontalOffset, qreal verticalOffset )
{
    if ( auto anchors = qskGetOrCreateAnchors( m_attachedItem ) )
    {
        anchors->setHorizontalCenterOffset( horizontalOffset );
        anchors->setVerticalCenterOffset( verticalOffset );
    }
}

void QskItemAnchors::setCenterOffset( Qt::Orientation orientation, qreal offset )
{
    auto anchors = qskGetOrCreateAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return;

    if ( orientation == Qt::Horizontal )
        anchors->setHorizontalCenterOffset( offset );
    else
        anchors->setVerticalCenterOffset( offset );
}

qreal QskItemAnchors::centerOffset( Qt::Orientation orientation )
{
    if ( const auto anchors = qskGetAnchors( m_attachedItem ) )
    {
        if ( orientation == Qt::Horizontal )
            return anchors->horizontalCenterOffset();
        else
            return anchors->verticalCenterOffset();
    }

    return 0.0;
}

QQuickItem* QskItemAnchors::settledItem( Qt::AnchorPoint edge ) const
{
    const auto anchors = qskGetAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return nullptr;

    if ( auto fill = anchors->fill() )
        return !isCenterAnchorPoint( edge ) ? fill : nullptr;

    if ( auto centerIn = anchors->centerIn() )
        return isCenterAnchorPoint( edge ) ? centerIn : nullptr;

    const auto& ops = operators( edge );
    return ( ( anchors->*ops.line ) () ).item;
}

void QskItemAnchors::addAnchors( Qt::Corner corner,
    QQuickItem* baseItem, Qt::Corner baseCorner )
{
    auto anchorPoint =
        []( Qt::Corner corner, Qt::Orientation orientation )
        {
            if ( orientation == Qt::Horizontal )
                return ( corner & 0x1 ) ? Qt::AnchorRight : Qt::AnchorLeft;
            else
                return ( corner >= 0x2 ) ? Qt::AnchorBottom : Qt::AnchorTop;
        };

    addAnchor( anchorPoint( corner, Qt::Horizontal ),
        baseItem, anchorPoint( baseCorner, Qt::Horizontal ) );

    addAnchor( anchorPoint( corner, Qt::Vertical ),
        baseItem, anchorPoint( baseCorner, Qt::Vertical ) );
}

void QskItemAnchors::addAnchor( Qt::AnchorPoint edge, QQuickItem* baseItem,
    Qt::AnchorPoint baseEdge )
{
    if ( baseItem == nullptr )
        return;

    if ( const auto anchors = qskGetOrCreateAnchors( m_attachedItem ) )
    {
        const auto& ops = operators( edge );
        ( anchors->*ops.setLine )( { baseItem, toQuickAnchor( baseEdge ) } );
    }
}

void QskItemAnchors::removeAnchor( Qt::AnchorPoint edge )
{
    const auto anchors = qskGetAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return;

    if ( auto fill = anchors->fill() )
    {
        if ( !isCenterAnchorPoint( edge ) )
        {
            anchors->resetFill();

            // setting the other borders as anchors
            for ( auto anchorPoint : { Qt::AnchorLeft, Qt::AnchorRight,
                Qt::AnchorTop, Qt::AnchorBottom } )
            {
                if ( edge != anchorPoint )
                    addAnchor( anchorPoint, fill, anchorPoint );
            }
        }

        return;
    }

    if ( auto centerIn = anchors->centerIn() )
    {
        if ( isCenterAnchorPoint( edge ) )
        {
            anchors->resetCenterIn();

            // setting the other direction as anchor
            const auto otherEdge = ( edge == Qt::AnchorHorizontalCenter )
                ? Qt::AnchorVerticalCenter : Qt::AnchorHorizontalCenter;

            addAnchor( otherEdge, centerIn, otherEdge );
        }

        return;
    }

    const auto& ops = operators( edge );
    ( anchors->*ops.resetLine ) ();
}

void QskItemAnchors::clearAnchors()
{
    if ( const auto anchors = qskGetAnchors( m_attachedItem ) )
    {
        anchors->resetFill();
        anchors->resetCenterIn();

        for ( int i = 0; i < 6; i++ )
        {
            const auto& ops = operators( static_cast< Qt::AnchorPoint >( i ) );
            ( anchors->*ops.resetLine ) ();
        }
    }
}

void QskItemAnchors::setBorderAnchors( QQuickItem* baseItem, Qt::Orientations orientations )
{
    if ( baseItem == nullptr || m_attachedItem == nullptr )
        return;

    switch( orientations )
    {
        case Qt::Horizontal:
        {
            addAnchor( Qt::AnchorLeft, baseItem, Qt::AnchorLeft );
            addAnchor( Qt::AnchorRight, baseItem, Qt::AnchorRight );
            break;
        }

        case Qt::Vertical:
        {
            addAnchor( Qt::AnchorTop, baseItem, Qt::AnchorTop );
            addAnchor( Qt::AnchorBottom, baseItem, Qt::AnchorBottom );
            break;
        }

        case Qt::Horizontal | Qt::Vertical:
        {
            auto anchors = qskGetOrCreateAnchors( m_attachedItem );
            for ( int i = 0; i < 6; i++ )
            {
                const auto& ops = operators( static_cast< Qt::AnchorPoint >( i ) );
                ( anchors->*ops.resetLine ) ();
            }

            anchors->setFill( baseItem );
            break;
        }
    }
}

void QskItemAnchors::setCenterAnchors( QQuickItem* baseItem, Qt::Orientations orientations )
{
    if ( baseItem == nullptr || m_attachedItem == nullptr )
        return;

    switch( orientations )
    {
        case Qt::Horizontal:
        {
            addAnchor( Qt::AnchorHorizontalCenter, baseItem, Qt::AnchorHorizontalCenter );
            break;
        }

        case Qt::Vertical:
        {
            addAnchor( Qt::AnchorVerticalCenter, baseItem, Qt::AnchorVerticalCenter );
            break;
        }

        case Qt::Horizontal | Qt::Vertical:
        {
            auto anchors = qskGetOrCreateAnchors( m_attachedItem );

            for ( int i = 0; i < 6; i++ )
            {
                const auto& ops = operators( static_cast< Qt::AnchorPoint >( i ) );
                ( anchors->*ops.resetLine ) ();
            }

            anchors->setCenterIn( baseItem );
            break;
        }
    }
}

Qt::AnchorPoint QskItemAnchors::settledItemAnchorPoint( Qt::AnchorPoint edge ) const
{
    if ( const auto anchors = qskGetAnchors( m_attachedItem ) )
    {
        /*
            Anchoring to the baseline of the attachedItem might have been
            done in QML code. As Qt::AnchorPoint does not have a corresponding
            value for it and QSkinny does not support the baseline concept at all
            we are lying and report Qt::AnchorTop instead. Hm ...
         */

        const auto& ops = operators( edge );
        return toAnchorPoint( ( ( anchors->*ops.line ) () ).anchorLine );
    }

    return Qt::AnchorLeft; // something
}
