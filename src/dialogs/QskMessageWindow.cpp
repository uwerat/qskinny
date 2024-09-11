/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMessageWindow.h"
#include "QskTextLabel.h"
#include "QskTextOptions.h"

namespace
{
    class TextLabel final : public QskTextLabel
    {
      public:
        TextLabel( QskMessageWindow* box )
        {
            setObjectName( QStringLiteral( "QskMessageWindowTextLabel" ) );
            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );

            setAlignment( Qt::AlignLeft | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );

            connect( this, &QskTextLabel::textChanged,
                box, &QskMessageWindow::textChanged );

            connect( this, &QskTextLabel::textOptionsChanged,
                box, &QskMessageWindow::textOptionsChanged );
        }
    };
}

class QskMessageWindow::PrivateData
{
  public:
    QskTextLabel* textLabel;
};

QskMessageWindow::QskMessageWindow( QWindow* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setFlags( Qt::Dialog | Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint );

    m_data->textLabel = new TextLabel( this );
    setDialogContentItem( m_data->textLabel );
}

QskMessageWindow::~QskMessageWindow()
{
}

void QskMessageWindow::setText( const QString& text )
{
    m_data->textLabel->setText( text );
}

QString QskMessageWindow::text() const
{
    return m_data->textLabel->text();
}

void QskMessageWindow::setTextOptions( const QskTextOptions& options )
{
    m_data->textLabel->setTextOptions( options );
}

QskTextOptions QskMessageWindow::textOptions() const
{
    return m_data->textLabel->textOptions();
}

#include "moc_QskMessageWindow.cpp"
