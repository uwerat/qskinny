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

    addBox( QskDialog::StandardButtons( QskDialog::Ok | QskDialog::Cancel ) );

    addBox( QskDialog::StandardButtons( QskDialog::Yes | QskDialog::No ) );

    addBox( QskDialog::StandardButtons( QskDialog::Save | QskDialog::Discard
        | QskDialog::RestoreDefaults | QskDialog::Help ) );

    addBox( QskDialog::StandardButtons( QskDialog::Abort
        | QskDialog::Retry | QskDialog::Ignore ) );

    addActionBox();
    addItem( m_layoutBox );
}

void Window::addBox( QskDialog::StandardButtons standardButtons )
{
    QskDialogButtonBox* box = new QskDialogButtonBox( m_orientation );
    box->setStandardButtons( standardButtons );
    box->setObjectName( "DialogBox" );

    m_layoutBox->addItem( box );
}

void Window::addActionBox()
{
    QskPushButton* flipButton = new QskPushButton( "Flip" );
    QskPushButton* centerButton = new QskPushButton( "Center" );

    QskDialogButtonBox* box = new QskDialogButtonBox( m_orientation );
    box->addButton( flipButton, QskDialog::ActionRole );
    box->addButton( centerButton, QskDialog::ActionRole );
    box->setObjectName( "ActionBox" );

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
    m_layoutBox->setActive( false );

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
    for ( int i = 0; i < m_layoutBox->itemCount(); i++ )
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
