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

    virtual void setSizeHint( int pos, Qt::Orientation, Qt::SizeHint, int height ) = 0;
    void setRowSizeHint( int row, Qt::SizeHint, int height );
    void setColumnSizeHint( int column, Qt::SizeHint, int width );

    void setRowFixedHeight( int row, qreal height );
    void setColumnFixedWidth( int column, qreal width );

    virtual void setStretchFactor( int pos, Qt::Orientation, int stretch ) = 0;
    void setRowStretchFactor( int row, int stretch );
    void setColumnStretchFactor( int row, int stretch );

    virtual void setSizeHintAt( int index, Qt::Orientation, Qt::SizeHint, int hint ) = 0;
    virtual void setSizePolicyAt( int index, Qt::Orientation, int policy ) = 0;

    void setSizePoliciesAt( int index, int horizontalPolicy, int verticalPolicy );

    void setMinimumWidthAt( int index, int hint );
    void setMinimumHeightAt( int index, int hint );
    void setMinimumSizeAt( int index, const QSize& );

    void setPreferredWidthAt( int index, int hint );
    void setPreferredHeightAt( int index, int hint );
    void setPreferredSizeAt( int index, const QSize& );

    void setMaximumWidthAt( int index, int hint );
    void setMaximumHeightAt( int index, int hint );
    void setMaximumSizeAt( int index, const QSize& );

    void setFixedWidthAt( int index, int hint );
    void setFixedHeightAt( int index, int hint );
    void setFixedSizeAt( int index, const QSize& );

    virtual void setAlignmentAt( int index, Qt::Alignment ) = 0;
    virtual void setRetainSizeWhenHiddenAt( int index, bool on ) = 0;
    virtual void setVisibleAt( int index, bool on ) = 0;

    virtual QSize preferredSize() const = 0;
};

#endif
