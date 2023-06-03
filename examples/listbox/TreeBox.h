/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskTreeView.h>

class TreeBox final : public QskTreeView
{
    using Inherited = QskTreeView;

  public:
    QSK_SUBCONTROLS( FirstRow, SecondRow )

    explicit TreeBox( QQuickItem* parent = nullptr );
    QskAspect::Subcontrol rowSubControl( int row ) const noexcept override;
    int rowCount() const override;
    int columnCount() const override;
    qreal columnWidth( int col ) const override;
    qreal rowHeight() const override;
    Q_INVOKABLE QVariant valueAt( int row, int col ) const override;
    qreal rowOffset( int row ) const override;
};
