/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef GRID_GRAPHICS_H
#define GRID_GRAPHICS_H

#include "GridAccessor.h"
#include <QGraphicsView>

class QGraphicsGridLayout;

class GridGraphics : public QGraphicsView, public GridAccessor
{
  public:
    GridGraphics( QWidget* parent = nullptr );
    ~GridGraphics() override;

    void insert( const QByteArray& colorName,
        int row, int column, int rowSpan, int columnSpan ) override;

    void setSpacing( Qt::Orientations, int spacing ) override;

    void setRowSizeHint( int row, Qt::SizeHint, int height ) override;
    void setColumnSizeHint( int column, Qt::SizeHint, int width ) override;

    void setSizeHint( int index, Qt::Orientation, Qt::SizeHint, int hint ) override;
    void setSizePolicy( int index, Qt::Orientation, int policy ) override;
    void setAlignment( int index, Qt::Alignment ) override;
    void setRetainSizeWhenHidden( int index, bool on ) override;

  protected:
    void resizeEvent( QResizeEvent* ) override;

  private:
    QGraphicsGridLayout* m_layout;
};

#endif
