/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SCROLL_VIEW_H
#define QSK_SCROLL_VIEW_H

#include "QskScrollBox.h"

class QSK_EXPORT QskScrollView : public QskScrollBox
{
    Q_OBJECT

    Q_PROPERTY( Qt::ScrollBarPolicy verticalScrollBarPolicy READ verticalScrollBarPolicy
        WRITE setVerticalScrollBarPolicy NOTIFY verticalScrollBarPolicyChanged FINAL )

    Q_PROPERTY( Qt::ScrollBarPolicy horizontalScrollBarPolicy READ horizontalScrollBarPolicy
        WRITE setHorizontalScrollBarPolicy NOTIFY horizontalScrollBarPolicyChanged FINAL )

    using Inherited = QskScrollBox;

  public:
    QSK_SUBCONTROLS( Panel, Viewport,
        HorizontalScrollBar, HorizontalScrollHandle,
        VerticalScrollBar, VerticalScrollHandle )

    QSK_STATES( Pressed )

    QskScrollView( QQuickItem* parent = nullptr );
    ~QskScrollView() override;

    void setVerticalScrollBarPolicy( Qt::ScrollBarPolicy );
    Qt::ScrollBarPolicy verticalScrollBarPolicy() const;

    void setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy );
    Qt::ScrollBarPolicy horizontalScrollBarPolicy() const;

    Qt::Orientations scrollableOrientations() const;

    bool isScrolling( Qt::Orientation ) const;
    QskAspect::States scrollBarStates( QskAspect::Subcontrol ) const;

    QRectF viewContentsRect() const override;
    QskAnimationHint flickHint() const override;

  Q_SIGNALS:
    void verticalScrollBarPolicyChanged();
    void horizontalScrollBarPolicyChanged();

  protected:
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    QPointF scrollOffset( const QWheelEvent* ) const override;
#endif

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
