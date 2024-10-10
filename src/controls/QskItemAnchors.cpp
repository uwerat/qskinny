/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskItemAnchors.h"
#include "QskMargins.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquickanchors_p.h>
#include <private/qquickanchors_p_p.h>
QSK_QT_PRIVATE_END

namespace
{
    inline QQuickAnchors::Anchor toQuickAnchor( Qt::AnchorPoint edge )
    {
        switch( edge )
        {
            case Qt::AnchorLeft:
                return QQuickAnchors::LeftAnchor;

            case Qt::AnchorHorizontalCenter:
                return QQuickAnchors::HCenterAnchor;

            case Qt::AnchorRight:
                return QQuickAnchors::RightAnchor;

            case Qt::AnchorTop:
                return QQuickAnchors::TopAnchor;

            case Qt::AnchorVerticalCenter:
                return QQuickAnchors::VCenterAnchor;

            case Qt::AnchorBottom:
                return QQuickAnchors::BottomAnchor;
        }

        return QQuickAnchors::InvalidAnchor;
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

QskItemAnchors::QskItemAnchors()
{
}

QskItemAnchors::~QskItemAnchors()
{
}

QQuickItem* QskItemAnchors::item() const
{
    return m_anchors ? QQuickAnchorsPrivate::get( m_anchors )->item : nullptr;
}

QMarginsF QskItemAnchors::margins() const
{
    if ( !isValid() )
        return QMarginsF();

    return QMarginsF( m_anchors->leftMargin(), m_anchors->topMargin(),
        m_anchors->rightMargin(), m_anchors->bottomMargin() );
}

void QskItemAnchors::setMargins( const QMarginsF& margins )
{
    m_anchors->setLeftMargin( margins.left() );
    m_anchors->setRightMargin( margins.right() );
    m_anchors->setTopMargin( margins.top() );
    m_anchors->setBottomMargin( margins.bottom() );
}

void QskItemAnchors::addAnchor( QQuickItem* otherItem,
    Qt::AnchorPoint otherEdge, Qt::AnchorPoint edge )
{
    const auto& ops = operators( edge );
    ( m_anchors->*ops.setLine )( { otherItem, toQuickAnchor( otherEdge ) } );
}

void QskItemAnchors::removeAnchor( Qt::AnchorPoint edge )
{
    const auto& ops = operators( edge );
    ( m_anchors->*ops.resetLine ) ();
}

void QskItemAnchors::addAnchors( QQuickItem* otherItem,
    Qt::Corner otherCorner, Qt::Corner corner )
{
    auto anchorPoint =
        []( Qt::Corner corner, Qt::Orientation orientation )
        {
            if ( orientation == Qt::Horizontal )
                return ( corner & 0x1 ) ? Qt::AnchorRight : Qt::AnchorLeft;
            else
                return ( corner >= 0x2 ) ? Qt::AnchorBottom : Qt::AnchorTop;
        };

    addAnchor( otherItem, anchorPoint( otherCorner, Qt::Horizontal ),
        anchorPoint( corner, Qt::Horizontal ) );

    addAnchor( otherItem, anchorPoint( otherCorner, Qt::Vertical ),
        anchorPoint( corner, Qt::Vertical ) );
}

void QskItemAnchors::addAnchors( QQuickItem* otherItem, Qt::Orientations orientations )
{
    if ( orientations & Qt::Horizontal )
    {
        addAnchor( otherItem, Qt::AnchorLeft, Qt::AnchorLeft );
        addAnchor( otherItem, Qt::AnchorRight, Qt::AnchorRight );
    }

    if ( orientations & Qt::Vertical )
    {
        addAnchor( otherItem, Qt::AnchorTop, Qt::AnchorTop );
        addAnchor( otherItem, Qt::AnchorBottom, Qt::AnchorBottom );
    }
}

