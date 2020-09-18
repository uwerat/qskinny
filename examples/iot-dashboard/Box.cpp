#include "Box.h"
#include "DaytimeSkin.h"

#include "src/shadowedrectangle.h"

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


    auto* r = new ShadowedRectangle( this );
    r->setColor( Qt::green );


    m_label->setFontRole( DaytimeSkin::TitleFont );

    if( m_label->text().isEmpty() )
    {
        m_label->setVisible( false );
    }
}
