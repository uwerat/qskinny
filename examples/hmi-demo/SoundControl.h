#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QskBox.h>
#include <QskControl.h>
#include <QskPushButton.h>

class QskGridBox;

class FilledRectangle : public QskPushButton // ### move to some main control file
{
public:
    QSK_SUBCONTROLS( Panel )

    FilledRectangle( QQuickItem* parent ): QskPushButton( parent )
    {
        setFlat( true );
        setFixedWidth( 80 ); // ### style
    }

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override final;
};

class BalanceFadeBox : public QskPushButton
{
public:
    QSK_SUBCONTROLS( Panel )

    BalanceFadeBox( QQuickItem* parent ): QskPushButton( parent )
    {
    }

    virtual QskAspect::Subcontrol effectiveSubcontrol(
        QskAspect::Subcontrol subControl ) const override final;
};

class SoundControl : public QskControl
{
public:
    SoundControl( QQuickItem* parent = nullptr );
};

#endif // SOUNDCONTROL_H
