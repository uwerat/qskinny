/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "QskPlotNamespace.h"

#include <QskSkinnable.h>
#include <qobject.h>

class QskPlotView;
class QskIntervalF;

class QskPlotItem : public QObject, public QskSkinnable
{
    Q_OBJECT

    Q_PROPERTY( QskPlot::Axis xAxis READ xAxis WRITE setXAxis NOTIFY axisChanged )
    Q_PROPERTY( QskPlot::Axis yAxis READ yAxis WRITE setYAxis NOTIFY axisChanged )
    Q_PROPERTY( qreal z READ z WRITE setZ NOTIFY zChanged )

  public:
    /*
        The item can decide if it wants to use plot ( scales ) coordinates or
        canvas ( QQuickItem ) coordinates for the vertexes of its scenegraph nodes.

        PlotItems, that represent some sort of data ( f.e curves ) usually
        prefer to use plot coordinates, while decorations ( f.e legend ) are
        often aligned to the plot canvas geometry.

        Items in plot coordinates often do not need to be updated when
        the geometry of the plot canvas or the scales have been changed.
        To opt out from these updates the plot item needs to overload
        the transformationChange() hook.
     */

    enum CoordinateType
    {
        CanvasCoordinates,
        PlotCoordinates
    };

    Q_ENUM( CoordinateType );

    enum ChangeFlag
    {
        XBoundariesChanged    = 1 << 0,
        XTickmarksChanged     = 1 << 1,

        YBoundariesChanged    = 1 << 1,
        YTickmarksChanged     = 1 << 2,

        CanvasGeometryChanged = 1 << 2
    };
    Q_ENUM( ChangeFlag );

    Q_DECLARE_FLAGS( ChangeFlags, ChangeFlag );

    QskPlotItem( QObject* = nullptr );
    ~QskPlotItem() override;

    void attach( QskPlotView* );
    void detach();

    void setXAxis( QskPlot::Axis );
    QskPlot::Axis xAxis() const;

    void setYAxis( QskPlot::Axis );
    QskPlot::Axis yAxis() const;

    void setAxes( QskPlot::Axis xAxis, QskPlot::Axis yAxis );

    void setZ( qreal );
    qreal z() const;

    void setCoordinateType( CoordinateType );
    CoordinateType coordinateType() const;

    /*
        Indicates if the item depends on clipping

        Batching of node updates is one of the main performance features
        of the scene graph - however clipping breaks batching.
        A plot that has no plot item, that needs clipping, can decide
        to skip inserting a clip node.
     */
    virtual bool needsClipping() const;

    QTransform transformation() const;
    QskIntervalF boundaries( Qt::Orientation ) const;

    QRectF scaleRect() const;

    const QskPlotView* view() const;

    void markDirty();
    void resetDirty();
    bool isDirty() const;

    virtual void updateNode( QSGNode* );
    virtual void transformationChanged( ChangeFlags );

  Q_SIGNALS:
    void axisChanged();
    void zChanged( qreal );

  private:
    QQuickItem* owningItem() const override final;
    void updatePlot();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskPlotItem::ChangeFlags )

inline void QskPlotItem::detach()
{
    attach( nullptr );
}
