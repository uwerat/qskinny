/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef GRID_ACCESSOR_H
#define GRID_ACCESSOR_H

#include <Qt>

class QColor;
class QSize;
class QByteArray;

class GridAccessor
{
  public:
    virtual ~GridAccessor() = default;

    virtual void insert( const QByteArray& colorName,
        int row, int column, int rowSpan, int columnSpan ) = 0;

    void setSpacing( int spacing );
    virtual void setSpacing( Qt::Orientations, int spacing ) = 0;

    virtual void setRowSizeHint( int row, Qt::SizeHint, int height ) = 0;
    virtual void setColumnSizeHint( int column, Qt::SizeHint, int width ) = 0;

    void setRowFixedHeight( int row, qreal height );
    void setColumnFixedWidth( int column, qreal width );

    virtual void setSizeHint( int index, Qt::Orientation, Qt::SizeHint, int hint ) = 0;
    virtual void setSizePolicy(int index, Qt::Orientation, int policy ) = 0;

    void setSizePolicy( int index, int horizontalPolicy, int verticalPolicy );

    void setMinimumWidth( int index, int hint );
    void setMinimumHeight( int index, int hint );
    void setMinimumSize( int index, const QSize& );

    void setPreferredWidth( int index, int hint );
    void setPreferredHeight( int index, int hint );
    void setPreferredSize( int index, const QSize& );

    void setMaximumWidth( int index, int hint );
    void setMaximumHeight( int index, int hint );
    void setMaximumSize( int index, const QSize& );

    virtual void setAlignment( int index, Qt::Alignment ) = 0;
    virtual void setRetainSizeWhenHidden( int, bool on ) = 0;
};

#endif
