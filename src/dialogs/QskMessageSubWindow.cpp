/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMessageSubWindow.h"
#include "QskGraphic.h"
#include "QskGraphicLabel.h"
#include "QskLinearBox.h"
#include "QskSkin.h"
#include "QskTextLabel.h"

#include <qfontmetrics.h>

namespace
{
    class TextLabel final : public QskTextLabel
    {
      public:
        TextLabel( QskMessageSubWindow* box )
        {
            setObjectName( QStringLiteral( "QskMessageSubWindowTextLabel" ) );
            initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );

            setAlignment( Qt::AlignLeft | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );

            connect( this, &QskTextLabel::textChanged,
                box, &QskMessageSubWindow::textChanged );

            connect( this, &QskTextLabel::textOptionsChanged,
                box, &QskMessageSubWindow::textOptionsChanged );
        }
    };

    class SymbolLabel final : public QskGraphicLabel
    {
      public:
        SymbolLabel( QskMessageSubWindow* )
        {
            setObjectName( QStringLiteral( "QskMessageSubWindowSymbolLabel" ) );
            initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );

            setAlignment( Qt::AlignTop | Qt::AlignHCenter );
            updateSourceSize();
        }

      protected:
        void changeEvent( QEvent* event ) override
        {
            if ( event->type() == QEvent::FontChange )
                updateSourceSize();

            QskGraphicLabel::changeEvent( event );
        }

      private:
        void updateSourceSize()
        {
            // when there is no explicit size known,
            // we always adjust the icon according to the font

            if ( sourceSize().isEmpty() )
            {
                const QFont font = effectiveFont( QskTextLabel::Text );
                setPreferredSize( -1.0, 1.5 * QFontMetricsF( font ).height() );
            }
        }
    };
}

class QskMessageSubWindow::PrivateData
{
  public:
    QskGraphicLabel* symbolLabel;
    QskTextLabel* textLabel;
};

QskMessageSubWindow::QskMessageSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->textLabel = new TextLabel( this );

    m_data->symbolLabel = new SymbolLabel( this );
    m_data->symbolLabel->hide();

    auto box = new QskLinearBox( Qt::Horizontal );
    box->setDefaultAlignment( Qt::AlignTop | Qt::AlignHCenter );
    box->setSpacing( 0 );
    box->addItem( m_data->symbolLabel );
    box->addItem( m_data->textLabel );
    box->setStretchFactor( m_data->textLabel, 10 );

    setContentItem( box );
}

QskMessageSubWindow::~QskMessageSubWindow()
{
}

void QskMessageSubWindow::setText( const QString& text )
{
    m_data->textLabel->setText( text );
}

QString QskMessageSubWindow::text() const
{
    return m_data->textLabel->text();
}

void QskMessageSubWindow::setTextOptions( const QskTextOptions& options )
{
    m_data->textLabel->setTextOptions( options );
}

QskTextOptions QskMessageSubWindow::textOptions() const
{
    return m_data->textLabel->textOptions();
}

void QskMessageSubWindow::setSymbolSource( const QUrl& url )
{
    m_data->symbolLabel->setSource( url );
    m_data->symbolLabel->setVisible( !url.isEmpty() );

    if ( auto box = qobject_cast< QskLinearBox* >( contentItem() ) )
        box->setSpacing( m_data->symbolLabel->isVisible() ? 15 : 0 ); // metrics !!
}

void QskMessageSubWindow::setSymbolType( int symbolType )
{
    setSymbol( effectiveSkin()->symbol( symbolType ) );
}

QUrl QskMessageSubWindow::symbolSource() const
{
    return m_data->symbolLabel->source();
}

void QskMessageSubWindow::setSymbol( const QskGraphic& symbol )
{
    m_data->symbolLabel->setVisible( !symbol.isNull() );
    m_data->symbolLabel->setGraphic( symbol );
}

QskGraphic QskMessageSubWindow::symbol() const
{
    return m_data->symbolLabel->graphic();
}

#include "moc_QskMessageSubWindow.cpp"
