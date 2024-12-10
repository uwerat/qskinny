/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SPIN_BOX_H
#define QSK_SPIN_BOX_H

#include <QskBoundedValueInput.h>

class QSK_EXPORT QskSpinBox : public QskBoundedValueInput
{
    Q_OBJECT
    using Inherited = QskBoundedValueInput;

    Q_PROPERTY( bool wrapping READ isWrapping
        WRITE setWrapping NOTIFY wrappingChanged )

    Q_PROPERTY( Decoration decoration READ decoration
        WRITE setDecoration RESET resetDecoration NOTIFY decorationChanged )

  public:
    QSK_SUBCONTROLS( Panel, TextPanel, Text,
        UpPanel, UpIndicator, DownPanel, DownIndicator )

    QSK_STATES( Decreasing, Increasing )

    enum Decoration : quint8
    {
        NoDecoration,

        ButtonsLeftAndRight,
        ButtonsRight,
        UpDownControl
    };
    Q_ENUM( Decoration )

    QskSpinBox( QQuickItem* parent = nullptr );
    QskSpinBox( qreal min, qreal max,
        qreal stepSize, QQuickItem* parent = nullptr );

    ~QskSpinBox() override;

    void setDecoration( Decoration );
    void resetDecoration();
    Decoration decoration() const;

    void setWrapping( bool );
    bool isWrapping() const;

    void increment( qreal ) override;

  Q_SIGNALS:
    void decorationChanged( Decoration );
    void wrappingChanged( bool );

  protected:
    void timerEvent( QTimerEvent* ) override;

    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void hoverMoveEvent( QHoverEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
