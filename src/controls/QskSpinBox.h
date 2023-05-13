/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
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

    Q_PROPERTY( int decimals READ decimals
        WRITE setDecimals NOTIFY decimalsChanged )

    Q_PROPERTY( Qt::Alignment textAlignment READ textAlignment
        WRITE setTextAlignment RESET textAlignment NOTIFY textAlignmentChanged )

    Q_PROPERTY( QString text READ text NOTIFY textChanged )

  public:
    QSK_SUBCONTROLS( Panel, TextPanel, Text,
        UpPanel, UpIndicator, DownPanel, DownIndicator )

    QSK_STATES( Decreasing, Increasing )

    enum Decoration
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

    // Qt::AlignLeft, Qt::AlignRight or Qt::AlignHCenter.
    void setTextAlignment( Qt::Alignment );
    void resetTextAlignment();
    Qt::Alignment textAlignment() const;

    void setWrapping( bool );
    bool isWrapping() const;

    void setDecimals( int );
    int decimals() const;

    QString text() const;
    virtual QString textFromValue( qreal ) const;

    void increment( qreal ) override;

  Q_SIGNALS:
    void decorationChanged( Decoration );
    void textAlignmentChanged( Qt::Alignment );

    void wrappingChanged( bool );

    void decimalsChanged( int );
    void textChanged();

  protected:
    void timerEvent( QTimerEvent* ) override;

    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;
    void mouseUngrabEvent() override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
