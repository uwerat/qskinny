/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskProgressRing.h>

class QskTextLabel;

class ValueMeter : public QskProgressRing
{
  public:
    ValueMeter( QQuickItem* parent = nullptr );

    void setTextColor( const QColor& );

  protected:
    virtual QString text( qreal ) const;

  private:
    void updateMeter( qreal value );
    QskTextLabel* m_label = nullptr;
};
