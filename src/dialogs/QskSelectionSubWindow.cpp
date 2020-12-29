/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSelectionSubWindow.h"
#include "QskLinearBox.h"
#include "QskSimpleListBox.h"
#include "QskTextLabel.h"

namespace
{
    class TextLabel final : public QskTextLabel
    {
      public:
        TextLabel( QskSelectionSubWindow* subWindow )
        {
            setObjectName( QStringLiteral( "QskSelectionSubWindowTextLabel" ) );
            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Constrained );

            setAlignment( Qt::AlignLeft | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );

            connect( this, &QskTextLabel::textChanged,
                subWindow, &QskSelectionSubWindow::infoTextChanged );

            connect( this, &QskTextLabel::textOptionsChanged,
                subWindow, &QskSelectionSubWindow::infoTextOptionsChanged );
        }
    };

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

class QskSelectionSubWindow::PrivateData
{
  public:
    QskTextLabel* textLabel;
    QskSimpleListBox* listBox;
};

QskSelectionSubWindow::QskSelectionSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    m_data->textLabel = new TextLabel( this );
    m_data->textLabel->setVisible( false );

    m_data->listBox = new ListBox( this );
#if 1
    m_data->listBox->setPreferredSize( 500, 500 );
#endif

    auto box = new QskLinearBox( Qt::Vertical );
    box->setSpacing( 10 ); // hint
    box->addItem( m_data->textLabel );
    box->addItem( m_data->listBox );
    box->setStretchFactor( 1, 10 );

    setContentItem( box );
    setDialogActions( QskDialog::Ok | QskDialog::Cancel );
}

QskSelectionSubWindow::~QskSelectionSubWindow()
{
}

void QskSelectionSubWindow::setInfoText( const QString& text )
{
    m_data->textLabel->setText( text );
    m_data->textLabel->setVisible( !text.isEmpty() );
}

QString QskSelectionSubWindow::infoText() const
{
    return m_data->textLabel->text();
}

void QskSelectionSubWindow::setInfoTextOptions( const QskTextOptions& options )
{
    if ( options != infoTextOptions() )
    {
        m_data->textLabel->setTextOptions( options );
        Q_EMIT infoTextOptionsChanged( options );
    }
}

QskTextOptions QskSelectionSubWindow::infoTextOptions() const
{
    return m_data->textLabel->textOptions();
}

void QskSelectionSubWindow::setEntries( const QStringList& entries )
{
    m_data->listBox->setEntries( entries );
}

QStringList QskSelectionSubWindow::entries() const
{
    return m_data->listBox->entries();
}

void QskSelectionSubWindow::setSelectedRow( int row )
{
    m_data->listBox->setSelectedRow( row );
}

int QskSelectionSubWindow::selectedRow() const
{
    return m_data->listBox->selectedRow();
}

QString QskSelectionSubWindow::selectedEntry() const
{
    return m_data->listBox->selectedEntry();
}

#include "moc_QskSelectionSubWindow.cpp"
