/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_VIEW_H
#define QSK_TAB_VIEW_H

#include "QskControl.h"

class QskTabBar;
class QskTabButton;

class QSK_EXPORT QskTabView : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( int count READ count NOTIFY countChanged FINAL )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL )

    typedef QskControl Inherited;

  public:
    QSK_SUBCONTROLS( TabBar, Page )

    QskTabView( QQuickItem* parent = nullptr );
    QskTabView( Qt::Orientation, QQuickItem* parent = nullptr );

    ~QskTabView() override;

    const QskTabBar* tabBar() const;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    int addTab( QskTabButton*, QQuickItem* );
    int insertTab( int index, QskTabButton*, QQuickItem* );

    int addTab( const QString&, QQuickItem* );
    int insertTab( int index, const QString&, QQuickItem* );

    void removeTab( int index );
    void clear();

    QQuickItem* itemAt( int index ) const;
    QskTabButton* buttonAt( int index ) const;

    int itemIndex( QQuickItem* );
    int buttonIndex( QskTabButton* );

    QQuickItem* currentItem() const;
    QskTabButton* currentButton() const;

    int currentIndex() const;
    int count() const;

    QRectF tabRect() const;

    QSizeF contentsSizeHint() const override;

  public Q_SLOTS:
    void setCurrentIndex( int index );

  Q_SIGNALS:
    void currentIndexChanged( int index );
    void countChanged();
    void orientationChanged();

  protected:
    bool event( QEvent* event ) override;
    void updateLayout() override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
