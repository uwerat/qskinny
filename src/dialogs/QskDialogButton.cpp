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
        QskDialog::Action action, QQuickItem* parent )
    : QskPushButton( parent )
    , m_action( action )
{
    setText( QskDialogButtonBox::buttonText( m_action ) );
}

QskDialogButton::QskDialogButton( QQuickItem* parent )
    : QskDialogButton( QskDialog::NoAction, parent )
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

void QskDialogButton::setAction( QskDialog::Action action )
{
    if ( action != m_action )
    {
        m_action = action;
        setText( QskDialogButtonBox::buttonText( m_action ) );

        Q_EMIT actionChanged();
    }
}

QskDialog::Action QskDialogButton::action() const
{
    return m_action;
}

void QskDialogButton::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::LocaleChange )
        setText( QskDialogButtonBox::buttonText( m_action ) );

    Inherited::changeEvent( event );
}

#include "moc_QskDialogButton.cpp"
