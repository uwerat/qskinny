/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSelectionSubWindow.h"
#include "QskSimpleListBox.h"

/*
    QInputDialog uses a combo box instead of a list widget
    Guess we should do the same TODO ...
 */
static inline QskSimpleListBox* qskListBox(
    QskSelectionSubWindow* subWindow )
{
    return qobject_cast< QskSimpleListBox* >( subWindow->contentItem() );
}

static inline const QskSimpleListBox* qskListBox(
    const QskSelectionSubWindow* subWindow )
{
    return qobject_cast< QskSimpleListBox* >( subWindow->contentItem() );
}

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
    };
}

QskSelectionSubWindow::QskSelectionSubWindow( QQuickItem* parent )
    : Inherited( parent )
{
    auto listBox = new ListBox( this );
#if 1
    listBox->setPreferredSize( 500, 500 );
#endif

    setContentItem( listBox );
    setDialogActions( QskDialog::Ok | QskDialog::Cancel );
}

QskSelectionSubWindow::~QskSelectionSubWindow()
{
}

void QskSelectionSubWindow::setEntries( const QStringList& entries )
{
    if ( auto listBox = qskListBox( this ) )
        listBox->setEntries( entries );
}

QStringList QskSelectionSubWindow::entries() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->entries();

    return QStringList();
}

void QskSelectionSubWindow::setSelectedRow( int row )
{
    if ( auto listBox = qskListBox( this ) )
        listBox->setSelectedRow( row );
}

int QskSelectionSubWindow::selectedRow() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->selectedRow();

    return -1;
}

QString QskSelectionSubWindow::selectedEntry() const
{
    if ( auto listBox = qskListBox( this ) )
        return listBox->selectedEntry();

    return QString();
}

#include "moc_QskSelectionSubWindow.cpp"
