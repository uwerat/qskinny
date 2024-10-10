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

QskItemAnchors::QskItemAnchors( QQuickItem* anchoredItem )
    : m_anchoredItem( anchoredItem )
{
}

QskItemAnchors::~QskItemAnchors()
{
}

bool QskItemAnchors::operator==( const QskItemAnchors& other ) const noexcept
{
    return m_anchoredItem.data() == other.m_anchoredItem.data();
}

QQuickItem* QskItemAnchors::anchoredItem() const
{
    return m_anchoredItem;
}

QMarginsF QskItemAnchors::margins() const
{
    if ( const auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        return QMarginsF( anchors->leftMargin(), anchors->topMargin(),
            anchors->rightMargin(), anchors->bottomMargin() );
    }

    return QMarginsF();
}

void QskItemAnchors::setMargins( const QMarginsF& margins )
{
    if ( const auto anchors = qskGetOrCreateAnchors( m_anchoredItem ) )
    {
        anchors->setLeftMargin( margins.left() );
        anchors->setRightMargin( margins.right() );
        anchors->setTopMargin( margins.top() );
        anchors->setBottomMargin( margins.bottom() );
    }
}

void QskItemAnchors::setCenterOffset( Qt::Orientation orientation, qreal offset )
{
    if ( const auto anchors = qskGetOrCreateAnchors( m_anchoredItem ) )
    {
        if ( orientation == Qt::Horizontal )
            anchors->setHorizontalCenterOffset( offset );
        else
            anchors->setVerticalCenterOffset( offset );
    }
}

qreal QskItemAnchors::centerOffset( Qt::Orientation orientation )
{
    if ( const auto anchors = qskGetOrCreateAnchors( m_anchoredItem ) )
    {
        if ( orientation == Qt::Horizontal )
            return anchors->horizontalCenterOffset();
        else
            return anchors->verticalCenterOffset();
    }

    return 0.0;
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

void QskItemAnchors::addAnchors( QQuickItem* baseItem, Qt::Orientations orientations )
{
    if ( orientations & Qt::Horizontal )
    {
        addAnchor( Qt::AnchorLeft, baseItem, Qt::AnchorLeft );
        addAnchor( Qt::AnchorRight, baseItem, Qt::AnchorRight );
    }

    if ( orientations & Qt::Vertical )
    {
        addAnchor( Qt::AnchorTop, baseItem, Qt::AnchorTop );
        addAnchor( Qt::AnchorBottom, baseItem, Qt::AnchorBottom );
    }
}

void QskItemAnchors::addAnchor( Qt::AnchorPoint edge, QQuickItem* baseItem,
    Qt::AnchorPoint baseEdge )
{
    if ( const auto anchors = qskGetOrCreateAnchors( m_anchoredItem ) )
    {
        const auto& ops = operators( edge );
        ( anchors->*ops.setLine )( { baseItem, toQuickAnchor( baseEdge ) } );
    }
}

void QskItemAnchors::removeAnchor( Qt::AnchorPoint edge )
{
    if ( const auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        const auto& ops = operators( edge );
        ( anchors->*ops.resetLine ) ();
    }
}


QQuickItem* QskItemAnchors::baseItem( Qt::AnchorPoint edge ) const
{
    if ( const auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        const auto& ops = operators( edge );
        return ( ( anchors->*ops.line ) () ).item;
    }

    return nullptr;
}

Qt::AnchorPoint QskItemAnchors::basePosition( Qt::AnchorPoint edge ) const
{
    if ( const auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        /*
            Anchoring to the baseline of the anchoredItem might have been
            done in QML code. As Qt::AnchorPoint does not have a corresponding
            value for it and QSkinny does not support the baseline concept at all
            we are lying and report Qt::AnchorTop instead. Hm ...
         */

        const auto& ops = operators( edge );
        return toAnchorPoint( ( ( anchors->*ops.line ) () ).anchorLine );
    }

    return Qt::AnchorLeft; // something
}

void QskItemAnchors::setControlItem( QQuickItem* item, bool adjustSize )
{
    if ( const auto anchors = qskGetOrCreateAnchors( m_anchoredItem ) )
    {
        if ( adjustSize )
            anchors->setFill( item );
        else
            anchors->setCenterIn( item );
    }
}

void QskItemAnchors::removeControlItem( bool adjustSize )
{
    if ( auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        if ( adjustSize )
            anchors->resetFill();
        else
            anchors->resetCenterIn();
    }
}

QQuickItem* QskItemAnchors::controlItem( bool adjustSize ) const
{
    if ( const auto anchors = qskGetAnchors( m_anchoredItem ) )
    {
        if ( adjustSize )
            return anchors->fill();
        else
            return anchors->centerIn();
    }

    return nullptr;
}
