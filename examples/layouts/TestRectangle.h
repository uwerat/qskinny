/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef TEST_RECTANGLE
#define TEST_RECTANGLE 1

#include <QskTextLabel.h>

class TestRectangle : public QskTextLabel
{
  public:
    TestRectangle( const char* colorName, QQuickItem* parent = nullptr );
};

#endif
