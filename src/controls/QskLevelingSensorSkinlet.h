#pragma once

#include <QskSkinlet.h>
#include <QSGNode>

class QskLevelingSensor;

class QSK_EXPORT QskLevelingSensorSkinlet : public QskSkinlet
{
    Q_GADGET

        using Inherited = QskSkinlet;

public:
    enum NodeRole
    {
        OuterDisk,
        Horizon,
        HorizonClip,
        TickmarksX,
        TickmarksXLabels,
        TickmarksY,
        TickmarksYLabels,
        TickmarksZ,
        TickmarksZLabels,
        TriangleBar,
        TickmarksYIndicator,
        RoleCount
    };

    Q_INVOKABLE QskLevelingSensorSkinlet(QskSkin* skin = nullptr);
    ~QskLevelingSensorSkinlet() override = default;

    static Q_REQUIRED_RESULT float outerRadius(const QskSkinnable* const skinnable);
    static Q_REQUIRED_RESULT float innerRadius(const QskSkinnable* const skinnable);
    static Q_REQUIRED_RESULT QPointF center(const QskSkinnable* const skinnable);

protected:

    Q_REQUIRED_RESULT QRectF subControlRect(const QskSkinnable* skinnable,
        const QRectF& contentsRect, QskAspect::Subcontrol subControl) const override;

    Q_REQUIRED_RESULT QSGNode* updateSubNode(const QskSkinnable* skinnable,
        quint8 nodeRole, QSGNode* node) const override;

    template<NodeRole>
    Q_REQUIRED_RESULT QRectF subControlRect(const QskLevelingSensor* sensor,
        const QRectF& contentsRect) const;

    template<NodeRole>
    Q_REQUIRED_RESULT QSGNode* updateSubNode(const QskLevelingSensor* sensor,
        quint8 nodeRole, QSGNode* node) const;
};
