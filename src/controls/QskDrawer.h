/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_DRAWER_H
#define QSK_DRAWER_H

#include "QskSlideIn.h"

class QSK_EXPORT QskDrawer : public QskSlideIn
{
    Q_OBJECT

    using Inherited = QskSlideIn;

    Q_PROPERTY( Qt::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged )

    Q_PROPERTY( qreal dragMargin READ dragMargin
        WRITE setDragMargin RESET resetDragMargin NOTIFY dragMarginChanged )

    Q_PROPERTY( bool interactive READ isInteractive
        WRITE setInteractive NOTIFY interactiveChanged )

  public:
    QskDrawer( QQuickItem* = nullptr );
    ~QskDrawer() override;

    void setEdge( Qt::Edge );
    Qt::Edge edge() const override final;

    void setInteractive( bool );
    bool isInteractive() const;

    void setDragMargin( qreal );
    void resetDragMargin();
    qreal dragMargin() const;

  Q_SIGNALS:
    void edgeChanged( Qt::Edge );
    void dragMarginChanged( qreal );
    void interactiveChanged( bool );

  protected:
    bool event( QEvent* ) override;

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void gestureEvent( QskGestureEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
