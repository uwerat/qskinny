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
        WRITE setDragMargin RESET resetDragMargin NOTIFY dragMarginChanged )

    Q_PROPERTY( bool interactive READ isInteractive
        WRITE setInteractive NOTIFY interactiveChanged )

  public:
    QSK_SUBCONTROLS( Panel )

    QskDrawer( QQuickItem* = nullptr );
    ~QskDrawer() override;

    void setEdge( Qt::Edge );
    Qt::Edge edge() const;

    void setInteractive( bool );
    bool isInteractive() const;

    void setDragMargin( qreal );
    void resetDragMargin();
    qreal dragMargin() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

    QRectF clipRect() const override;
    QRectF focusIndicatorRect() const override;

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );
    void dragMarginChanged( qreal );
    void interactiveChanged( bool );

  protected:
    void itemChange( ItemChange, const ItemChangeData& ) override;
    void gestureEvent( QskGestureEvent* ) override;
    void keyPressEvent( QKeyEvent* ) override;

  private:
    void setSliding( bool );

    void setIntermediate( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
