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
class QQuickAnchors;
class QQuickItem;

class QskItemAnchors
{
  public:
    QskItemAnchors();
    ~QskItemAnchors();

    QQuickItem* item() const;

    bool isValid() const;

    bool operator==( const QskItemAnchors& ) const noexcept;
    bool operator!=( const QskItemAnchors& ) const noexcept;

    QMarginsF margins() const;
    void setMargins( const QMarginsF& );

    void addAnchor( QQuickItem*, Qt::AnchorPoint, Qt::AnchorPoint );
    void addAnchors( QQuickItem*, Qt::Corner, Qt::Corner );
    void addAnchors( QQuickItem*, Qt::Orientations = Qt::Horizontal | Qt::Vertical );

    void removeAnchor( Qt::AnchorPoint );

  private:
    QPointer< QQuickAnchors > m_anchors;
};

inline bool QskItemAnchors::operator==(
    const QskItemAnchors& other ) const noexcept
{
    return m_anchors.data() == other.m_anchors.data();
}

inline bool QskItemAnchors::operator!=(
    const QskItemAnchors& other ) const noexcept
{
    return !( *this == other );
}

inline bool QskItemAnchors::isValid() const
{
    return !m_anchors.isNull();
}

#endif
