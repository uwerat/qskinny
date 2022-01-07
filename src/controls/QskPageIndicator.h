/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PAGE_INDICATOR_H
#define QSK_PAGE_INDICATOR_H

#include "QskControl.h"

class QSK_EXPORT QskPageIndicator : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( int count READ count
        WRITE setCount NOTIFY countChanged FINAL )

    Q_PROPERTY( qreal currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL )

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged FINAL )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Bullet )
    QSK_STATES( Selected )

    QskPageIndicator( QQuickItem* parent = nullptr );
    QskPageIndicator( int count, QQuickItem* parent = nullptr );

    ~QskPageIndicator() override;

    int count() const;
    qreal currentIndex() const;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation );

    qreal valueRatioAt( int index ) const;
    QRectF bulletRect( int index ) const;
    int indexAtPosition( const QPointF& ) const;

    QskAspect::Placement effectivePlacement() const override;

  Q_SIGNALS:
    void countChanged( int );
    void currentIndexChanged( qreal );
    void orientationChanged( Qt::Orientation );
    void pageRequested( int index );

  public Q_SLOTS:
    void setCount( int count );
    void setCurrentIndex( qreal index );

  protected:
    void mousePressEvent( QMouseEvent* e ) override;
    void mouseUngrabEvent() override;
    void mouseReleaseEvent( QMouseEvent* e ) override;

    void keyPressEvent( QKeyEvent* ) override;

#ifndef QT_NO_WHEELEVENT
    void wheelEvent( QWheelEvent* ) override;
#endif

  private:
    void incrementRequested( int );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
