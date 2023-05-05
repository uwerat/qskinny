/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_RADIO_BOX_H
#define QSK_RADIO_BOX_H

#include "QskControl.h"
#include <qstringlist.h>

class QSK_EXPORT QskRadioBox : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( int selectedIndex READ selectedIndex
        WRITE setSelectedIndex NOTIFY selectedIndexChanged FINAL )

    Q_PROPERTY( QStringList options READ options
        WRITE setOptions NOTIFY optionsChanged FINAL )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel, Button, CheckIndicatorPanel, CheckIndicator, Text, Ripple )
    QSK_STATES( Selected, Pressed )

    QskRadioBox( QQuickItem* parent = nullptr );
    QskRadioBox( const QStringList&, QQuickItem* parent = nullptr );
    QskRadioBox( const QStringList&, int, QQuickItem* parent = nullptr );

    ~QskRadioBox() override;

    QRectF focusIndicatorRect() const override;

    QStringList options() const;
    QString optionAt( int ) const;

    int selectedIndex() const;
    int pressedIndex() const;

  public Q_SLOTS:
    void setSelectedIndex( int );
    void setOptions( const QStringList& );

  Q_SIGNALS:
    void selectedIndexChanged( int );
    void optionsChanged( const QStringList& );

  protected:
    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;

    int indexAt( const QPointF& ) const;

  private:
    void setHoveredIndex( int index );
    void setFocusedIndex( int index );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
