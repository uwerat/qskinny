#include "QskShadowedRectangleSkinlet.h"
#include "QskShadowedRectangle.h"

#include "src/scenegraph/shadowedrectanglenode.h"

QskShadowedRectangleSkinlet::QskShadowedRectangleSkinlet( QskSkin* skin ) : QskSkinlet( skin )
    , m_border( new BorderGroup )
    , m_shadow( new ShadowGroup )
    , m_corners( new CornersGroup )
{
    setNodeRoles( { ShadowRole } );
}

QRectF QskShadowedRectangleSkinlet::subControlRect( const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* QskShadowedRectangleSkinlet::updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto rectangle = static_cast< const QskShadowedRectangle* >( skinnable );

    switch( nodeRole )
    {
        case ShadowRole:
            return updateShadowNode( rectangle, node );

        default:
            return nullptr;
    }
}

QSGNode* QskShadowedRectangleSkinlet::updateShadowNode( const QskShadowedRectangle* rectangle, QSGNode* node ) const
{
    auto shadowNode = static_cast<ShadowedRectangleNode*>( node );

    if( !shadowNode )
    {
        shadowNode = new ShadowedRectangleNode{};
    }

//    shadowNode->setBorderEnabled( m_border->isEnabled() );
//    shadowNode->setRect( rectangle->contentsRect() );
//    shadowNode->setSize( m_shadow->size() );
//    shadowNode->setRadius( m_corners->toVector4D( m_radius ) );
//    shadowNode->setOffset( QVector2D{float( m_shadow->xOffset() ), float( m_shadow->yOffset() )} );
//    shadowNode->setColor( m_color );
//    shadowNode->setShadowColor( m_shadow->color() );
//    shadowNode->setBorderWidth( m_border->width() );
//    shadowNode->setBorderColor( m_border->color() );
//    shadowNode->updateGeometry();

    shadowNode->setBorderEnabled( true );
    shadowNode->setRect( {0, 0, 100, 50} );
    shadowNode->setSize( 50 );
    shadowNode->setRadius( m_corners->toVector4D( 5 ) );
    shadowNode->setOffset( QVector2D{float( 1 ), float( 1 )} );
    shadowNode->setColor( Qt::green );
    shadowNode->setShadowColor( Qt::magenta );
    shadowNode->setBorderWidth( 2 );
    shadowNode->setBorderColor( Qt::red );
    shadowNode->updateGeometry();
    return shadowNode;
}
