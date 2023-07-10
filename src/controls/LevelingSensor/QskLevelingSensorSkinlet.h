#pragma once

#include <QskSkinlet.h>
#include <QSGNode>

class LevelingSensor;

class LevelingSensorSkinlet : public QskSkinlet
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

    Q_INVOKABLE LevelingSensorSkinlet(QskSkin* skin = nullptr);
    ~LevelingSensorSkinlet() override = default;

    /// @returns Returns the inner radius of the @p skinnable
    static Q_REQUIRED_RESULT float radius2(const QskSkinnable* const skinnable);
    /// @returns Returns the outer radius of the @p skinnable
    static Q_REQUIRED_RESULT float radius1(const QskSkinnable* const skinnable);
    /// @returns Returns the center point of the control
    static Q_REQUIRED_RESULT QPointF center(const QskSkinnable* const skinnable);

protected:

    Q_REQUIRED_RESULT QRectF subControlRect(const QskSkinnable* skinnable,
        const QRectF& contentsRect, QskAspect::Subcontrol subControl) const override;

    Q_REQUIRED_RESULT QSGNode* updateSubNode(const QskSkinnable* skinnable,
        quint8 nodeRole, QSGNode* node) const override;

    template<NodeRole>
    Q_REQUIRED_RESULT QRectF subControlRect(const LevelingSensor* sensor,
        const QRectF& contentsRect) const;

    template<NodeRole>
    Q_REQUIRED_RESULT QSGNode* updateSubNode(const LevelingSensor* sensor,
        quint8 nodeRole, QSGNode* node) const;
};
