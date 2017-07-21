/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ABSTRACT_BUTTON_H
#define QSK_ABSTRACT_BUTTON_H

#include "QskGlobal.h"
#include "QskControl.h"

class QSK_EXPORT QskAbstractButton : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged FINAL )

    Q_PROPERTY( bool autoRepeat READ autoRepeat WRITE setAutoRepeat NOTIFY autoRepeatChanged FINAL )
    Q_PROPERTY( int autoRepeatDelay READ autoRepeatDelay
        WRITE setAutoRepeatDelay NOTIFY autoRepeatDelayChanged FINAL )
    Q_PROPERTY( int autoRepeatInterval READ autoRepeatInterval
        WRITE setAutoRepeatInterval NOTIFY autoRepeatIntervalChanged FINAL )

    Q_PROPERTY( bool exclusive READ exclusive
        WRITE setExclusive NOTIFY exclusiveChanged FINAL )

    Q_PROPERTY( bool pressed READ isPressed WRITE setPressed NOTIFY pressedChanged FINAL )
    Q_PROPERTY( bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged FINAL )

    using Inherited = QskControl;

public:
    QSK_STATES( Flat, Checked, Pressed, Checkable )

    QskAbstractButton( QQuickItem* parent = nullptr );
    virtual ~QskAbstractButton();

    void setCheckable( bool );
    bool isCheckable() const;

    bool isChecked() const;

    void setPressed( bool on );
    bool isPressed() const;

    void setAutoRepeat( bool );
    bool autoRepeat() const;

    void setAutoRepeatDelay( int ms );
    int autoRepeatDelay() const;

    void setAutoRepeatInterval( int ms );
    int autoRepeatInterval() const;

    void setExclusive( bool );
    bool exclusive() const;

public Q_SLOTS:
    void toggle();
    void click();
    void setChecked( bool );

Q_SIGNALS:
    void pressed();
    void released();
    void canceled();
    void clicked();
    void toggled( bool );

    void pressedChanged();
    void checkedChanged();
    void checkableChanged();

    void autoRepeatChanged();
    void autoRepeatDelayChanged();
    void autoRepeatIntervalChanged();

    void exclusiveChanged();

protected:
    virtual bool event( QEvent* ) override;

    virtual void keyPressEvent( QKeyEvent* ) override;
    virtual void keyReleaseEvent( QKeyEvent* ) override;

    virtual void mouseMoveEvent( QMouseEvent* ) override;
    virtual void mousePressEvent( QMouseEvent* ) override;
    virtual void mouseReleaseEvent( QMouseEvent* ) override;
    virtual void mouseUngrabEvent() override;

    virtual void focusInEvent( QFocusEvent* ) override;
    virtual void focusOutEvent( QFocusEvent* ) override;

    virtual void timerEvent( QTimerEvent* ) override;

private:
    void releaseButton();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
