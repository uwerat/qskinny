/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCROLL_VIEW_H
#define QSK_SCROLL_VIEW_H

#include "QskControl.h"

class QSK_EXPORT QskScrollView : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QPointF scrollPos READ scrollPos WRITE setScrollPos NOTIFY scrollPosChanged FINAL )

    Q_PROPERTY( Qt::ScrollBarPolicy verticalScrollBarPolicy READ verticalScrollBarPolicy
        WRITE setVerticalScrollBarPolicy NOTIFY verticalScrollBarPolicyChanged FINAL )

    Q_PROPERTY( Qt::ScrollBarPolicy horizontalScrollBarPolicy READ horizontalScrollBarPolicy
        WRITE setHorizontalScrollBarPolicy NOTIFY horizontalScrollBarPolicyChanged FINAL )

    Q_PROPERTY( Qt::Orientations flickableOrientations READ flickableOrientations
        WRITE setFlickableOrientations NOTIFY flickableOrientationsChanged FINAL )

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel, Viewport,
        HorizontalScrollBar, HorizontalScrollHandle,
        VerticalScrollBar, VerticalScrollHandle )

    QSK_STATES( VerticalHandlePressed, HorizontalHandlePressed )

    QskScrollView( QQuickItem* parent = nullptr );
    virtual ~QskScrollView();

    void setVerticalScrollBarPolicy( Qt::ScrollBarPolicy );
    Qt::ScrollBarPolicy verticalScrollBarPolicy() const;

    void setHorizontalScrollBarPolicy( Qt::ScrollBarPolicy );
    Qt::ScrollBarPolicy horizontalScrollBarPolicy() const;

    void setFlickableOrientations( Qt::Orientations );
    Qt::Orientations flickableOrientations() const;

    int flickRecognizerTimeout() const;
    void setFlickRecognizerTimeout( int timeout );

    QPointF scrollPos() const;
    bool isScrolling( Qt::Orientation ) const;

    Qt::Orientations scrollableOrientations() const;
    QSizeF scrollableSize() const;

    QRectF viewContentsRect() const;
    virtual QRectF gestureRect() const override;

Q_SIGNALS:
    void scrolledTo( const QPointF& );
    void scrollPosChanged();

    void verticalScrollBarPolicyChanged();
    void horizontalScrollBarPolicyChanged();

    void flickableOrientationsChanged();

public Q_SLOTS:
    void setScrollPos( const QPointF& );
    void scrollTo( const QPointF& );

    void ensureVisible( const QPointF& );
    void ensureVisible( const QRectF& );

protected:
    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent( QMouseEvent* ) override;
    virtual void geometryChangeEvent( QskGeometryChangeEvent* ) override;
    virtual void gestureEvent( QskGestureEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent( QWheelEvent* ) override;
#endif

    virtual bool gestureFilter( QQuickItem*, QEvent* ) override;

    void setScrollableSize( const QSizeF& );

private:
    QPointF boundedScrollPos( const QPointF& ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
