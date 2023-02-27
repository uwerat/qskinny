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

    Q_PROPERTY( bool wrapping READ isWrapping
        WRITE setWrapping NOTIFY wrappingChanged )

    Q_PROPERTY( bool tracking READ isTracking
        WRITE setTracking NOTIFY trackingChanged )

    Q_PROPERTY( bool accelerating READ isAccelerating
        WRITE setAccelerating NOTIFY acceleratingChanged )

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

        Buttons,
        UpDownControl
    };
    Q_ENUM( Decoration )

    QskSpinBox( QQuickItem* parent = nullptr );
    QskSpinBox( qreal min, qreal max, qreal stepSize,
         QQuickItem* parent = nullptr );

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

    void setTracking( bool );
    bool isTracking() const;

    void setAccelerating( bool );
    bool isAccelerating() const;

    void setDecimals( int );
    int decimals() const;

    QString text() const;
    virtual QString textFromValue( qreal ) const;

  Q_SIGNALS:
    void decorationChanged( Decoration );
    void textAlignmentChanged( Qt::Alignment );

    void trackingChanged( bool );
    void wrappingChanged( bool );
    void acceleratingChanged( bool );

    void decimalsChanged( int );
    void textChanged();

  private:
    void timerEvent( QTimerEvent* ) override;

    void mouseReleaseEvent( QMouseEvent* ) override;
    void mousePressEvent( QMouseEvent* ) override;

    void keyPressEvent( QKeyEvent* ) override;
    void keyReleaseEvent( QKeyEvent* ) override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
