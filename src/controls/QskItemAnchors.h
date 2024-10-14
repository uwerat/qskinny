/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ITEM_ANCHORS_H
#define QSK_ITEM_ANCHORS_H

#include "QskGlobal.h"
#include <qnamespace.h>
#include <qpointer.h>

class QMarginsF;
class QQuickItem;

/*
    QskItemAnchors is a C++ API to access the Qt/Quick anchoring.

    Qt/Quick anchoring is a simple concept, that allows to attach
    borders ( Qt::AnchorPoint ) of one item ( attachedItem ) to borders of
    other items ( settledItem ).

    In opposite to anchoring layouts ( f.e QGraphicsAnchorLayout, QskAnchorLayout )
    the positions of the anchors are provided - not calculated. That makes
    Qt/Quick anchoring unsuitable for scenarios, where the space of
    a bounding rectangle has to be distributed.

    The implementation supports attaching to the parent or the siblings
    of attachedItem only ( conceptually this limitation would not be necessary ).

    Note that what is known in QML as "fill" or "centerIn" can be done
    using setBorderAnchors or setCenterAnchors.

    Limitations:
        - access to baseline settings are not implemented
          ( for no other reason than Qt::AnchorPoint does not have it )
 */
class QSK_EXPORT QskItemAnchors
{
  public:
    QskItemAnchors( QQuickItem* attachedItem = nullptr );
    ~QskItemAnchors();

    QQuickItem* attachedItem() const;

    QQuickItem* settledItem( Qt::AnchorPoint ) const;
    Qt::AnchorPoint settledItemAnchorPoint( Qt::AnchorPoint ) const;

    bool operator==( const QskItemAnchors& ) const noexcept;
    bool operator!=( const QskItemAnchors& ) const noexcept;

    QMarginsF margins() const;
    void setMargins( const QMarginsF& );

    void setCenterOffsets( qreal horizontalOffset, qreal verticalOffset );
    void setCenterOffset( Qt::Orientation, qreal offset );
    qreal centerOffset( Qt::Orientation );

    // add to - or remove from - the list of anchors
    void addAnchor( Qt::AnchorPoint, QQuickItem*, Qt::AnchorPoint );
    void addAnchors( Qt::Corner, QQuickItem*, Qt::Corner );
    void removeAnchor( Qt::AnchorPoint );

    void clearAnchors();

    // replacing the list of anchors
    void setBorderAnchors( QQuickItem*, Qt::Orientations = Qt::Horizontal | Qt::Vertical );
    void setCenterAnchors( QQuickItem*, Qt::Orientations = Qt::Horizontal | Qt::Vertical );

  private:
    QPointer< QQuickItem > m_attachedItem;
};

inline bool QskItemAnchors::operator!=(
    const QskItemAnchors& other ) const noexcept
{
    return !( *this == other );
}

#endif
