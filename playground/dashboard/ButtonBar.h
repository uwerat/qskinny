/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class ButtonBar : public QskLinearBox
{
  public:
    QSK_SUBCONTROLS( Indicator )

    ButtonBar( QQuickItem* = nullptr );
    void addIndicator( const char* name );

  protected:
    QSizeF layoutSizeHint( Qt::SizeHint, const QSizeF& ) const override;
};
