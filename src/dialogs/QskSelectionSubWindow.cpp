/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSelectionSubWindow.h"
#include "QskSimpleListBox.h"

namespace
{
    class ListBox final : public QskSimpleListBox
    {
      public:
        ListBox( QskSelectionSubWindow* subWindow )
        {
            setObjectName( QStringLiteral( "QskSelectionSubWindowListBox" ) );

            connect( this, &QskSimpleListBox::selectedRowChanged,
                subWindow, &QskSelectionSubWindow::selectedRowChanged );

            connect( this, &QskSimpleListBox::selectedEntryChanged,
                subWindow, &QskSelectionSubWindow::selectedEntryChanged );

            connect( this, &QskSimpleListBox::entriesChanged,
                subWindow, &QskSelectionSubWindow::entriesChanged );
        }

#if 1
        // how to find a reasonable default size ???
        QSizeF contentsSizeHint() const override
        {
            return QSizeF( 500, 500 );
        }
#endif
    };
}

QskSelectionSubWindow::QskSelectionSubWindow( QQuickItem* parent )
    : Inherited( parent )
{
    setInputControl( new ListBox( this ) );
    setActions( QskDialog::Ok | QskDialog::Cancel );
}

QskSelectionSubWindow::~QskSelectionSubWindow()
{
}

void QskSelectionSubWindow::setEntries( const QStringList& entries )
{
    auto listBox = static_cast< ListBox* >( inputControl() );
    listBox->setEntries( entries );
}

QStringList QskSelectionSubWindow::entries() const
{
    const auto* listBox = static_cast< const ListBox* >( inputControl() );
    return listBox->entries();
}

void QskSelectionSubWindow::setSelectedRow( int row )
{
    auto* listBox = static_cast< ListBox* >( inputControl() );
    listBox->setSelectedRow( row );
}

int QskSelectionSubWindow::selectedRow() const
{
    const auto listBox = static_cast< const ListBox* >( inputControl() );
    return listBox->selectedRow();
}

QString QskSelectionSubWindow::selectedEntry() const
{
    const auto listBox = static_cast< const ListBox* >( inputControl() );
    return listBox->selectedEntry();
}

#include "moc_QskSelectionSubWindow.cpp"
