/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSelectionWindow.h"
#include "QskGraphicLabel.h"
#include "QskGraphic.h"
#include "QskLinearBox.h"
#include "QskSimpleListBox.h"
#include "QskTextLabel.h"
#include "QskTextOptions.h"

namespace
{
    class TextLabel final : public QskTextLabel
    {
      public:
        TextLabel( QskSelectionWindow* window )
        {
            setObjectName( QStringLiteral( "QskSelectionWindowTextLabel" ) );
            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );

            setAlignment( Qt::AlignLeft | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );

            connect( this, &QskTextLabel::textChanged,
                window, &QskSelectionWindow::infoTextChanged );

            connect( this, &QskTextLabel::textOptionsChanged,
                window, &QskSelectionWindow::infoTextOptionsChanged );
        }
    };

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

class QskSelectionWindow::PrivateData
{
  public:
    QskTextLabel* textLabel;
    QskSimpleListBox* listBox;
};

QskSelectionWindow::QskSelectionWindow( QWindow* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );

    m_data->textLabel = new TextLabel( this );
    m_data->textLabel->setVisible( false );

    m_data->listBox = new ListBox( this );
#if 1
    m_data->listBox->setPreferredSize( 500, 500 );
#endif

    auto box = new QskLinearBox( Qt::Vertical );
    box->setMargins( 5 );
    box->setSpacing( 10 );
    box->addItem( m_data->textLabel );
    box->addItem( m_data->listBox );
    box->setStretchFactor( m_data->listBox, 10 );

    setDialogContentItem( box );
    setDialogActions( QskDialog::Ok | QskDialog::Cancel );
}

QskSelectionWindow::~QskSelectionWindow()
{
}

void QskSelectionWindow::setInfoText( const QString& text )
{
    m_data->textLabel->setText( text );
    m_data->textLabel->setVisible( !text.isEmpty() );
}

QString QskSelectionWindow::infoText() const
{
    return m_data->textLabel->text();
}

void QskSelectionWindow::setInfoTextOptions( const QskTextOptions& options )
{
    if ( options != infoTextOptions() )
    {
        m_data->textLabel->setTextOptions( options );
        Q_EMIT infoTextOptionsChanged( options );
    }
}

QskTextOptions QskSelectionWindow::infoTextOptions() const
{
    return m_data->textLabel->textOptions();
}

void QskSelectionWindow::setEntries( const QStringList& entries )
{
    m_data->listBox->setEntries( entries );
}

QStringList QskSelectionWindow::entries() const
{
    return m_data->listBox->entries();
}

void QskSelectionWindow::setSelectedRow( int row )
{
    m_data->listBox->setSelectedRow( row );
}

int QskSelectionWindow::selectedRow() const
{
    return m_data->listBox->selectedRow();
}

QString QskSelectionWindow::selectedEntry() const
{
    return m_data->listBox->selectedEntry();
}

#include "moc_QskSelectionWindow.cpp"
