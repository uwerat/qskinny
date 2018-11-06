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

            setAlignment( Qt::AlignHCenter | Qt::AlignTop );
            setWrapMode( QskTextOptions::WordWrap );

            connect( this, &QskTextLabel::textChanged,
                box, &QskMessageSubWindow::infoTextChanged );

            connect( this, &QskTextLabel::textOptionsChanged,
                box, &QskMessageSubWindow::infoTextOptionsChanged );
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
    QskTextLabel* infoTextLabel;
};  

QskMessageSubWindow::QskMessageSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->infoTextLabel = new TextLabel( this );

    m_data->symbolLabel = new SymbolLabel( this );
    m_data->symbolLabel->hide();

    const auto alignment = Qt::AlignTop | Qt::AlignHCenter;

    auto box = new QskLinearBox( Qt::Horizontal );
    box->setSpacing( 0 );
    box->addItem( m_data->symbolLabel, alignment );
    box->addItem( m_data->infoTextLabel, alignment );
    box->setStretchFactor( m_data->infoTextLabel, 10 );

    setContentItem( box );
}

QskMessageSubWindow::~QskMessageSubWindow()
{
}

void QskMessageSubWindow::setInfoText( const QString& text )
{
    m_data->infoTextLabel->setText( text );
}

QString QskMessageSubWindow::infoText() const
{
    return m_data->infoTextLabel->text();
}

void QskMessageSubWindow::setInfoTextOptions( const QskTextOptions& options )
{
    m_data->infoTextLabel->setTextOptions( options );
}

QskTextOptions QskMessageSubWindow::infoTextOptions() const
{
    return m_data->infoTextLabel->textOptions();
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
