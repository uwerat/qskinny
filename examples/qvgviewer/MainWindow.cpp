/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "MainWindow.h"

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

#include <QPainter>
#include <QVariant>
#include <QRawFont>
#include <QPainterPath>
#include <QFontDatabase>
#include <QFile>

#if 0
static QRawFont symbolFont()
{
    const auto name = "FluentSystemIcons-Resizable";
    //const auto symbolName = "Material Symbols Outlined";
    //const auto symbolName = "Font Awesome 6 Free";

    return QRawFont::fromFont( QFont( name ) );
}
#else
static QRawFont symbolFont()
{
    const auto name = ":/fonts/FluentSystemIcons.ttf";

    QRawFont font;

    QFile file( name );
    if ( file.open( QIODevice::ReadOnly ) )
        font.loadFromData( file.readAll(), 16, QFont::PreferDefaultHinting );
    else
        qWarning() << "Can't load symbol fonts from:" << file.fileName();

    return font;
}
#endif

static QskGraphic icon( QChar symbol )
{
    QskGraphic graphic;
    
#if 0
    QPainter painter( &graphic );
    painter.setFont( QFont( symbolName() ) );
    painter.setPen( Qt::black );
    painter.drawText( 0, 0, symbol );
#else
    const auto rawFont = symbolFont();

    quint32 glyphIndex;
    int numGlyphs = 1;

    const bool success = rawFont.glyphIndexesForChars( &symbol, 1, &glyphIndex, &numGlyphs );
    Q_ASSERT( success );

    if ( success )
    {
        auto path = rawFont.pathForGlyph( glyphIndex );
        if ( !path.isEmpty() )
        {
            path.setFillRule(Qt::WindingFill);

            QPainter painter( &graphic );
            painter.setRenderHint( QPainter::Antialiasing, true );
            painter.fillPath( path, Qt::black );
        }
    }
#endif

    return graphic;
}

class SymbolBox : public QskSpinBox
{
    Q_OBJECT

  public:
    SymbolBox( QQuickItem* parent = nullptr )
        : QskSpinBox( 0xe700, 0xf800, 1.0, parent )
    {
        setDecimals( 0 );
        setValue( 0xf1c4 );

        connect( this, &QskSpinBox::valueChanged,
            this, [this]() { Q_EMIT symbolChanged( symbol() ); } );
    }

    QChar symbol() const { return static_cast< char16_t  >( value() ); }
    void setSymbol( QChar symbol ) { setValue( symbol.unicode() ); }

  Q_SIGNALS:
    void symbolChanged( QChar );
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

    void showSymbol( QChar symbol )
    {
        setGraphic( icon( symbol ) );
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

MainWindow::MainWindow()
{
    auto label = new GraphicLabel();

    auto spinBox = new SymbolBox();
    
    auto invertButton = new QskPushButton( "Inverted" );
    invertButton->setSizePolicy( Qt::Horizontal, QskSizePolicy::Fixed );
    invertButton->setCheckable( true );
    invertButton->setLayoutAlignmentHint( Qt::AlignRight );

    auto hBox = new QskLinearBox( Qt::Horizontal );
    hBox->addItem( spinBox );
    hBox->addItem( invertButton );

    auto box = new QskLinearBox( Qt::Vertical );
    box->setPanel( true );
    box->setPadding( 5 );
    box->addItem( hBox );
    box->addItem( label );

    addItem( box );

    connect( spinBox, &SymbolBox::symbolChanged,
        label, &GraphicLabel::showSymbol );

    connect( invertButton, &QskPushButton::toggled,
        label, &GraphicLabel::setDarknessMode );

    connect( qskSkinManager, &QskSkinManager::skinChanged,
        this, &MainWindow::setGraphicRoles );

    setGraphicRoles( qskSkinManager->skin() );
    label->showSymbol( spinBox->symbol() );

#if 0
    for ( auto name : QFontDatabase::families() )
        qDebug() << name;
#endif
}

void MainWindow::setGraphicRoles( QskSkin* skin )
{
    // substituting black
    QskColorFilter colorFilter;
    colorFilter.addColorSubstitution( QskRgb::Black, QskRgb::DarkKhaki );

    skin->setGraphicFilter( GraphicLabel::Inverted, colorFilter );
}

#include "MainWindow.moc"
#include "moc_MainWindow.cpp"
