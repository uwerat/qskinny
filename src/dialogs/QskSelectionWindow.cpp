/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSelectionWindow.h"
#include "QskSimpleListBox.h"

/*
    QInputDialog uses a combo box instead of a list widget
    Guess we should do the same TODO ...
 */
static inline QskSimpleListBox* qskListBox(
    QskSelectionWindow* window )
{
    return qobject_cast< QskSimpleListBox* >( window->dialogContentItem() );
}

static inline const QskSimpleListBox* qskListBox(
    const QskSelectionWindow* window )
{
    return qobject_cast< QskSimpleListBox* >( window->dialogContentItem() );
}

namespace
{
    class ListBox final : public QskSimpleListBox
    {
      public:
        ListBox( QskSelectionWindow* window )
        {
            setObjectName( QStringLiteral( "QskSelectionWindowListBox" ) );

            connect( this, &QskSimpleListBox::selectedRowChanged,
                window, &QskSelectionWindow::selectedRowChanged );

            connect( this, &QskSimpleListBox::selectedEntryChanged,
                window, &QskSelectionWindow::selectedEntryChanged );

            connect( this, &QskSimpleListBox::entriesChanged,
                window, &QskSelectionWindow::entriesChanged );
        }
    };
}

QskSelectionWindow::QskSelectionWindow( QWindow* parent )
    : Inherited( parent )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );

    auto listBox = new ListBox( this );
#if 1
    listBox->setPreferredSize( 500, 500 );
#endif

    setDialogContentItem( listBox );
    setDialogActions( QskDialog::Ok | QskDialog::Cancel );
}

QskSelectionWindow::~QskSelectionWindow()
{
}

void QskSelectionWindow::setEntries( const QStringList& entries )
{
    if ( auto listBox = qskListBox( this ) )
        listBox->setEntries( entries );
}

QStringList QskSelectionWindow::entries() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->entries();

    return QStringList();
}

void QskSelectionWindow::setSelectedRow( int row )
{
    if ( auto listBox = qskListBox( this ) )
        listBox->setSelectedRow( row );
}

int QskSelectionWindow::selectedRow() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->selectedRow();

    return -1;
}

QString QskSelectionWindow::selectedEntry() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->selectedEntry();

    return QString();
}

#include "moc_QskSelectionWindow.cpp"
