/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include <SkinnyNamespace.h>

#include <QskFunctions.h>
#include <QskLinearBox.h>
#include <QskPushButton.h>
#include <QskQuick.h>
#include <QskWindow.h>
#include <QskGraphicLabel.h>
#include <QskGraphic.h>
#include <QskEvent.h>

#include <QGuiApplication>
#include <QDebug>

#include <SkinnyShortcut.h>
#include <cmath>

#include <private/qquickshadereffect_p.h>
#include <private/qquickshadereffectsource_p.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    #include <QFile>
#else
    #include <QUrl>
#endif

class ButtonBox : public QskLinearBox
{
  public:
    ButtonBox( QQuickItem* parent = nullptr )
        : QskLinearBox( parent )
    {
        ( void ) new QskPushButton( "Button 1", this );
        ( void ) new QskPushButton( "Button 2", this );
        ( void ) new QskPushButton( "Button 3", this );
    }
};

class ShaderEffect : public QQuickShaderEffect
{
    using Inherited = QQuickShaderEffect;

  public:
    ShaderEffect( QQuickItem* parent = nullptr )
        : QQuickShaderEffect( parent )
    {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        setVertexShader( loadShader( ":/shaders/blur.vert" ) );
        setFragmentShader( loadShader( ":/shaders/blur.frag" ) );
#else
        setVertexShader( QUrl( "qrc:/shaders/blur.vert.qsb" ) );
        setFragmentShader( QUrl( "qrc:/shaders/blur.frag.qsb" ) );
#endif

        m_source = new QQuickShaderEffectSource( this );
        m_source->setLive( true );

        setSource( m_source );
    }

    void setSourceItem( QQuickItem* item ) { m_source->setSourceItem( item ); }
    QQuickItem* sourceItem() const { return m_source->sourceItem(); }

  protected:
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    void geometryChanged( const QRectF& newRect, const QRectF& oldRect ) override
    {
        Inherited::geometryChanged( newRect, oldRect );
        updateSourceRect( newRect );
    }
#else
    void geometryChange( const QRectF& newRect, const QRectF& oldRect ) override
    {
        Inherited::geometryChange( newRect, oldRect );
        updateSourceRect( newRect );
    }
#endif

  private:
    void updateSourceRect( const QRectF& rect )
    {
        const auto sourceRect = m_source->sourceItem()->mapRectFromItem( this, rect );
        m_source->setSourceRect( sourceRect );
    }

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QByteArray loadShader( const char* path ) const
    {
        QFile f( path );
        f.open( QFile::ReadOnly );

        return f.readAll();
    }
#endif

    void setSource( QQuickShaderEffectSource* source )
    {
        setProperty( "source", QVariant::fromValue( source ) );
    }

    QQuickShaderEffectSource* m_source;
};

class BlurredBox : public QskControl
{
    using Inherited = QskControl;

  public:
    BlurredBox( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setFlag( QQuickItem::ItemHasContents, false );
        setAutoLayoutChildren( true );

        m_effect = new ShaderEffect( this );
    }

    void setSourceItem( QQuickItem* item )
    {
        m_effect->setSourceItem( item );
    }

  private:
    ShaderEffect* m_effect;
};

class BackgroundItem : public QskControl
{
  public:
    BackgroundItem( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        m_label = new QskGraphicLabel( this );
        m_label->setFillMode( QskGraphicLabel::Stretch );

        const QImage image( ":/images/parrots.jpg" );
        m_label->setGraphic( QskGraphic::fromImage( image ) );

        startTimer( 20 );
    }

    void timerEvent( QTimerEvent* ) override
    {
        static int counter = 0;

        const auto angle = counter++ / 50.0 * M_PI * 2.0;

        const auto x = std::cos( angle );
        const auto y = std::sin( angle );

        const qreal margin = 20;

        auto labelRect = rect();
        labelRect.adjust( margin, margin, -margin, -margin );
        labelRect.translate( margin * x, margin * y );

        m_label->setGeometry( labelRect );
    }

  private:
    QskGraphicLabel* m_label;
};

class MainView : public QskControl
{
  public:
    MainView( QQuickItem* parent = nullptr )
        : QskControl( parent )
    {
        setPolishOnResize( true );

        m_background = new BackgroundItem( this );

        m_blurredBox = new BlurredBox( this );
        m_blurredBox->setSourceItem( m_background );

        (void )new ButtonBox( m_blurredBox );
    }

  protected:
    void updateLayout()
    {
        m_background->setGeometry( rect() );

        QRectF blurredRect( QPointF(), 0.7 * size() );
        blurredRect.moveCenter( rect().center() );

        m_blurredBox->setGeometry( blurredRect );
    }

  private:
    BackgroundItem* m_background;
    BlurredBox* m_blurredBox;
};

int main( int argc, char** argv )
{
    QGuiApplication app( argc, argv );

    SkinnyShortcut::enable( SkinnyShortcut::AllShortcuts );

    QskWindow window;
    window.setColor( Qt::darkGray );
    window.addItem( new MainView( window.contentItem() ) );
    window.resize( 800, 600 );
    window.show();

    return app.exec();
}
