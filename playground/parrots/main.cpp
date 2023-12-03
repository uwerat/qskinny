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

static const char* vertexShader = R"(
    uniform highp mat4 qt_Matrix;
    attribute highp vec4 qt_Vertex;
    attribute highp vec2 qt_MultiTexCoord0;
    varying highp vec2 coord;
    void main() {
        coord = qt_MultiTexCoord0;
        gl_Position = qt_Matrix * qt_Vertex;
    })";

static const char* fragmentShader = R"(
    varying highp vec2 coord;
    uniform sampler2D source;
    uniform lowp float qt_Opacity;
    
    void main() {
        vec2 delta = vec2(0.01, 0.01);
        gl_FragColor =(0.0538 * texture2D(source, coord - 3.182 * delta)
                     + 0.3229 * texture2D(source, coord - 1.364 * delta)
                     + 0.2466 * texture2D(source, coord)
                     + 0.3229 * texture2D(source, coord + 1.364 * delta)
                     + 0.0538 * texture2D(source, coord + 3.182 * delta)) * qt_Opacity;
    })";

class ButtonBox: public QskLinearBox
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
        setVertexShader( ::vertexShader );
        setFragmentShader( ::fragmentShader );

        m_source = new QQuickShaderEffectSource( this );
        m_source->setLive( true );

        setSource( m_source );
    }

    void setSourceItem( QQuickItem* item ) { m_source->setSourceItem( item ); }
    QQuickItem* sourceItem() const { return m_source->sourceItem(); }

  protected:
    void geometryChanged( const QRectF& newRect, const QRectF& oldRect ) override
    {
        Inherited::geometryChanged( newRect, oldRect );

        auto sourceRect = newRect;
        sourceRect = m_source->sourceItem()->mapRectFromItem( this, sourceRect );

        m_source->setSourceRect( sourceRect );
    }

  private:
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

        const auto angle = counter++ / 50.0 * M_PI * 2.0 ;

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
