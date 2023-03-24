#include "test_QskRgbLiterals.h"
#include <QTest>
#include <QskRgbValue.h>

#define QSK_REQUIRE_CONSTEXPR_LITERALS
#ifdef QSK_REQUIRE_CONSTEXPR_LITERALS
namespace QskRgb::literals::integral
{
    // RGBA hex string to QRgb with defaulted alpha = 0xFF
    static_assert( "#123456"_rgba == 0xFF123456, "invalid or not constexpr" );
    static_assert( "#123456"_argb == 0xFF123456, "invalid or not constexpr" );

    // ARGB hex string to QRgb with defaulted alpha = 0xFF
    static_assert( "#AA112233"_argb == 0xAA112233, "invalid or not constexpr" );
    static_assert( "#112233AA"_rgba == 0xAA112233, "invalid or not constexpr" );

    // RGBA hex literal to QRgb with defaulted alpha = 0xFF
    static_assert( 0x112233_rgba == 0xFF112233, "invalid or not constexpr" );
    static_assert( 0xaabbcc_rgba == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0xAABBCC_rgba == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0x112233aa_rgba == 0xaa112233, "invalid or not constexpr" );

    // ARGB hex literal to QRgb with defaulted alpha = 0xFF
    static_assert( 0x112233_argb == 0xFF112233, "invalid or not constexpr" );
    static_assert( 0xaabbcc_argb == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0xAABBCC_argb == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0x112233aa_argb == 0x112233aa, "invalid or not constexpr" );
}
#endif

void QskRgbLiterals::parsing_data()
{
    QTest::addColumn< QString >( "text" );
    QTest::addColumn< QRgb >( "expected" );

    QTest::newRow( "" ) << "" << ( QRgb ) 0;
    QTest::newRow( "#" ) << "#" << ( QRgb ) 0;
    QTest::newRow( "#1" ) << "#1" << ( QRgb ) 0;
    QTest::newRow( "#12" ) << "#12" << ( QRgb ) 0;
    QTest::newRow( "#123" ) << "#123" << ( QRgb ) 0;
    QTest::newRow( "#1234" ) << "#1234" << ( QRgb ) 0;
    QTest::newRow( "#12345" ) << "#12345" << ( QRgb ) 0;
    QTest::newRow( "#123456" ) << "#123456" << ( QRgb ) 0xFF123456;
    QTest::newRow( "#1234567" ) << "#1234567" << ( QRgb ) 0;
    QTest::newRow( "#12345678" ) << "#12345678" << ( QRgb ) 0x12345678;
    QTest::newRow( "#123456789" ) << "#123456789" << ( QRgb ) 0;
}

void QskRgbLiterals::parsing()
{
    QFETCH( QString, text );
    QFETCH( QRgb, expected );

    const auto actual = text.toStdString();
    QCOMPARE( QskRgb::fromHexString( actual.c_str(), actual.size(), QskRgb::ARGB ), expected );
}

void QskRgbLiterals::qrgbLiterals_data()
{
    QTest::addColumn< QRgb >( "actual" );
    QTest::addColumn< QRgb >( "expected" );

    using namespace QskRgb::literals::integral;

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
    using namespace QskRgb::literals::integral;
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
