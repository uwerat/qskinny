/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_DRAWER_H
#define QSK_DRAWER_H

#include "QskPopup.h"
#include <qnamespace.h>

class QSK_EXPORT QskDrawer : public QskPopup
{
    Q_OBJECT

    using Inherited = QskPopup;

    Q_PROPERTY( Qt::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged )

    Q_PROPERTY( qreal dragMargin READ dragMargin
        WRITE setDragMargin NOTIFY dragMarginChanged )

  public:
    QSK_SUBCONTROLS( Panel )

    QskDrawer( QQuickItem* = nullptr );
    ~QskDrawer() override;

    void setEdge( Qt::Edge );
    Qt::Edge edge() const;

    void setDragMargin( qreal );
    qreal dragMargin() const;

    void updateLayout() override;

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );
    void dragMarginChanged( qreal );

  protected:
    void itemChange( ItemChange, const ItemChangeData& ) override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;
    void gestureEvent( QskGestureEvent* ) override;

  private:
    void setFading( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
