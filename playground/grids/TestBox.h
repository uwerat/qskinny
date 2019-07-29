/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef TEST_BOX_H
#define TEST_BOX_H

#include "GridAccessor.h"
#include <QWidget>

class QLabel;

class TestBox : public QWidget, public GridAccessor
{
  public:
    enum
    {
        Skinny,
        Widgets,
        Graphics,
        Quick,

        GridCount
    };

    TestBox( QWidget* parent = nullptr );
    ~TestBox() override;

    void reset();

    void setColumns( int );
    void enableGrid( int, bool on );

    void insert( const QByteArray& colorName,
        int row, int column, int rowSpan, int columnSpan ) override;

    void setSpacing( Qt::Orientations, int spacing ) override;
    using GridAccessor::setSpacing;

    void setStretchFactor( int pos, Qt::Orientation, int stretch ) override;
    void setSizeHint( int pos, Qt::Orientation, Qt::SizeHint, int hint ) override;

    void setSizeHintAt( int index, Qt::Orientation, Qt::SizeHint, int hint ) override;
    void setSizePolicyAt( int index, Qt::Orientation, int policy ) override;
    void setAlignmentAt( int index, Qt::Alignment ) override;
    void setRetainSizeWhenHiddenAt( int index, bool on ) override;
    void setVisibleAt( int index, bool on ) override;

    QSize preferredSize() const override;

  protected:
    bool event( QEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void updateLabels();

  private:
    void layoutGrids();

    QWidget* m_grids[ GridCount ];
    QLabel* m_labels[ GridCount ];

    int m_columnCount = 2;
};

#endif
