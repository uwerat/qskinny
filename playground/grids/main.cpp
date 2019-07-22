/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"
#include <QApplication>
#include <QskSizePolicy.h>

class MainBox : public TestBox
{
  public:
    MainBox()
    {
        insert( "PaleVioletRed", 0, 0, 1, 1 );
        insert( "DarkSeaGreen", 1, 1, 1, 1 );
        insert( "SkyBlue", 2, 1, 1, 1 );
        insert( "Coral", 2, 2, 1, 1 );
        insert( "NavajoWhite", 3, 0, 1, 3 );

        setSizePolicy( 1, QskSizePolicy::Fixed, QskSizePolicy::Preferred );
        setSizePolicy( 2, QskSizePolicy::Fixed, QskSizePolicy::Preferred );
        setColumnSizeHint( 1, Qt::MinimumSize, 100 );
    }

  private:
    void setSizePolicy( int index,
        QskSizePolicy::Policy horizontalPolicy,
        QskSizePolicy::Policy verticalPolicy )
    {
        TestBox::setSizePolicy( index, Qt::Horizontal, horizontalPolicy );
        TestBox::setSizePolicy( index, Qt::Vertical, verticalPolicy );
    }
};

int main( int argc, char** argv )
{
    QApplication a( argc, argv );

    MainBox box;
    box.resize( 600, 600 );
    box.show();

    return a.exec();
}
