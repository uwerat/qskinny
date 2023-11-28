/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "QskPlotItem.h"
#include <qnamespace.h>

class PlotCursor : public QskPlotItem
{
    Q_OBJECT

    using Inherited = QskPlotItem;

  public:
    QSK_SUBCONTROLS( Line, LabelPanel, LabelText )

    PlotCursor( QObject* = nullptr );
    ~PlotCursor() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setCanvasPosition( qreal );

    void setPosition( qreal );
    qreal position() const;

    void transformationChanged( ChangeFlags ) override;
    bool needsClipping() const override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
