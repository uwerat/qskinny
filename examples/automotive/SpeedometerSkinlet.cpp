#include "SpeedometerSkinlet.h"
#include "Speedometer.h"

#include <QskBoxBorderColors.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxNode.h>
#include <QskBoxShapeMetrics.h>
#include <QskTextColors.h>
#include <QskTextNode.h>
#include <QskTextOptions.h> // ### remove

#include <QFontMetrics>
#include <QSGFlatColorMaterial>
#include <QtMath>

namespace
{

    class TicksNode : public QSGGeometryNode
    {
        public:
            TicksNode( const QColor& color ):
                m_geometry( QSGGeometry::defaultAttributes_Point2D(), 0 )
            {
                m_geometry.setDrawingMode( GL_LINES );
                m_geometry.setVertexDataPattern( QSGGeometry::StaticPattern );

                m_material.setColor( color );

                setGeometry( &m_geometry );
                setMaterial( &m_material );
            }

        private:
            QSGFlatColorMaterial m_material;
            QSGGeometry m_geometry;
    };
}

SpeedometerSkinlet::SpeedometerSkinlet( QskSkin* skin ) :
    QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, TicksRole, NumbersRole, NeedleRole } );
}

SpeedometerSkinlet::~SpeedometerSkinlet()
{
}

QRectF SpeedometerSkinlet::subControlRect( const QskSkinnable* skinnable,
                                           QskAspect::Subcontrol ) const
{
    const auto speedometer = static_cast< const Speedometer* >( skinnable );

    // ### differentiate for subcontrols
    return speedometer->contentsRect();
}

QSGNode* SpeedometerSkinlet::updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole,
                                            QSGNode* node ) const
{
    const Speedometer* speedometer = static_cast< const Speedometer* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( speedometer, node );

        case TicksRole:
            return updateTicksNode( speedometer, node );

        case NumbersRole:
            return updateNumbersNode( speedometer, node );

        case NeedleRole:
            return updateNeedleNode( speedometer, node );

        default:
            return nullptr;
    }
}

QSGNode* SpeedometerSkinlet::updatePanelNode( const Speedometer* speedometer, QSGNode* node ) const
{
    auto boxNode = static_cast< QskBoxNode* >( node );

    if( boxNode == nullptr )
    {
        QRectF panelRect = subControlRect( speedometer, Speedometer::Panel );
        boxNode = new QskBoxNode;
        qreal radius = panelRect.width() / 2;
        QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
        QskBoxBorderMetrics borderMetrics( 2 );
        QskBoxBorderColors borderColors( Qt::white );
        QskGradient gradient( Qt::black );
        boxNode->setBoxData( panelRect, shapeMetrics, borderMetrics, borderColors, gradient );
    }

    return boxNode;
}

QSGNode* SpeedometerSkinlet::updateTicksNode( const Speedometer* speedometer, QSGNode* node ) const
{
    auto ticksNode = static_cast< TicksNode* >( node );

    if( ticksNode == nullptr )
    {
        ticksNode = new TicksNode( Qt::white );
    }

    // ### add API for this:
    // ### make qfloat etc.?
    float startAngle = -215;
    float endAngle = 35; // ### angle is still wrong somehow
    const int tickCount = 18;
    int highlightedMarksStep = 3;

    auto geometry = ticksNode->geometry();
    geometry->allocate( tickCount * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, static_cast< size_t >( geometry->vertexCount() ) );

    auto stepStride = ( endAngle - startAngle ) / ( tickCount - 1 );

    QMarginsF panelMargins = speedometer->marginsHint( Speedometer::Panel | QskAspect::Margin );
    const QRectF panelRect = subControlRect( speedometer,
        Speedometer::Panel ).marginsRemoved( panelMargins );

    QPointF center = QPointF( panelRect.x() + panelRect.width() / 2,
                              panelRect.y() + panelRect.height() / 2 );
    auto radius = static_cast< float >( panelRect.width() / 2 );

    const QMarginsF numbersMargins = speedometer->marginsHint( Speedometer::Numbers | QskAspect::Margin );
    QFontMetrics fontMetrics( speedometer->effectiveFont( Speedometer::Numbers ) );

    float angle = startAngle;

    // Create a series of tickmarks from minimum to maximum
    for( int i = 0; i < tickCount; ++i, angle += stepStride )
    {
        qreal cosine = qCos( qDegreesToRadians( angle ) );
        qreal sine = qSin( qDegreesToRadians( angle ) );

        float xStart = center.x() + radius * cosine;
        float yStart = center.y() + radius * sine;

        // ### skin hint for each of highlighted / normal marks
        qreal length = ( i % highlightedMarksStep == 0 ) ? 15 : 15;
        float xEnd = center.x() + ( radius - length ) * cosine;
        float yEnd = center.y() + ( radius - length ) * sine;

        vertexData[0].set( xStart, yStart );
        vertexData[1].set( xEnd, yEnd );

        vertexData += 2;

        QString text = QString::number( i * 10 );

        float w = fontMetrics.width( text );
        float h = fontMetrics.height();
        float adjustX = ( -0.5 * cosine - 0.5 ) * w;
        float adjustY = ( -0.5 * sine - 0.5 ) * h;

        float numbersX = xEnd + ( -1 * numbersMargins.left() * cosine ) + adjustX;
        float numbersY = yEnd + ( -1 * numbersMargins.top() * sine ) + adjustY;

        QRectF numbersRect( numbersX, numbersY, w, h );

        QskTextNode* numbersNode;

        if ( ticksNode->childCount() > i )
        {
            numbersNode = static_cast< QskTextNode* >( ticksNode->childAtIndex( i ) );
        }
        else
        {
            numbersNode = new QskTextNode();
        }

        numbersNode->setTextData( speedometer, text, numbersRect, QFont(),
                                  QskTextOptions(), QskTextColors( Qt::white ),
                                  Qt::AlignCenter | Qt::AlignHCenter, Qsk::Normal );

        if ( ticksNode->childCount() <= i )
        {
            ticksNode->appendChildNode( numbersNode );
        }
        // ### remove nodes in case they are superfluous
    }

    geometry->setLineWidth( 2 );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
}

QSGNode* SpeedometerSkinlet::updateNumbersNode( const Speedometer* speedometer, QSGNode* node ) const
{
    return nullptr;
}

QSGNode* SpeedometerSkinlet::updateNeedleNode( const Speedometer* speedometer, QSGNode* node ) const
{
    QMarginsF margins = speedometer->marginsHint( Speedometer::Panel | QskAspect::Margin );
    const QRectF panelRect = subControlRect( speedometer, Speedometer::Panel ).marginsRemoved( margins );
    auto radius = 15; // ### skin hint
    QPointF center = QPointF( panelRect.x() + panelRect.width() / 2,
        panelRect.y() + panelRect.height() / 2 );

    auto boxNode = static_cast< QskBoxNode* >( node );

    if( boxNode == nullptr )
    {
        boxNode = new QskBoxNode;
        QRectF centerNodeRect( center.x() - radius, center.y() - radius,
            2 * radius, 2 * radius );
        QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
        QskBoxBorderMetrics borderMetrics( 2 );
        QskBoxBorderColors borderColors( Qt::red );
        QskGradient gradient( Qt::red );
        boxNode->setBoxData( centerNodeRect, shapeMetrics, borderMetrics, borderColors, gradient );
    }

    TicksNode* needleNode;

    if ( boxNode->childCount() == 0 )
    {
        needleNode = new TicksNode( Qt::red );
    }
    else
    {
        needleNode = static_cast< TicksNode* >( boxNode->childAtIndex( 0 ) );
    }

    auto panelRadius = static_cast< float >( panelRect.width() / 2 );

    auto needleWidth = 2; // ### do differently somehow
    QRectF needleRect( center.x() - needleWidth , center.y() - needleWidth ,
        panelRadius - ( needleWidth  + 10 ), 2 * needleWidth  );
    float xStart = center.x() - needleWidth ;
    float yStart = center.y();

    float angle = 315; // ### API
    qreal cosine = qCos( qDegreesToRadians( angle ) );
    qreal sine = qSin( qDegreesToRadians( angle ) );

    float needleRadius = panelRadius - 10; // 10 == margins ### skinhint
    float xEnd = center.x() + needleRadius * cosine;
    float yEnd = center.y() + needleRadius * sine;

    auto geometry = needleNode->geometry();
    geometry->allocate( 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, static_cast< size_t >( geometry->vertexCount() ) );

    vertexData[0].set( xStart, yStart );
    vertexData[1].set( xEnd, yEnd );

    geometry->setLineWidth( 2 * needleWidth  );
    geometry->markVertexDataDirty();

    needleNode->markDirty( QSGNode::DirtyGeometry );




    if ( boxNode->childCount() == 0 )
    {
        boxNode->appendChildNode( needleNode );
    }

    return boxNode;
}

#include "moc_SpeedometerSkinlet.cpp"
