/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TAB_BAR_H
#define QSK_TAB_BAR_H

#include "QskBox.h"
#include "QskNamespace.h"

class QskTabButton;
class QskTextOptions;

class QSK_EXPORT QskTabBar : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( Qsk::Position tabPosition READ tabPosition
        WRITE setTabPosition NOTIFY tabPositionChanged FINAL )

    Q_PROPERTY( Qt::Orientation orientation READ orientation )

    Q_PROPERTY( bool autoScrollFocusButton READ autoScrollFocusButton
        WRITE setAutoScrollFocusedButton NOTIFY autoScrollFocusedButtonChanged FINAL )

    Q_PROPERTY( bool autoFitTabs READ autoFitTabs
        WRITE setAutoFitTabs NOTIFY autoFitTabsChanged FINAL )

    Q_PROPERTY( int count READ count NOTIFY countChanged FINAL )

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged FINAL )

    Q_PROPERTY( QskTextOptions textOptions READ textOptions
        WRITE setTextOptions NOTIFY textOptionsChanged )

    using Inherited = QskBox;

  public:
    QSK_SUBCONTROLS( Panel )

    QskTabBar( QQuickItem* parent = nullptr );
    QskTabBar( Qsk::Position, QQuickItem* parent = nullptr );

    ~QskTabBar() override;

    void setTabPosition( Qsk::Position );
    Qsk::Position tabPosition() const;

    Qt::Orientation orientation() const;

    void setAutoScrollFocusedButton( bool );
    bool autoScrollFocusButton() const;

    void setAutoFitTabs( bool );
    bool autoFitTabs() const;

    void ensureButtonVisible( const QskTabButton* );

    void setTextOptions( const QskTextOptions& );
    QskTextOptions textOptions() const;

    Q_INVOKABLE int addTab( const QString& text );
    Q_INVOKABLE int insertTab( int index, const QString& text );

    Q_INVOKABLE int addTab( QskTabButton* );
    Q_INVOKABLE int insertTab( int index, QskTabButton* );

    Q_INVOKABLE void removeTab( int index );
    Q_INVOKABLE void clear( bool autoDelete = false );

    bool isTabEnabled( int index ) const;
    void setTabEnabled( int index, bool );

    int currentIndex() const;
    int count() const;

    Q_INVOKABLE QskTabButton* buttonAt( int );
    Q_INVOKABLE const QskTabButton* buttonAt( int ) const;

    Q_INVOKABLE QskTabButton* currentButton();
    Q_INVOKABLE const QskTabButton* currentButton() const;

    Q_INVOKABLE QString currentButtonText() const;
    Q_INVOKABLE QString buttonTextAt( int index ) const;

    int indexOf( const QskTabButton* ) const;
    Q_INVOKABLE int indexOf( QskTabButton* ) const;

    QskAspect::Placement effectivePlacement() const override;

    QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol ) const override;

  public Q_SLOTS:
    void setCurrentIndex( int index );

  Q_SIGNALS:
    void currentIndexChanged( int index );
    void buttonClicked( int index );
    void countChanged( int );
    void textOptionsChanged( const QskTextOptions& );
    void tabPositionChanged( Qsk::Position );
    void autoScrollFocusedButtonChanged( bool );
    void autoFitTabsChanged( bool );

  protected:
    void componentComplete() override;

  private:
    void adjustCurrentIndex();
    void handleButtonClick();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

// Qml does not like const versions
inline int QskTabBar::indexOf( const QskTabButton* tabButton ) const
{
    return QskTabBar::indexOf( const_cast< QskTabButton* >( tabButton ) );
}

#endif
