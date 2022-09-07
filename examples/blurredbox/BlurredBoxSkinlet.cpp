#include "BlurredBoxSkinlet.h"
#include "BlurredBox.h"
#include "BlurredBoxNode.h"

BlurredBoxSkinlet::BlurredBoxSkinlet()
{
    setNodeRoles( { PanelRole } );
}

QRectF BlurredBoxSkinlet::subControlRect(
    const QskSkinnable*, const QRectF& contentsRect, QskAspect::Subcontrol ) const
{
    return contentsRect;
}

QSGNode* BlurredBoxSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto box = dynamic_cast< const BlurredBox* >( skinnable );
    const auto r = box->subControlRect( BlurredBox::Panel );

    if ( r.isEmpty() )
    {
        return nullptr;
    }

    const auto result = QskSkinlet::updateSubNode( skinnable, nodeRole, node );

    switch ( nodeRole )
    {
        case PanelRole:
            auto blurred = QskSGNode::ensureNode< BlurredBoxNode >( node );
            auto rectOfScreen = box->rectOfScreen();
            auto rectOnScreen = box->rectOnScreen();
            blurred->setBlurData( r, box->shape(), rectOfScreen, rectOnScreen,
                static_cast< float >( box->opacity() ), box->blurDirections(), box->blurQuality(),
                box->blurSize() );
            return blurred;
    }

    return result;
}
