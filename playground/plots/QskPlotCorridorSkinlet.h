/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class QskPlotCorridorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        CorridorRole,
        LowerBoundRole,
        UpperBoundRole
    };

    Q_INVOKABLE QskPlotCorridorSkinlet( QskSkin* = nullptr );
    ~QskPlotCorridorSkinlet() override;

    void updateNode( QskSkinnable*, QSGNode* ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateCorridorNode( const QskSkinnable*, QSGNode* ) const;
    QSGNode* updateBorderNode( const QskSkinnable*, quint8, QSGNode* ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
