#include "test_QskRgbLiterals.h"
#include <QTest>
#include <QskRgbValue.h>

void QskRgbLiterals::parsing_data()
{
    QTest::addColumn< QString >( "text" );
    QTest::addColumn< QRgb >( "expected" );

    QTest::newRow( "" ) << "" << ( QRgb ) 0;
    QTest::newRow( "#" ) << "#" << ( QRgb ) 0;
    QTest::newRow( "#1" ) << "#1" << ( QRgb ) 0x1;
    QTest::newRow( "#12" ) << "#12" << ( QRgb ) 0x12;
    QTest::newRow( "#123" ) << "#123" << ( QRgb ) 0x123;
    QTest::newRow( "#1234" ) << "#1234" << ( QRgb ) 0x1234;
    QTest::newRow( "#12345" ) << "#12345" << ( QRgb ) 0x12345;
    QTest::newRow( "#123456" ) << "#123456" << ( QRgb ) 0x123456;
    QTest::newRow( "#1234567" ) << "#1234567" << ( QRgb ) 0x1234567;
    QTest::newRow( "#12345678" ) << "#12345678" << ( QRgb ) 0x12345678;
    QTest::newRow( "#123456789" ) << "#123456789" << ( QRgb ) 0x23456789;
}

void QskRgbLiterals::parsing()
{
    QFETCH( QString, text );
    QFETCH( QRgb, expected );

    const auto actual = text.toStdString();
    QCOMPARE( QskRgb::fromHexString( actual.c_str(), actual.size() ), expected );
}

void QskRgbLiterals::qrgbLiterals_data()
{
    QTest::addColumn< QRgb >( "actual" );
    QTest::addColumn< QRgb >( "expected" );

    using namespace QskRgb::literals::qrgb;

    QTest::newRow( "\"#123456\"_rgba" ) << "#123456"_rgba << ( QRgb ) 0xFF123456;
    QTest::newRow( "\"#123456\"_argb" ) << "#123456"_argb << ( QRgb ) 0xFF123456;

    QTest::newRow( "\"#AA112233\"_argb" ) << "#AA112233"_argb << ( QRgb ) 0xAA112233;
    QTest::newRow( "\"#112233AA\"_rgba" ) << "#112233AA"_rgba << ( QRgb ) 0xAA112233;

    QTest::newRow( "0x112233_rgba" ) << 0x112233_rgba << ( QRgb ) 0xFF112233;
    QTest::newRow( "0xaabbcc_rgba" ) << 0xaabbcc_rgba << ( QRgb ) 0xFFAABBCC;
    QTest::newRow( "0xAABBCC_rgba" ) << 0xAABBCC_rgba << ( QRgb ) 0xFFAABBCC;
    QTest::newRow( "0x112233aa_rgba" ) << 0x112233aa_rgba << ( QRgb ) 0xaa112233;

    QTest::newRow( "0x112233_argb" ) << 0x112233_argb << ( QRgb ) 0xFF112233;
    QTest::newRow( "0xaabbcc_argb" ) << 0xaabbcc_argb << ( QRgb ) 0xFFAABBCC;
    QTest::newRow( "0xAABBCC_argb" ) << 0xAABBCC_argb << ( QRgb ) 0xFFAABBCC;
    QTest::newRow( "0x112233aa_argb" ) << 0x112233aa_argb << ( QRgb ) 0x112233aa;
}

void QskRgbLiterals::qrgbLiterals()
{
    using namespace QskRgb::literals::qrgb;
    QFETCH( QRgb, actual );
    QFETCH( QRgb, expected );
    QCOMPARE( actual, expected );
}

void QskRgbLiterals::colorLiterals_data()
{
    QTest::addColumn< QColor >( "actual" );
    QTest::addColumn< QColor >( "expected" );

    using namespace QskRgb::literals::color;
    QTest::newRow( "\"#112233\"_rgba" ) << "#112233"_rgba << QColor::fromRgb( 0x11, 0x22, 0x33 );
    QTest::newRow( "\"#112233\"_argb" ) << "#112233"_argb << QColor::fromRgb( 0x11, 0x22, 0x33 );
    QTest::newRow( "\"#112233AA\"_rgba" )
        << "#112233AA"_rgba << QColor::fromRgb( 0x11, 0x22, 0x33, 0xAA );
    QTest::newRow( "\"#112233AA\"_argb" )
        << "#112233AA"_argb << QColor::fromRgb( 0x22, 0x33, 0xAA, 0x11 );

    QTest::newRow( "0x112233_rgba" ) << 0x112233_rgba << QColor::fromRgb( 0x11, 0x22, 0x33 );
    QTest::newRow( "0x112233_argb" ) << 0x112233_argb << QColor::fromRgb( 0x11, 0x22, 0x33 );
    QTest::newRow( "0x112233AA_rgba" )
        << 0x112233AA_rgba << QColor::fromRgb( 0x11, 0x22, 0x33, 0xAA );
    QTest::newRow( "0x112233AA_argb" )
        << 0x112233AA_argb << QColor::fromRgb( 0x22, 0x33, 0xAA, 0x11 );
}

void QskRgbLiterals::colorLiterals()
{
    QFETCH( QColor, actual );
    QFETCH( QColor, expected );
    QCOMPARE( actual, expected );
}
