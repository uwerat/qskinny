/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskItemAnchors.h"
#include "QskMargins.h"
#include "QskInternalMacros.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickanchors_p.h>
#include <private/qquickanchors_p_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

namespace
{
    struct AnchorLineOperators
    {
        QQuickAnchorLine ( QQuickAnchors::*line ) () const;
        void ( QQuickAnchors::*setLine )( const QQuickAnchorLine& );
        void ( QQuickAnchors::*resetLine )();
    };

    class UpdateBlocker
    {
      public:
        UpdateBlocker( QQuickAnchors* anchors )
            : m_anchors( anchors )
        {
            auto d = QQuickAnchorsPrivate::get( anchors );

            m_wasComplete = d->componentComplete;
            d->componentComplete = false;
        }

        ~UpdateBlocker()
        {
            reset();
        }

        void reset() const
        {
            auto d = QQuickAnchorsPrivate::get( m_anchors );
            d->componentComplete = m_wasComplete;
        }
      private:
        QQuickAnchors* m_anchors;
        bool m_wasComplete;
    };
}

static inline QQuickAnchors* qskGetOrCreateAnchors( QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->anchors();
}

static inline QQuickAnchors* qskGetAnchors( QQuickItem* item )
{
    if ( item == nullptr )
        return nullptr;

    return QQuickItemPrivate::get( item )->_anchors;
}

static inline const QQuickAnchors* qskGetAnchors( const QQuickItem* item )
{
    return qskGetAnchors( const_cast< QQuickItem* >( item ) );
}

static inline QQuickAnchors::Anchor qskToQuickAnchor( Qt::AnchorPoint edge )
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

static inline Qt::AnchorPoint qskToAnchorPoint( QQuickAnchors::Anchor anchor )
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

static inline bool qskIsCenterAnchorPoint( Qt::AnchorPoint edge )
{
    return ( edge == Qt::AnchorHorizontalCenter )
        || ( edge == Qt::AnchorVerticalCenter );
}

static inline const AnchorLineOperators& qskAnchorLineOperators( Qt::AnchorPoint edge )
{
    using A = QQuickAnchors;

    static constexpr AnchorLineOperators table[] =
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

    const auto oldMargins = this->margins();

    Qt::Orientations changes;

    if ( margins.left() != oldMargins.left() )
        changes |= Qt::Horizontal;

    if ( margins.right() != oldMargins.right() )
        changes |= Qt::Horizontal;

    if ( margins.top() != oldMargins.top() )
        changes |= Qt::Vertical;

    if ( margins.bottom() != oldMargins.bottom() )
        changes |= Qt::Vertical;

    if ( changes )
    {
        const auto left = margins.left();

        UpdateBlocker blocker( anchors );

        anchors->setLeftMargin( left + 1.0 );
        anchors->setRightMargin( margins.right() );
        anchors->setTopMargin( margins.top() );
        anchors->setBottomMargin( margins.bottom() );

        blocker.reset();

        anchors->setLeftMargin( left ); // trigger final update
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
        return !qskIsCenterAnchorPoint( edge ) ? fill : nullptr;

    if ( auto centerIn = anchors->centerIn() )
        return qskIsCenterAnchorPoint( edge ) ? centerIn : nullptr;

    const auto& ops = qskAnchorLineOperators( edge );
    return ( ( anchors->*ops.line ) () ).item;
}

void QskItemAnchors::addAnchors( Qt::Corner corner,
    QQuickItem* settledItem, Qt::Corner settledItemCorner )
{
    auto anchorPoint =
        []( Qt::Corner cn, Qt::Orientation orientation )
        {
            if ( orientation == Qt::Horizontal )
                return ( cn & 0x1 ) ? Qt::AnchorRight : Qt::AnchorLeft;
            else
                return ( cn >= 0x2 ) ? Qt::AnchorBottom : Qt::AnchorTop;
        };

    addAnchor( anchorPoint( corner, Qt::Horizontal ),
        settledItem, anchorPoint( settledItemCorner, Qt::Horizontal ) );

    addAnchor( anchorPoint( corner, Qt::Vertical ),
        settledItem, anchorPoint( settledItemCorner, Qt::Vertical ) );
}

void QskItemAnchors::addAnchor( Qt::AnchorPoint edge, QQuickItem* settledItem,
    Qt::AnchorPoint settledItemEdge )
{
    if ( settledItem == nullptr )
        return;

    if ( const auto anchors = qskGetOrCreateAnchors( m_attachedItem ) )
    {
        const auto& ops = qskAnchorLineOperators( edge );
        ( anchors->*ops.setLine )( { settledItem, qskToQuickAnchor( settledItemEdge ) } );
    }
}

void QskItemAnchors::removeAnchor( Qt::AnchorPoint edge )
{
    const auto anchors = qskGetAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return;

    if ( auto fill = anchors->fill() )
    {
        if ( !qskIsCenterAnchorPoint( edge ) )
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
        if ( qskIsCenterAnchorPoint( edge ) )
        {
            anchors->resetCenterIn();

            // setting the other direction as anchor
            const auto otherEdge = ( edge == Qt::AnchorHorizontalCenter )
                ? Qt::AnchorVerticalCenter : Qt::AnchorHorizontalCenter;

            addAnchor( otherEdge, centerIn, otherEdge );
        }

        return;
    }

    const auto& ops = qskAnchorLineOperators( edge );
    ( anchors->*ops.resetLine ) ();
}

void QskItemAnchors::clearAnchors()
{
    const auto anchors = qskGetAnchors( m_attachedItem );
    if ( anchors == nullptr )
        return;

    const UpdateBlocker blocker( anchors );

    anchors->resetFill();
    anchors->resetCenterIn();

    for ( int i = 0; i < 6; i++ )
    {
        const auto& ops = qskAnchorLineOperators( static_cast< Qt::AnchorPoint >( i ) );
        ( anchors->*ops.resetLine ) ();
    }
}

void QskItemAnchors::setBorderAnchors(
    QQuickItem* settledItem, Qt::Orientations orientations )
{
    if ( settledItem == nullptr || m_attachedItem == nullptr )
        return;

    auto anchors = qskGetOrCreateAnchors( m_attachedItem );

    switch( orientations )
    {
        case Qt::Horizontal:
        {
            clearAnchors();

            const UpdateBlocker blocker( anchors );
            addAnchor( Qt::AnchorLeft, settledItem, Qt::AnchorLeft );
            blocker.reset();

            addAnchor( Qt::AnchorRight, settledItem, Qt::AnchorRight );


            break;
        }

        case Qt::Vertical:
        {
            clearAnchors();

            const UpdateBlocker blocker( anchors );
            addAnchor( Qt::AnchorTop, settledItem, Qt::AnchorTop );
            blocker.reset();

            addAnchor( Qt::AnchorBottom, settledItem, Qt::AnchorBottom );

            break;
        }

        case Qt::Horizontal | Qt::Vertical:
        {
            if ( settledItem != anchors->fill() )
            {
                clearAnchors();
                anchors->setFill( settledItem );
            }
            break;
        }
    }
}

void QskItemAnchors::setCenterAnchors(
    QQuickItem* settledItem, Qt::Orientations orientations )
{
    if ( settledItem == nullptr || m_attachedItem == nullptr )
        return;

    switch( orientations )
    {
        case Qt::Horizontal:
        {
            clearAnchors();
            addAnchor( Qt::AnchorHorizontalCenter,
                settledItem, Qt::AnchorHorizontalCenter );
            break;
        }

        case Qt::Vertical:
        {
            clearAnchors();
            addAnchor( Qt::AnchorVerticalCenter,
                settledItem, Qt::AnchorVerticalCenter );
            break;
        }

        case Qt::Horizontal | Qt::Vertical:
        {
            auto anchors = qskGetOrCreateAnchors( m_attachedItem );

            if ( settledItem != anchors->centerIn() )
            {
                clearAnchors();
                anchors->setCenterIn( settledItem );
            }
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

        const auto& ops = qskAnchorLineOperators( edge );
        return qskToAnchorPoint( ( ( anchors->*ops.line ) () ).anchorLine );
    }

    return Qt::AnchorLeft; // something
}
