#include "GlyphTable.h"
#include <QskGlyphTable.h>

#include <QMap>
#include <QDebug>

static int dumpCodes( const QskGlyphTable& table, char32_t from, char32_t to )
{
    QHash< int, QString > inverted;

    const auto names = table.nameTable();
    for ( auto it = names.constBegin(); it != names.constEnd(); ++it )
        inverted.insert( it.value(), it.key() );

    int n = 0;

    for ( auto c = from; c <= to; c++ )
    {
        if ( const auto index = table.codeToIndex( c ) )
        {
            const auto it = inverted.constFind( index );
            if ( it != inverted.constEnd() )
            {
                printf("%X %d %s\n", c, it.key(), qPrintable( it.value() ) );
                n++;
            }
        }
    }

    return n;
}

void GlyphTable::dump( const QskGlyphTable& table )
{
    QMap< int, QString > sorted;

    {
        const auto names = table.nameTable();
        for ( auto it = names.constBegin(); it != names.constEnd(); ++it )
            sorted.insert( it.value(), it.key() );
    }

    for ( auto it = sorted.constBegin(); it != sorted.constEnd(); ++it )
        printf("%d %s\n", it.key(), qPrintable( it.value() ) );
}

void GlyphTable::dumpPrivateCodes( const QskGlyphTable& table )
{
#if 1
    /*
        Unicode Private Use Areas (PUA)
        see https://en.wikipedia.org/wiki/Private_Use_Areas
     */

    int count = 0;
    count += dumpCodes( table, 0xe000, 0xf8ff );
    count += dumpCodes( table, 0xf0000, 0xffffd );
    count += dumpCodes( table, 0x100000, 0x10fffd );

    qDebug() << "GlyphNames:" << table.count() << count << table.nameTable().count();
#else
    dump( table );
    qDebug() << "GlyphNames:" << table.count() << table.nameTable().count();
#endif
}

#include <QDir>
#include <QString>
#include <QRawFont>

void GlyphTable::dumpFont( const QString& path )
{
    const QRawFont font( path, 16 );
    Q_ASSERT( font.isValid() );

    //GlyphTable::dumpPrivateCodes( font );
    GlyphTable::dump( font );
}

void GlyphTable::dumpAllFonts()
{
    const auto entries = QDir( ":fonts" ).entryInfoList();
#if 0
    for ( const auto& entry : entries )
    {
        const auto path = entry.absoluteFilePath();
        qDebug() << path;
        dumpGlyphs( path );
    }
#else
    {
        const auto path = entries[1].absoluteFilePath();
        dumpFont( path );
        qDebug() << path;
    }
#endif
}
