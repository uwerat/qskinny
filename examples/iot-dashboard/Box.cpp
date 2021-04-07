#include "Box.h"
#include "Skin.h"

#include "src/shadowedrectangle.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextLabel.h>

QSK_SUBCONTROL( Box, Panel )

Box::Box( const QString& title, QQuickItem* parent )
    : QskLinearBox( Qt::Vertical, parent )
    , m_title( title )
    , m_label( new QskTextLabel( m_title, this ) )
{
    setAutoAddChildren( true );
    setAutoLayoutChildren( true );
    setBackgroundColor( Qt::white );

    setPaddingHint( Panel, 15 );
    setPanel( true );
    setBoxShapeHint( QskBox::Panel, 6 );
    QskBoxBorderColors borderColors( "#dddddd" );
    borderColors.setAlpha( 100 );
    setBoxBorderColorsHint( QskBox::Panel, borderColors );

    m_label->setFontRole( Skin::TitleFont );

    if( m_label->text().isEmpty() )
    {
        m_label->setVisible( false );
    }
}

QskAspect::Subcontrol Box::effectiveSubcontrol( QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
    {
        return Box::Panel;
    }

    return subControl;
}
