/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButton.h"
#include "QskDialogButtonBox.h"
#include "QskSkin.h"

QskDialogButton::QskDialogButton(
        QskDialog::Action action, QQuickItem* parent )
    : QskPushButton( parent )
    , m_action( action )
{
    resetButton();
}

QskDialogButton::QskDialogButton( QQuickItem* parent )
    : QskDialogButton( QskDialog::NoAction, parent )
{
}

QskDialogButton::~QskDialogButton()
{
}

void QskDialogButton::setAction( QskDialog::Action action )
{
    if ( action != m_action )
    {
        m_action = action;
        resetButton();

        Q_EMIT actionChanged();
    }
}

QskDialog::Action QskDialogButton::action() const
{
    return m_action;
}

void QskDialogButton::changeEvent( QEvent* event )
{
    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::LocaleChange:
        case QEvent::StyleChange:
            resetButton();
            break;
    }

    Inherited::changeEvent( event );
}

void QskDialogButton::resetButton()
{
    if ( const auto skin = effectiveSkin() )
        setText( skin->dialogButtonText( m_action ) );
}

#include "moc_QskDialogButton.cpp"
