#include "Diagram.h"

#include <QPainter>

static constexpr int segments = 7;

Diagram::Diagram( QQuickItem* parent )
    : QskControl( parent )
    , m_content( new DiagramContent( this ) )
{
    setAutoLayoutChildren( true );
}

void Diagram::updateLayout()
{
    m_content->setContentsSize( size().toSize() );
    m_content->update();
}

DiagramContent::DiagramContent( QQuickItem* parent ) : QQuickPaintedItem( parent )
{
}

void DiagramContent::paint( QPainter* painter )
{
//    auto size = contentsSize();
//    QRectF outerRect( {0, 0}, size );

    painter->setRenderHint( QPainter::Antialiasing, true );

    qreal stepSize = width() / segments;

    QLinearGradient myGradient( {width() / 2, 0}, {width() / 2, height()} );
    myGradient.setColorAt( 0, "#996776FF" );
    myGradient.setColorAt( 1, "#116776FF" );
    QPen myPen( Qt::transparent );

    QPainterPath myPath;
    myPath.moveTo( 0, height() );

    qreal ys[7] = {0.8, 0.85, 0.92, 0.5, 0.88, 0.7, 0.8};

    for( int i = 0; i < stepSize; i++ )
    {
        qreal x1 = i * stepSize + stepSize / 2;
        qreal y = ys[i] * height();
        qreal x2 = x1;
        myPath.cubicTo( x1, y, x2, height() * ( 1 - ys[i] ), stepSize * ( i + 1 ), height() * ( 1 - ys[i] ) );
    }

    painter->setBrush( myGradient );
    painter->setPen( myPen );
    painter->drawPath( myPath );
}
