#include "Box.h"
#include "DaytimeSkin.h"

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

    setMarginsHint( Panel | QskAspect::Padding, 15 );
    setPanel( true );
    setBoxShapeHint( QskBox::Panel, 6 );
    setBoxBorderMetricsHint( QskBox::Panel, 2 );
    QskBoxBorderColors borderColors( "#dddddd" );
    borderColors.setAlpha( 100 );
    setBoxBorderColorsHint( QskBox::Panel, borderColors );

    m_label->setFontRole( DaytimeSkin::TitleFont );
}
