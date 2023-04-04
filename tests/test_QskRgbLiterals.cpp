#include "test_QskRgbLiterals.h"
#include <QTest>
#include <QskRgbValue.h>

#define QSK_REQUIRE_CONSTEXPR_LITERALS
#ifdef QSK_REQUIRE_CONSTEXPR_LITERALS
namespace QskRgb::literals::integral
{
    // RGBA hex literal to QRgb with defaulted alpha = 0xFF
    static_assert( 0x112233_rgba == 0xFF112233, "invalid or not constexpr" );
    static_assert( 0xAaBbCc_rgba == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0x112233Aa_rgba == 0xAA112233, "invalid or not constexpr" );

    // ARGB hex literal to QRgb with defaulted alpha = 0xFF
    static_assert( 0x112233_argb == 0xFF112233, "invalid or not constexpr" );
    static_assert( 0xAaBbCc_argb == 0xFFAABBCC, "invalid or not constexpr" );
    static_assert( 0x112233Aa_argb == 0x112233aa, "invalid or not constexpr" );
}
#endif

namespace QskRgb::literals
{
    static_assert( std::is_same_v< decltype( 0x112233_qrgba ), QRgb >, "Invalid return type!" );
    static_assert( std::is_same_v< decltype( 0x112233_qargb ), QRgb >, "Invalid return type!" );
    static_assert( std::is_same_v< decltype( 0x112233_cargb ), QColor >, "Invalid return type!" );
    static_assert( std::is_same_v< decltype( 0x112233_crgba ), QColor >, "Invalid return type!" );
}

void QskRgbLiterals::qrgbLiterals_data()
{
    QTest::addColumn< QRgb >( "actual" );
    QTest::addColumn< QRgb >( "expected" );

    using namespace QskRgb::literals::integral;

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
