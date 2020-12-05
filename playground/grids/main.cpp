/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "TestBox.h"

#include <QskSizePolicy.h>
#include <QskFunctions.h>

#include <QApplication>
#include <QListWidget>
#include <QDebug>

#include <functional>
#include <vector>

class Box : public TestBox
{
  public:
    Box( QWidget* parent = nullptr )
        : TestBox( parent )
    {
        m_tests =
        {
            &Box::test0, &Box::test1, &Box::test2,
            &Box::test3, &Box::test4, &Box::test5,
            &Box::test6, &Box::test7, &Box::test8,
            &Box::test9, &Box::test10, &Box::test11,
            &Box::test12
        };
    }

    void showTest( int id )
    {
        reset();

        id = qBound( 0, id, count() - 1 );
        m_tests[id]( *this );

        updateLabels();
    }

    int count() const
    {
        return m_tests.size();
    }

  private:

    void insert( const char* colorName, int row, int column )
    {
        insert( colorName, row, column, 1, 1 );
    }

    using TestBox::insert;

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
    void test10();
    void test11();
    void test12();

    std::vector< std::function< void( Box& ) > > m_tests;
};

void Box::test0()
{
    // something, that works with all layouts

    insert( "PaleVioletRed", 0, 0 );
    insert( "DarkSeaGreen", 1, 1 );
    insert( "SkyBlue", 2, 1 );
    insert( "Coral", 2, 2 );
    insert( "NavajoWhite", 3, 0, 1, 3 );
}

void Box::test1()
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

void Box::test2()
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

void Box::test3()
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

void Box::test4()
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

void Box::test5()
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

    setAlignmentAt( 0, Qt::AlignCenter );
    setAlignmentAt( 1, Qt::AlignCenter );
}

void Box::test6()
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

void Box::test7()
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

void Box::test8()
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

void Box::test9()
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

void Box::test10()
{
    /*
        Widgets gives space to the bounded columns 1/2 instead
        of 0/3, where they would be more useful.
     */
    insert( "PaleVioletRed", 0, 0 );
    insert( "NavajoWhite", 0, 1, 1, 2 );
    insert( "Coral", 0, 3 );

    setMaximumWidthAt( 1, 70 );
}

void Box::test11()
{
    /*
        Here we have an expandble element, that is limited by
        its maximum size and another element, that can grow only.

        The expected behavior is, that if the expandable elements
        do not accept the complete available space the rest goes to
        the elements, that can grow only.

        - Graphic/Skinny do not handle the grow flag once there is
          an element with the expand flag
     */

    // Quick layouts do not support GrowFlag vs. ExpandFlag
    enableGrid( Quick, false );

    insert( "PaleVioletRed", 0, 0 );
    insert( "NavajoWhite", 1, 0 );

    setSizePolicyAt( 0, Qt::Vertical, QskSizePolicy::Preferred );
    setPreferredHeightAt( 0, 50 );

    setSizePolicyAt( 1, Qt::Vertical, QskSizePolicy::Expanding );
    setMaximumHeightAt( 1, 50 );
}

void Box::test12()
{
    /*
         We have different strategies where to put the spaces, when
         there is more space than the elements are willing to accept.
         QSkinny has the extraSpacingAt parameter to control this.
     */

    insert( "PaleVioletRed", 0, 0 );
    insert( "NavajoWhite", 1, 0 );
    insert( "Coral", 2, 0 );

    for ( int i = 0; i < 3; i++ )
        setFixedHeightAt( i, 50 );
}

class MainWidget : public QWidget
{
  public:
    MainWidget( int id )
    {
        m_box = new Box( this );
        m_box->showTest( id );

        m_listBox = new QListWidget( this );
        for ( int i = 0; i < m_box->count(); i++ )
            m_listBox->addItem( QStringLiteral( "Test " ) + QString::number( i ) );

        m_listBox->setCurrentRow( id );

        connect( m_listBox, &QListWidget::currentRowChanged,
            m_box, &Box::showTest );
    }

  protected:
    void resizeEvent( QResizeEvent* ) override
    {
        /*
            Not using layouts here to avoid confusion
            while debugging.
         */

        const auto r = contentsRect();
        const int spacing = 5;

        const int w1 = qskHorizontalAdvance( m_listBox->font(), "Test 100" ) + 20;
        const int w2 = r.width() - w1 - spacing;

        m_listBox->setGeometry( r.left(), r.top(), w1, r.height() );
        m_box->setGeometry( r.right() - w2, r.top(), w2, r.height() );

        m_listBox->setFocus();
    }

  private:
    QListWidget* m_listBox;
    Box* m_box;
};

int main( int argc, char** argv )
{
    QApplication a( argc, argv );

    int testcase = 0;
    if ( argc == 2 )
        testcase = atoi( argv[1] );

    MainWidget w( testcase );

    w.resize( 600, 600 );
    w.show();

    return a.exec();
}
