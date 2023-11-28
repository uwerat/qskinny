/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class PlotCursorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole { CursorLine, TextBox, Text };

    Q_INVOKABLE PlotCursorSkinlet( QskSkin* = nullptr );
    ~PlotCursorSkinlet() override;

    void updateNode( QskSkinnable*, QSGNode* ) const override;

    int sampleCount( const QskSkinnable*,
        QskAspect::Subcontrol ) const override final;

    QRectF sampleRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol, int index ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    QSGNode* updateSampleNode( const QskSkinnable*,
        QskAspect::Subcontrol, int index, QSGNode* ) const override;

  private:
    QSGNode* updateCursorLineNode( const QskSkinnable*, QSGNode* ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
