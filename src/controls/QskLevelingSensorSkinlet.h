/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LEVELING_SENSOR_SKINLET_H
#define QSK_LEVELING_SENSOR_SKINLET_H

#include <QSGNode>
#include <QskSkinlet.h>

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

    Q_INVOKABLE QskLevelingSensorSkinlet( QskSkin* skin = nullptr );
    ~QskLevelingSensorSkinlet() override = default;

    Q_REQUIRED_RESULT static float outerRadius( const QskSkinnable* const skinnable );
    Q_REQUIRED_RESULT static float innerRadius( const QskSkinnable* const skinnable );
    Q_REQUIRED_RESULT static QPointF center( const QskSkinnable* const skinnable );

  protected:
    Q_REQUIRED_RESULT QRectF subControlRect( const QskSkinnable* skinnable,
        const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const override;

    Q_REQUIRED_RESULT QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override;

    template< NodeRole >
    Q_REQUIRED_RESULT QRectF subControlRect(
        const QskLevelingSensor* sensor, const QRectF& contentsRect ) const;

    template< NodeRole >
    Q_REQUIRED_RESULT QSGNode* updateSubNode(
        const QskLevelingSensor* sensor, quint8 nodeRole, QSGNode* node ) const;
};

#endif