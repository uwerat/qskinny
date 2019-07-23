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
    MainBox( int id )
    {
        switch( id )
        {
            case 0:
                setup0();
                break;

            case 1:
                setup1();
                break;

            case 2:
                setup2();
                break;

            case 3:
                setup3();
                break;
        }
    }

  private:

    void setup0();
    void setup1();
    void setup2();
    void setup3();

    void setSizePolicyAt( int index,
        QskSizePolicy::Policy horizontalPolicy,
        QskSizePolicy::Policy verticalPolicy )
    {
        TestBox::setSizePolicyAt( index, Qt::Horizontal, horizontalPolicy );
        TestBox::setSizePolicyAt( index, Qt::Vertical, verticalPolicy );
    }
};

void MainBox::setup0()
{
    // something, that works with all layouts

    insert( "PaleVioletRed", 0, 0, 1, 1 );
    insert( "DarkSeaGreen", 1, 1, 1, 1 );
    insert( "SkyBlue", 2, 1, 1, 1 );
    insert( "Coral", 2, 2, 1, 1 );
    insert( "NavajoWhite", 3, 0, 1, 3 );
}

void MainBox::setup1()
{
    /*
        The Graphics layout adds the extra space to the stretchable
        while the other layouts use the extra space for reaching
        a ratio according to the stretch factor first,
        That leads to column 0 being too large.

     */

    setColumns( 1 );

    insert( "PaleVioletRed", 0, 0, 1, 1 );
    insert( "DarkSeaGreen", 1, 1, 1, 1 );
    insert( "SkyBlue", 2, 1, 1, 1 );
    insert( "Coral", 2, 2, 1, 1 );
    insert( "NavajoWhite", 3, 0, 1, 3 );

    setSizePolicyAt( 0, QskSizePolicy::Expanding, QskSizePolicy::Preferred );
    setSizePolicyAt( 1, QskSizePolicy::Fixed, QskSizePolicy::Preferred );
    setSizePolicyAt( 2, QskSizePolicy::Expanding, QskSizePolicy::Preferred );

#if 1
    // Quick layouts do not support row/column hints
    setColumnSizeHint( 1, Qt::MinimumSize, 100 );
#else
    for ( int i = 0; i < 4; i++ )
        setMinimumWidthAt( i, 100 );
#endif
}

void MainBox::setup2()
{
    /*
        The Graphics layout uses a "magic" formula for how to apply
        the stretch factors, while the other layouts result in
        predictable sizes being calculated according to the factors.
        In the specific situation column 1 in the Graphics layout is
        larger than twice of columns 0.
     */

    setColumns( 4 );

    insert( "PaleVioletRed", 0, 0, 1, 1 );
    insert( "DarkSeaGreen", 1, 0, 1, 1 );

    setRowStretchFactor( 0, 1 );
    setRowStretchFactor( 1, 2 );
}

void MainBox::setup3()
{
    /*
        When setting a maximum size together with an alignment the expected result
        would be a cell that can grow beyond the maximum, while the content stays
        at its maximum, being aligned inside the cell.

        But with widgets, the cell size gets always bounded to the preferred size
        as soon as an alignment has been set. In ths

        With graphics we have the effect, that the cells get bounded to the
        maximum size and we always end up with being aligned Top | Left.

     */
    setColumns( 1 );

    insert( "PaleVioletRed", 0, 0, 1, 1 );

    setPreferredWidthAt( 0, 10 );
    setPreferredHeightAt( 0, 10 );

    setMaximumWidthAt( 0, 200 );
    setMaximumHeightAt( 0, 200 );

    setAlignmentAt( 0, Qt::AlignCenter );
}

int main( int argc, char** argv )
{
    QApplication a( argc, argv );

    MainBox box( 0 );

    box.resize( 600, 600 );
    box.show();

    return a.exec();
}
