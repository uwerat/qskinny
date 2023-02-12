#ifndef QSK_RADIO_BOX_H
#define QSK_RADIO_BOX_H

#include "QskControl.h"

#include <QStringList>

class QSK_EXPORT QskRadioBox : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( int selectedIndex
		READ selectedIndex
		WRITE setSelectedIndex
		NOTIFY selectedIndexChanged FINAL )

    Q_PROPERTY( QStringList items
		READ items
		WRITE setItems
		NOTIFY itemsChanged FINAL )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Button, Symbol, Text, Ripple )
    QSK_STATES( Selected, Pressed, Focused )

    QskRadioBox( QQuickItem* parent = nullptr );
    QskRadioBox( const QStringList&, QQuickItem* parent = nullptr );
    QskRadioBox( const QStringList&, int, QQuickItem* parent = nullptr );
    
    const QStringList& items() const;
    int selectedIndex() const;
    int pressedIndex() const;

  public Q_SLOTS:
    void setSelectedIndex( int );
    void setItems( const QStringList& );

  Q_SIGNALS:
    void selectedIndexChanged( int );
    void itemsChanged( const QStringList& );

 protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;

    int indexAt( const QPointF& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
