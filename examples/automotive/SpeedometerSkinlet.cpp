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
} // namespace

SpeedometerSkinlet::SpeedometerSkinlet( QskSkin* skin ) :
    QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, LabelsRole, NeedleRole } );
}

SpeedometerSkinlet::~SpeedometerSkinlet() = default;

QRectF SpeedometerSkinlet::subControlRect( const QskSkinnable* skinnable,
                                           QskAspect::Subcontrol /*unused*/ ) const
{
    const auto speedometer = static_cast< const Speedometer* >( skinnable );

    // ### differentiate for subcontrols
    return speedometer->contentsRect();
}

QSGNode* SpeedometerSkinlet::updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole,
                                            QSGNode* node ) const
{
    const auto speedometer = static_cast< const Speedometer* >( skinnable );

    switch( nodeRole )
    {
        case PanelRole:
            return updatePanelNode( speedometer, node );

        case LabelsRole:
            return updateLabelsNode( speedometer, node );

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
        boxNode = new QskBoxNode;
    }

    QRectF panelRect = subControlRect( speedometer, Speedometer::Panel );
    qreal radius = panelRect.width() / 2;
    QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
    QskBoxBorderMetrics borderMetrics( 2 );
    QskBoxBorderColors borderColors( Qt::white );
    QskGradient gradient( Qt::black );
    boxNode->setBoxData( panelRect, shapeMetrics, borderMetrics, borderColors, gradient );

    return boxNode;
}

QSGNode* SpeedometerSkinlet::updateLabelsNode( const Speedometer* speedometer, QSGNode* node ) const
{
    const int labelsCount = speedometer->labels().count();

    // ### actually, we could draw labels with only one entry
    if ( labelsCount <= 1 )
    {
        return nullptr;
    }

    auto ticksNode = static_cast< TicksNode* >( node );

    if ( ticksNode == nullptr )
    {
        ticksNode = new TicksNode( Qt::white );
    }

    const auto startAngle = speedometer->minimum();
    const auto endAngle = speedometer->maximum();
    const auto step = ( endAngle - startAngle ) / ( labelsCount - 1 );

    auto geometry = ticksNode->geometry();
    geometry->allocate( labelsCount * 2 );

    auto vertexData = geometry->vertexDataAsPoint2D();
    memset( vertexData, 0, static_cast< size_t >( geometry->vertexCount() ) );

    QMarginsF panelMargins = speedometer->marginsHint( Speedometer::Panel | QskAspect::Margin );
    const QRectF panelRect = subControlRect( speedometer,
        Speedometer::Panel ).marginsRemoved( panelMargins );

    QPointF center = QPointF( panelRect.x() + panelRect.width() / 2,
                              panelRect.y() + panelRect.height() / 2 );
    auto radius = static_cast< float >( panelRect.width() / 2 );

    const QMarginsF numbersMargins = speedometer->marginsHint( Speedometer::Labels | QskAspect::Margin );
    QFontMetrics fontMetrics( speedometer->effectiveFont( Speedometer::Labels ) );

    auto angle = startAngle;

    // Create a series of tickmarks from minimum to maximum
    for( int i = 0; i < labelsCount; ++i, angle += step )
    {
        qreal cosine = qCos( qDegreesToRadians( angle ) );
        qreal sine = qSin( qDegreesToRadians( angle ) );

        auto xStart = center.x() + radius * cosine;
        auto yStart = center.y() + radius * sine;

        // ### skin hint for each of highlighted / normal marks
        qreal length = 15;
        auto xEnd = center.x() + ( radius - length ) * cosine;
        auto yEnd = center.y() + ( radius - length ) * sine;

        vertexData[0].set( xStart, yStart );
        vertexData[1].set( xEnd, yEnd );

        vertexData += 2;

        QVector< QString > labels = speedometer->labels();

        // only create a text node if there is a label for it:
        if ( labels.count() > i )
        {
            const QString& text = labels.at( i );

            auto w = fontMetrics.width( text );
            auto h = fontMetrics.height();
            auto adjustX = ( -0.5 * cosine - 0.5 ) * w;
            auto adjustY = ( -0.5 * sine - 0.5 ) * h;

            auto numbersX = xEnd + ( -1 * numbersMargins.left() * cosine ) + adjustX;
            auto numbersY = yEnd + ( -1 * numbersMargins.top() * sine ) + adjustY;

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
    }

    geometry->setLineWidth( 2 );
    geometry->markVertexDataDirty();

    ticksNode->markDirty( QSGNode::DirtyGeometry );

    return ticksNode;
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
    }

    QRectF centerNodeRect( center.x() - radius, center.y() - radius,
        2 * radius, 2 * radius );
    QskBoxShapeMetrics shapeMetrics( radius, radius, radius, radius );
    QskBoxBorderMetrics borderMetrics( 2 );
    QskBoxBorderColors borderColors( Qt::red );
    QskGradient gradient( Qt::red );
    boxNode->setBoxData( centerNodeRect, shapeMetrics, borderMetrics, borderColors, gradient );

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

    float angle = speedometer->value();
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
