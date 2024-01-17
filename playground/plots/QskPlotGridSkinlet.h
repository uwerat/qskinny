/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class QskPlotGridSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole { MinorGrid, MajorGrid };

    Q_INVOKABLE QskPlotGridSkinlet( QskSkin* = nullptr );
    ~QskPlotGridSkinlet() override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateGridNode(
        const QskSkinnable*, QskAspect::Subcontrol, QSGNode* ) const;
};
