/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButton.h"
#include "QskDialogButtonBox.h"
#include "QskSkin.h"

QSK_SUBCONTROL( QskDialogButton, Panel )
QSK_SUBCONTROL( QskDialogButton, Text )
QSK_SUBCONTROL( QskDialogButton, Graphic )

static QString qskButtonText(
    const QskDialogButton* button, QskDialog::Action action )
{
    if ( const auto skin = button->effectiveSkin() )
        return skin->dialogButtonText( action );

    return QString();
}

QskDialogButton::QskDialogButton(
        QskDialog::Action action, QQuickItem* parent )
    : QskPushButton( parent )
    , m_action( action )
{
    setText( qskButtonText( this, m_action ) );
}

QskDialogButton::QskDialogButton( QQuickItem* parent )
    : QskDialogButton( QskDialog::NoAction, parent )
{
}

QskDialogButton::~QskDialogButton()
{
}

QskAspect::Subcontrol QskDialogButton::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPushButton::Panel )
        return QskDialogButton::Panel;

    if ( subControl == QskPushButton::Text )
        return QskDialogButton::Text;

    if ( subControl == QskPushButton::Graphic )
        return QskDialogButton::Graphic;

    return Inherited::substitutedSubcontrol( subControl );
}

void QskDialogButton::setAction( QskDialog::Action action )
{
    if ( action != m_action )
    {
        m_action = action;
        setText( qskButtonText( this, action ) );

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
            setText( qskButtonText( this, m_action ) );
    }

    Inherited::changeEvent( event );
}

#include "moc_QskDialogButton.cpp"
