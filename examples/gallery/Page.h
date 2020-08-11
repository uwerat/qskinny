/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef PAGE_H
#define PAGE_H

#include <QskLinearBox.h>

class Page : public QskLinearBox
{
  public:
    Page( Qt::Orientation, QQuickItem* parent = nullptr );
    void setGradient( const QskGradient& );
};

#endif
