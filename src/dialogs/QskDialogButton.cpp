/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButton.h"
#include "QskDialogButtonBox.h"

QSK_SUBCONTROL( QskDialogButton, Panel )
QSK_SUBCONTROL( QskDialogButton, Text )
QSK_SUBCONTROL( QskDialogButton, Graphic )

QskDialogButton::QskDialogButton(
        QskDialog::StandardButton standardButton, QQuickItem* parent ):
    QskPushButton( parent ),
    m_buttonType( standardButton )
{
    setText( QskDialogButtonBox::buttonText( m_buttonType ) );
}

QskDialogButton::QskDialogButton( QQuickItem* parent ):
    QskDialogButton( QskDialog::NoButton, parent )
{
}

QskDialogButton::~QskDialogButton()
{
}

QskAspect::Subcontrol QskDialogButton::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPushButton::Panel )
        return QskDialogButton::Panel;

    if ( subControl == QskPushButton::Text )
        return QskDialogButton::Text;

    if ( subControl == QskPushButton::Graphic )
        return QskDialogButton::Graphic;

    return Inherited::effectiveSubcontrol( subControl );
}

void QskDialogButton::setStandardButton( QskDialog::StandardButton button )
{
    if ( button != m_buttonType )
    {
        m_buttonType = button;
        setText( QskDialogButtonBox::buttonText( m_buttonType ) );

        Q_EMIT standardButtonChanged();
    }
}

QskDialog::StandardButton QskDialogButton::standardButton() const
{
    return m_buttonType;
}

void QskDialogButton::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::LocaleChange )
        setText( QskDialogButtonBox::buttonText( m_buttonType ) );

    Inherited::changeEvent( event );
}

#include "moc_QskDialogButton.cpp"


