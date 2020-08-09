/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCROLL_BOX_H
#define QSK_SCROLL_BOX_H

#include "QskControl.h"

class QSK_EXPORT QskScrollBox : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QPointF scrollPos READ scrollPos
        WRITE setScrollPos NOTIFY scrollPosChanged FINAL )

    Q_PROPERTY( Qt::Orientations flickableOrientations READ flickableOrientations
        WRITE setFlickableOrientations NOTIFY flickableOrientationsChanged FINAL )

    Q_PROPERTY( bool autoScrollFocusedItem READ autoScrollFocusItem
        WRITE setAutoScrollFocusedItem NOTIFY autoScrollFocusedItemChanged FINAL )

    using Inherited = QskControl;

  public:
    QskScrollBox( QQuickItem* parent = nullptr );
    ~QskScrollBox() override;

    void setAutoScrollFocusedItem( bool on );
    bool autoScrollFocusItem() const;

    void setFlickableOrientations( Qt::Orientations );
    Qt::Orientations flickableOrientations() const;

    int flickRecognizerTimeout() const;
    void setFlickRecognizerTimeout( int timeout );

    virtual QskAnimationHint flickHint() const = 0;

    QPointF scrollPos() const;
    QSizeF scrollableSize() const;

    virtual QRectF viewContentsRect() const = 0;
    QRectF gestureRect() const override;

  Q_SIGNALS:
    void scrolledTo( const QPointF& );
    void scrollPosChanged();
    void scrollableSizeChanged( const QSizeF& );

    void autoScrollFocusedItemChanged( bool );
    void flickableOrientationsChanged();

  public Q_SLOTS:
    void setScrollPos( const QPointF& );
    void scrollTo( const QPointF& );

    void ensureItemVisible( const QQuickItem* );
    void ensureFocusItemVisible();
    void ensureVisible( const QPointF& );
    void ensureVisible( const QRectF& );

  protected:
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    void gestureEvent( QskGestureEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
    virtual QPointF scrollOffset( const QWheelEvent* ) const;
#endif

    bool gestureFilter( QQuickItem*, QEvent* ) override;
    void setScrollableSize( const QSizeF& );

  private:
    QPointF boundedScrollPos( const QPointF& ) const;
    void onFocusItemChanged();
    void connectWindow( const QQuickWindow*, bool on );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
