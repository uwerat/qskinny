#pragma once

#include <QskSkinlet.h>

class RadialNodesSkinlet : public QskSkinlet
{
    Q_GADGET
public:
    enum NodeRole
    {
        Text,
        Foreground,
        Lines,
        RoleCount
    };

    Q_INVOKABLE RadialNodesSkinlet( QskSkin* skin = nullptr );

protected:
    Q_REQUIRED_RESULT QRectF subControlRect( const QskSkinnable* skinnable, const QRectF& contentsRect, QskAspect::Subcontrol subControl) const override;
    Q_REQUIRED_RESULT QSGNode* updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node) const override;
};