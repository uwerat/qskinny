#pragma once

#include <QskSkinlet.h>

class RadialTickmarksSkinlet : public QskSkinlet
{
    Q_GADGET
public:
    enum NodeRole
    {
        Lines,
        RoleCount
    };

    Q_INVOKABLE RadialTickmarksSkinlet( QskSkin* skin = nullptr );

protected:
    Q_REQUIRED_RESULT QRectF subControlRect( const QskSkinnable* skinnable, const QRectF& contentsRect, QskAspect::Subcontrol subControl) const override;
    Q_REQUIRED_RESULT QSGNode* updateSubNode( const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node) const override;
};