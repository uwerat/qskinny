#include "Box.h"
#include "DaytimeSkin.h"

#include "src/shadowedrectangle.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

ShadowBox::ShadowBox( QskControl* control, QQuickItem* parent )
    : QskControl( parent )
    , m_control( control )
    , m_rectangle( new ShadowedRectangle( this ) )
{
    setAutoLayoutChildren( true );

    m_rectangle->shadow()->setColor( Qt::black );
    m_rectangle->shadow()->setSize( 15 );
    m_rectangle->setColor( Qt::white ); // ### opacity should only be for the shadow, not the background
    m_rectangle->setOpacity( 0.1 );
    m_rectangle->setSize( m_control->preferredSize() );
    m_rectangle->corners()->setTopLeft( 6 );
    m_rectangle->corners()->setTopRight( 6 );
    m_rectangle->corners()->setBottomLeft( 6 );
    m_rectangle->corners()->setBottomRight( 6 );
}

void ShadowBox::updateLayout()
{
    const auto s = m_rectangle->shadow()->size();
    qDebug() << m_control->geometry() << geometry();
    m_control->setPosition( {15, 0} ); // ### why?
    m_control->setZ( 1 );
}

Box::Box( const QString& title, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_title( title )
    , m_label( new QskTextLabel( m_title, this ) )
{
    setAutoAddChildren( true );
    setAutoLayoutChildren( true );
    setBackgroundColor( Qt::white );

    setMarginsHint( Panel | QskAspect::Padding, 15 );
    setPanel( true );
    setBoxShapeHint( QskBox::Panel, 6 );
    setBoxBorderMetricsHint( QskBox::Panel, 2 );
    QskBoxBorderColors borderColors( "#dddddd" );
    borderColors.setAlpha( 100 );
    setBoxBorderColorsHint( QskBox::Panel, borderColors );


    auto* r = new ShadowedRectangle( this );
    r->shadow()->setColor( Qt::black );
    r->shadow()->setSize( 15 );
    r->setPosition( {0, 0} );
    r->setOpacity( 0.1 );
    r->setSize( {100, 50} );
    r->corners()->setTopLeft( 6 );
    r->corners()->setTopRight( 6 );
    r->corners()->setBottomLeft( 6 );
    r->corners()->setBottomRight( 6 );
//    r->setRadius( 5 );
    auto* t = new QskTextLabel( "bla", r );


    m_label->setFontRole( DaytimeSkin::TitleFont );

    if( m_label->text().isEmpty() )
    {
        m_label->setVisible( false );
    }
}
