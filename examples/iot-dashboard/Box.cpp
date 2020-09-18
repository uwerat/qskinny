#include "Box.h"
#include "DaytimeSkin.h"

#include "QskShadowedRectangle.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

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


    auto* r = new QskShadowedRectangle( this );
//    r->setPosition( {50, 50} );
//    r->setColor( Qt::green );
//    r->setOpacity( 0.5 );
//    r->setSize( {100, 50} );
//    r->setVisible( true );
//    r->corners()->setTopLeft( 5 );
//    r->corners()->setTopRight( 5 );
//    r->corners()->setBottomLeft( 5 );
//    r->corners()->setBottomRight( 5 );
//    r->setRadius( 5 );
    auto* t = new QskTextLabel( "bla", r );


    m_label->setFontRole( DaytimeSkin::TitleFont );

    if( m_label->text().isEmpty() )
    {
        m_label->setVisible( false );
    }
}
