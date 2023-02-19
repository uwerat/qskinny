/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SPIN_BOX_H
#define QSK_SPIN_BOX_H

#include <QskBoundedValueInput.h>

class QSK_EXPORT QskSpinBox : public QskBoundedValueInput
{
    Q_OBJECT
    using Inherited = QskBoundedValueInput;

  public:

    QSK_SUBCONTROLS( Panel, TextPanel, Text,
        IncrementPanel, IncrementIndicator, DecrementPanel, DecrementIndicator )

    QSK_STATES( Pressed )

    QskSpinBox( QQuickItem* parent = nullptr );
    ~QskSpinBox() override;

#if 1
    /*
        -1: decrease
         0: none
         1: increment
     */
    int pressedButton() const;
    int hoveredButton() const;
#endif

  private:
    void hoverEnterEvent( QHoverEvent* event ) override;
    void hoverLeaveEvent( QHoverEvent* event ) override;
    void hoverMoveEvent( QHoverEvent* event ) override;

    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;

    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
