/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_VIEW_H
#define QSK_TAB_VIEW_H

#include "QskControl.h"
#include "QskNamespace.h"

class QskTabBar;
class QskTabButton;

class QSK_EXPORT QskTabView : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( QskTabBar* tabBar READ tabBar )

    Q_PROPERTY( Qt::Edge tabBarEdge READ tabBarEdge
        WRITE setTabBarEdge NOTIFY tabBarEdgeChanged FINAL )

    Q_PROPERTY( bool autoFitTabs READ autoFitTabs
        WRITE setAutoFitTabs NOTIFY autoFitTabsChanged FINAL )

    Q_PROPERTY( Qt::Orientation orientation READ orientation )

    Q_PROPERTY( int count READ count NOTIFY countChanged FINAL )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL )

    typedef QskControl Inherited;

  public:
    QSK_SUBCONTROLS( TabBar, Page )

    QskTabView( QQuickItem* parent = nullptr );
    ~QskTabView() override;

    const QskTabBar* tabBar() const;
    QskTabBar* tabBar();

    void setTabBarEdge( Qt::Edge );
    Qt::Edge tabBarEdge() const;

    void setAutoFitTabs( bool );
    bool autoFitTabs() const;

    Qt::Orientation orientation() const;

    int addTab( QskTabButton*, QQuickItem* );
    int insertTab( int index, QskTabButton*, QQuickItem* );

    Q_INVOKABLE int addTab( const QString&, QQuickItem* );
    Q_INVOKABLE int insertTab( int index, const QString&, QQuickItem* );

    Q_INVOKABLE void removeTab( int index );
    Q_INVOKABLE void clear( bool autoDelete = false );

    QQuickItem* itemAt( int index ) const;
    QskTabButton* buttonAt( int index ) const;

    int itemIndex( const QQuickItem* );
    int buttonIndex( const QskTabButton* );

    QQuickItem* currentItem() const;
    QskTabButton* currentButton() const;

    int currentIndex() const;
    int count() const;

    QRectF tabRect() const;

    QskAspect::Placement effectivePlacement() const override;

  public Q_SLOTS:
    void setCurrentIndex( int index );

  Q_SIGNALS:
    void currentIndexChanged( int index );
    void countChanged( int );
    void tabBarEdgeChanged( Qt::Edge );
    void autoFitTabsChanged( bool );

  protected:
    bool event( QEvent* event ) override;
    void updateLayout() override;

    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
