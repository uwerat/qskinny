/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class QskPlotCurveSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole { Polygon };

    Q_INVOKABLE QskPlotCurveSkinlet( QskSkin* = nullptr );
    ~QskPlotCurveSkinlet() override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updatePolygonNode( const QskSkinnable*, QSGNode* ) const;
};
