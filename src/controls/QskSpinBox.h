/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SPIN_BOX_H
#define QSK_SPIN_BOX_H

#include <QskBoundedInput.h>

class QSK_EXPORT QskSpinBox : public QskBoundedInput
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value NOTIFY valueChanged)

    using Inherited = QskBoundedInput;
public:
    QSK_SUBCONTROLS(Inc, Dec, IncText, DecText, TextPanel, Text, Layout)
    QSK_STATES( Pressed )

    explicit QskSpinBox( QQuickItem* parent = nullptr );
    ~QskSpinBox() override;

    void increment( qreal offset ) override;
    qreal value() const;

Q_SIGNALS:
    void valueChanged( qreal );
    void focusIndexChanged( int );

private:
    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;
    void hoverMoveEvent( QHoverEvent* ) override;

    void mouseReleaseEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    void focusInEvent( QFocusEvent* ) override;
    QRectF focusIndicatorRect() const override;

    class PrivateData;
    std::unique_ptr<PrivateData> m_data;
};

#endif
