/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GlyphBrowser.h"
#include "GraphicProvider.h"

#include <QskAnimationHint.h>
#include <QskAspect.h>
#include <QskColorFilter.h>
#include <QskGradient.h>
#include <QskGraphic.h>
#include <QskGraphicLabel.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskSpinBox.h>
#include <QskRgbValue.h>
#include <QskSkinManager.h>
#include <QskSkin.h>
#include <QskBoxShapeMetrics.h>

class GlyphSpinBox : public QskSpinBox
{
    Q_OBJECT

  public:
    GlyphSpinBox( QQuickItem* parent = nullptr )
        : QskSpinBox( 0, 2000, 1.0, parent )
    {
        setDecimals( 0 );
        setValue( 100 );

        connect( this, &QskSpinBox::valueChanged,
            this, [this]() { Q_EMIT indexChanged( index() ); } );
    }

    void setGlyphCount( uint count )
    {
        setMaximum( count );
    }

    uint index() const { return static_cast< uint >( value() ); }
    void setIndex( uint index ) { setValue( index ); }

  Q_SIGNALS:
    void indexChanged( uint );
};

class GraphicLabel : public QskGraphicLabel
{
  public:
    enum Role
    {
        Normal = 1000,
        Inverted
    };

    GraphicLabel( QQuickItem* parent = nullptr )
        : QskGraphicLabel( parent )
    {
        setMargins( 10 );
        setPanel( true );

        setBoxShapeHint( Panel, 8 );
        setAlignment( Qt::AlignCenter );
        setDarknessMode( false );

    }

    void setDarknessMode( bool on )
    {
        const int oldRole = graphicRole();

        QskGradient gradient;
        if ( on )
        {
            gradient = QskGradient( qRgb( 40, 40, 40 ) );
            setGraphicRole( Inverted );
        }
        else
        {
            gradient = QskGradient( QskRgb::Wheat );
            setGraphicRole( Normal );
        }

        const int duration = 500;

        const auto oldGradient = gradientHint( Panel );
        setGradientHint( Panel, gradient );

        // finally setup a smooth transition manually
        startTransition( Panel | QskAspect::Color, duration,
            QVariant::fromValue( oldGradient ), QVariant::fromValue( gradient ) );

        startTransition( Graphic | QskAspect::GraphicRole,
            duration, oldRole, graphicRole() );
    }
};

GlyphBrowser::GlyphBrowser( QQuickItem* parentItem )
    : QskControl( parentItem )
{
    setAutoLayoutChildren( true );

    m_graphicProvider = new GraphicProvider( this );

    m_graphicLabel = new GraphicLabel();

    auto spinBox = new GlyphSpinBox();

    auto invertButton = new QskPushButton( "Inverted" );
    invertButton->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    invertButton->setCheckable( true );
    invertButton->setLayoutAlignmentHint( Qt::AlignRight );

    auto hBox = new QskLinearBox( Qt::Horizontal );
    hBox->addItem( spinBox );
    hBox->addItem( invertButton );

    auto box = new QskLinearBox( Qt::Vertical, this );
    box->setPanel( true );
    box->setPadding( 5 );
    box->addItem( hBox );
    box->addItem( m_graphicLabel );

    connect( spinBox, &GlyphSpinBox::indexChanged,
        this, &GlyphBrowser::showGlyph );

    connect( invertButton, &QskPushButton::toggled,
        m_graphicLabel, &GraphicLabel::setDarknessMode );

    connect( qskSkinManager, &QskSkinManager::skinChanged,
        this, &GlyphBrowser::setGraphicRoles );

    setGraphicRoles( qskSkinManager->skin() );
    showGlyph( spinBox->index() );
}

void GlyphBrowser::setGraphicRoles( QskSkin* skin )
{
    // substituting black
    QskColorFilter colorFilter;
    colorFilter.addColorSubstitution( QskRgb::Black, QskRgb::DarkKhaki );

    skin->setGraphicFilter( GraphicLabel::Inverted, colorFilter );
}

void GlyphBrowser::showGlyph( uint index )
{
    m_graphicLabel->setGraphic( m_graphicProvider->glyphGraphic( index ) );
}

void GlyphBrowser::changeEvent( QEvent* event )
{
    if ( event->type() == QEvent::StyleChange )
    {
        m_graphicProvider->updateFont();
        showGlyph( 0 );
    }

    Inherited::changeEvent( event );
}

#include "GlyphBrowser.moc"
#include "moc_GlyphBrowser.cpp"
