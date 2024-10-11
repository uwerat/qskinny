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
    QskItemAnchors is a C++ API to access the Qt/Quick anchoring,
    that has been designed to be used from QML.

    Qt/Quick anchoring is a simple concept, that allows to

    - attach a border ( Qt::AnchorPoint ) of attachedItem to a border of a baseItem
    - center attachedItem to the center of a controlItem

    The Qt/Quick implementation supports attaching/centering to the parent or
    the siblings of attachedItem only ( conceptually this limitation
    would not be necessary ).

    While it is possible to have attachments for each border you can't
    center and attach at the same time.

    The expected logic would be, that defining an attachment disables
    centering and v.v. - however the implementation tolerates conflicts.
    Even worse is is possible to define centering ( = centerIn ) and
    center-/resizing ( = fill ) to different items at the same time.

    These conflicts are resolved by applying only one of the definitions in
    the following precedence:

        1) fill
        2) centerIn
        3) anchors

    Note that Qt/Quick ( in opposite to Qt/GraphicsView ) anchoring
    is not capable of handling typical layout scenarios, like distributing
    the space of a bounding rectangle to a chain of anchored children. 
    For those you can use QskAnchorLayout/QskAnchorBox.

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

    QQuickItem* baseItem( Qt::AnchorPoint ) const;
    Qt::AnchorPoint basePosition( Qt::AnchorPoint ) const;

    bool operator==( const QskItemAnchors& ) const noexcept;
    bool operator!=( const QskItemAnchors& ) const noexcept;

    QMarginsF margins() const;
    void setMargins( const QMarginsF& );

    void setCenterOffset( Qt::Orientation, qreal offset );
    qreal centerOffset( Qt::Orientation );

    void addAnchor( Qt::AnchorPoint, QQuickItem*, Qt::AnchorPoint );
    void addAnchors( Qt::Corner, QQuickItem*, Qt::Corner );
    void addAnchors( QQuickItem*, Qt::Orientations = Qt::Horizontal | Qt::Vertical );

    void removeAnchor( Qt::AnchorPoint );

    /*
        Qt/Quick anchoring knows the convenience modes "fill" and "centerIn".
        Internally these modes are not(!) mapped to anchor definitions.

        Both modes are setting the center point of the attachedItem to the center
        of the controlItem. "fill" also adjusts the size.
     */

    QQuickItem* controlItem( bool adjustSize ) const;
    void setControlItem( QQuickItem*, bool adjustSize );
    void removeControlItem( bool adjustSize );

  private:
    QPointer< QQuickItem > m_attachedItem;
};

inline bool QskItemAnchors::operator!=(
    const QskItemAnchors& other ) const noexcept
{
    return !( *this == other );
}

#endif
