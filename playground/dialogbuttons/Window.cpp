/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Window.h"

#include <QskDialogButtonBox.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>

Window::Window( Qt::Orientation orientation )
    : m_orientation( orientation )
{
    m_layoutBox = new QskLinearBox( invertedOrientation() );
    m_layoutBox->setObjectName( "MainBox" );
    m_layoutBox->setMargins( 10 );
    m_layoutBox->setExtraSpacingAt( Qt::BottomEdge | Qt::RightEdge );

    addBox( QskDialog::Ok );
    addBox( QskDialog::Ok | QskDialog::Cancel );
    addBox( QskDialog::Yes | QskDialog::No );
    addBox( QskDialog::Save | QskDialog::Discard |
        QskDialog::RestoreDefaults | QskDialog::Help );
    addBox( QskDialog::Abort | QskDialog::Retry | QskDialog::Ignore );

    addActionBox();
    addItem( m_layoutBox );
}

void Window::addBox( QskDialog::Actions actions )
{
    QskDialogButtonBox* box = new QskDialogButtonBox( m_orientation );
    box->setActions( actions );
    box->setObjectName( "DialogBox" );

    m_layoutBox->addItem( box );
}

void Window::addActionBox()
{
    QskPushButton* flipButton = new QskPushButton( "Flip" );
    QskPushButton* centerButton = new QskPushButton( "Center" );

    QskDialogButtonBox* box = new QskDialogButtonBox( m_orientation );
    box->addButton( flipButton, QskDialog::UserRole );
    box->addButton( centerButton, QskDialog::UserRole );
    box->setObjectName( "UserBox" );

    m_layoutBox->addItem( box );

    connect( flipButton, &QskPushButton::clicked, this, &Window::flipOrientation );
    connect( centerButton, &QskPushButton::clicked, this, &Window::centerButtons );
}

void Window::flipOrientation()
{
    auto newBox = new QskLinearBox( m_orientation );
    newBox->setObjectName( m_layoutBox->objectName() );
    newBox->setExtraSpacingAt( m_layoutBox->extraSpacingAt() );
    newBox->setMargins( m_layoutBox->margins() );

    m_orientation = invertedOrientation();

    const QVector< QskDialogButtonBox* > boxes = dialogBoxes();
    for ( QskDialogButtonBox* box : boxes )
    {
        box->setOrientation( m_orientation );
        box->setParent( nullptr );

        newBox->addItem( box );
    }

    delete m_layoutBox;
    m_layoutBox = newBox;

    addItem( m_layoutBox );
}

void Window::centerButtons()
{
    const QVector< QskDialogButtonBox* > boxes = dialogBoxes();
    for ( QskDialogButtonBox* box : boxes )
        box->setCenteredButtons( !box->centeredButtons() );
}

QVector< QskDialogButtonBox* > Window::dialogBoxes() const
{
    QVector< QskDialogButtonBox* > boxes;
    for ( int i = 0; i < m_layoutBox->elementCount(); i++ )
    {
        if ( auto box = qobject_cast< QskDialogButtonBox* >( m_layoutBox->itemAtIndex( i ) ) )
        {
            boxes += box;
        }
    }

    return boxes;
}

Qt::Orientation Window::invertedOrientation() const
{
    if ( m_orientation == Qt::Vertical )
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

#include "moc_Window.cpp"
