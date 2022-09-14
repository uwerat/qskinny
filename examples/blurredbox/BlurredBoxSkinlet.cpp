#include "BlurredBoxSkinlet.h"
#include "BlurredBox.h"
#include "BlurredBoxNode.h"
#include "BlurredBoxTextureProvider.h"

#include <QskSGNode.h>
#include <QskBoxShapeMetrics.h>

#include <utility>

BlurredBoxSkinlet::BlurredBoxSkinlet(std::shared_ptr<BlurredBoxTextureProvider> textureProvider) : m_textureProvider(std::move(textureProvider))
{
    setNodeRoles( { PanelRole } );
}

BlurredBoxSkinlet::~BlurredBoxSkinlet()
{
    m_textureProvider = nullptr;
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
            auto* const blurred = QskSGNode::ensureNode< BlurredBoxNode >( node );
            const auto boxShapeHint = box->boxShapeHint(BlurredBox::Panel);
            const auto rectOnScreen = box->mapRectToScene( box->contentsRect() );
            blurred->setBlurData( r, boxShapeHint, rectOnScreen,
                static_cast< float >( box->opacity() ), box->blurDirections(), box->blurQuality(),
                box->blurSize(), m_textureProvider.get());
            return blurred;
    }

    return result;
}
