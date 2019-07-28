/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"

#include <QskSizePolicy.h>
#include <QApplication>
#include <QDebug>

class MainBox : public TestBox
{
  public:
    MainBox( int id )
    {
        using Testcase = void ( MainBox::* )();

        const Testcase tests[] =
        {
            &MainBox::test0, &MainBox::test1, &MainBox::test2,
            &MainBox::test3, &MainBox::test4, &MainBox::test5,
            &MainBox::test6, &MainBox::test7, &MainBox::test8,
            &MainBox::test9
        };

        const int count = static_cast< int >( sizeof( tests ) / sizeof( tests[0] ) );

        if ( id < 0 || id >= count )
            id = 0;

        ( this->*tests[id] )();
    }

    void insert( const char* colorName, int row, int column )
    {
        insert( colorName, row, column, 1, 1 );
    }

    using TestBox::insert;

  private:

    void test0();
    void test1();
    void test2();
    void test3();
    void test4();
    void test5();
    void test6();
    void test7();
    void test8();
    void test9();
};

void MainBox::test0()
{
    // something, that works with all layouts

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );
    insert( "SkyBlue", 2, 1 );
    insert( "Coral", 2, 2 );
    insert( "NavajoWhite", 3, 0, 1, 3 );
}

void MainBox::test1()
{
    /*
        Graphics layout adds the extra space to the stretchables
        while the other layouts use the extra space for reaching
        a ratio according to the stretch factor first,
        That leads to column 0 being too large.
     */

    setColumns( 1 );

    // Quick layouts do not support row/column hints
    enableGrid( Quick, false );

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );
    insert( "SkyBlue", 2, 1 );
    insert( "Coral", 2, 2 );

    setSizePolicyAt( 0, Qt::Horizontal, QskSizePolicy::Expanding );
    setSizePolicyAt( 1, Qt::Horizontal, QskSizePolicy::Fixed );
    setSizePolicyAt( 2, Qt::Horizontal, QskSizePolicy::Expanding );
    setSizePolicyAt( 3, Qt::Horizontal, QskSizePolicy::Fixed );

    setColumnSizeHint( 1, Qt::MinimumSize, 100 );
}

void MainBox::test2()
{
    setColumns( 1 );

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );
    insert( "SkyBlue", 2, 1 );
    insert( "Coral", 2, 2 );

    setSizePolicyAt( 0, Qt::Horizontal, QskSizePolicy::Expanding );
    setSizePolicyAt( 1, Qt::Horizontal, QskSizePolicy::Fixed );
    setSizePolicyAt( 2, Qt::Horizontal, QskSizePolicy::Expanding );
    setSizePolicyAt( 3, Qt::Horizontal, QskSizePolicy::Fixed );

    setMinimumWidthAt( 1, 100 );
    setMinimumWidthAt( 2, 100 );
}

void MainBox::test3()
{
    /*
        The Graphics layout uses a "magic" formula for how to apply
        the stretch factors, while the other layouts result in
        predictable sizes.
        In the specific situation column 1 in the Graphics layout is
        larger than twice of columns 0.
     */

    setColumns( 3 );

    // Quick layouts do not support row/column hints
    enableGrid( Quick, false );

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 0 );

    setRowStretchFactor( 0, 1 );
    setRowStretchFactor( 1, 2 );
}

void MainBox::test4()
{
    /*
        When setting a maximum size together with an alignment the expected result
        would be a cell that can grow beyond the maximum, while the content stays
        at its maximum, being aligned inside the cell.

        But with widgets, the cell size gets always bounded to the preferred size
        as soon as an alignment has been set.

        With graphics we have the effect, that the cells gets bounded to the
        maximum size and we always end up with being aligned Top | Left.

     */
    setColumns( 1 );

    insert( "PaleVioletRed", 0, 0 );

    setPreferredSizeAt( 0, QSize( 10, 10 ) );
    setMaximumSizeAt( 0, QSize( 80, 80 ) );

    setAlignmentAt( 0, Qt::AlignCenter );
}

void MainBox::test5()
{
    /*
        QGridLayoutEngine does not work correctly when putting more
        than one element into the same cell. For the specific situation
        below we have a wrong preferredSize for Quick and Graphic as only the
        last element being inserted one cell goes into the calculation.

        The reason behind this limitation is a second list for the elements
        organized according to row/column that can only store one
        element per cell. Unfortunately the implementation iterates in several
        situations over rows/columns using this list.
        Iterating over the elements instead avoids this limitation and would
        be more efficient as well.

        Actually the row/column organized list is ony useful for faster lookups,
        when retrieving an item at a specific cell - beside that it
        complicates the code without offering extra benefits.

        The position of the rectangles differs because the systems have
        default alignments.
     */

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 0, 0 );

    setFixedSizeAt( 0, QSize( 100, 100 ) );
    setFixedSizeAt( 1, QSize( 50, 50 ) );
}

void MainBox::test6()
{
    /*
        QGridLayoutEngine ignores the first column coming from
        the multicell element at the bottom
     */
    insert( "DarkSeaGreen", 1, 1 );
    insert( "Coral", 2, 2 );
    insert( "NavajoWhite", 3, 0, 1, 3 );

    //setSpacing( 0 );
}

void MainBox::test7()
{
    /*
        This test is very similar to test6, but here we can see a
        difference between Quick and Graphic. The hidden element has
        an effect on the layout for Graphic, what is actually wrong, when
        setRetainSizeWhenHidden is not set. The preferred width also
        includes a wrong contribution from the hidden element.
     */
    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );
    insert( "Coral", 2, 2 );
    insert( "NavajoWhite", 3, 0, 1, 3 );

    setVisibleAt( 0, false );
    //setRetainSizeWhenHiddenAt( 0, true );
}

void MainBox::test8()
{
    /*
        This test creates a situation, where we have more space than
        the minimum, but not enough for preferred. For this situation
        all layout engines use a different algorithm how to distribute
        the extra space. All of them are based on the difference between
        preferred and minimum.

        - Skinny simply uses the ratio of the differences
        - Widgets seems to do something that works exponatially
          ( need to check the code ).
        - Graphic/Quick ( QGridLayoutEngine ) levels the impact
          of the differences down.

        Hard to say what a user expects to happen.
     */
    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );

    for ( int i = 0; i < 2; i++ )
    {
        setMinimumSizeAt( i, QSize( 20, 20 ) );
        setPreferredSizeAt( i, ( i + 1 ) * QSize( 2000, 2000 ) );
    }
}

void MainBox::test9()
{
    /*
        This one is a bit unclear about how much space from the first
        element goes to column 1/2. Skinny gives 50% to both,
        while QGridLayoutEngine gives 100% to column 1 ( starting
        at 0 + spacing() ). The Widgets implementation seems to be
        wrong as column 1 grows beyond 100%.

        The situation can be clarified by using setColumnFixedWidth -
        unfortunately Quick does not support this.
     */
    insert( "PaleVioletRed", 0, 0, 1, 2 );
    insert( "DarkSeaGreen", 1, 1 );

    setFixedWidthAt( 0, 100 );
    //setColumnFixedWidth( 0, 50 );
}

int main( int argc, char** argv )
{
    QApplication a( argc, argv );

    int testcase = 0;
    if ( argc == 2 )
        testcase = atoi( argv[1] );

    MainBox box( testcase );

    box.resize( 600, 600 );
    box.show();

    return a.exec();
}
