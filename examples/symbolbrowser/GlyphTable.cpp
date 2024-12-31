#include "GlyphTable.h"

#include <qrawfont.h>
#include <private/qrawfont_p.h>
#include <qvarlengtharray.h>

#include <cstdio>
#include <QtEndian>

namespace PostTable
{
    /*
        https://learn.microsoft.com/en-us/typography/opentype/spec/post
     */

    inline QByteArray toByteArray( const uint8_t* s )
    {
        return QByteArray( reinterpret_cast< const char* > ( s + 1 ), *s );
    }

    QHash< QByteArray, int > glyphTab( const QByteArray& blob )
    {
        QHash< QByteArray, int > tab;

        const auto* glyphData = reinterpret_cast< const uint16_t* >( blob.constData() + 32 );
        if ( const auto nglyphs = qFromBigEndian( *glyphData++ ) )
        {
            QVarLengthArray< const uint8_t* > strings;
            strings.reserve( nglyphs );

            const auto from = reinterpret_cast< const uint8_t* >( glyphData + nglyphs );
            const auto to = reinterpret_cast< const uint8_t* >( blob.data() + blob.size() );

            for ( auto s = from; s < to; s += *s + 1 )
                strings += s;

            for ( int i = 0; i < nglyphs; i++ )
            {
                const int idx = qFromBigEndian( glyphData[i] ) - 258;

                if ( idx >= 0 )
                    tab.insert( toByteArray( strings[idx] ), i );
            }
        }

        return tab;
    }
}

namespace CompactFontFormatTable
{
    /*
        https://adobe-type-tools.github.io/font-tech-notes/pdfs/5176.CFF.pdf
     */

    uint32_t offsetAt( const uint8_t* d, int size )
    {
        switch (size)
        {
            case 0:
                return 0;
            case 1:
                return d[0];
            case 2:
                return ( d[0] << 8 ) | d[1];
            case 3:
                return ( d[0] << 16 ) | ( d[1] << 8 ) | d[2];
            default:
                return ( d[0] << 24 ) | ( d[1] << 16 ) | ( d[2] << 8 ) | d[3];
        }
    }

    static int indexDataSize( const uint8_t* d)
    {
        const int nitems = ( d[0] << 8 ) | d[1];
        if ( nitems == 0 )
            return 2;

        const int size = d[2];

        const uint8_t* lx = d + 3 + nitems * size;
        return lx + size - 1 + offsetAt( lx, size ) - d;
    }

    static const uint8_t* indexData( const uint8_t* d )
    {
        const int nitems = ( d[0] << 8 ) | d[1];
        d += 2;

        if ( nitems == 0 )
            return d;

        const int size = d[0];

        const uint8_t* _contents = d + ( nitems + 1 ) * size;
        return _contents + offsetAt(d + 1, size);
    }

    static const uint8_t* skipHeader( const uint8_t* d )
    {
        return d + d[2];
    }

    static const uint8_t* skipIndex( const uint8_t* d )
    {
        return d + indexDataSize( d );
    }


    static QVector< int > stringIds( const uint8_t* data, int nglyphs )
    {
        const int format = data[0];

        QVector< int > _sids;
        _sids += 0;

        const uint8_t* p = data + 1;
        switch( format )
        {
            case 0:
            {
                for (; _sids.size() < nglyphs; p += 2)
                {
                    int sid = ( p[0] << 8 ) | p[1];
                    _sids += sid;
                }
                break;
            }
            case 1:
            {
                for (; _sids.size() < nglyphs; p += 3)
                {
                    const int sid = ( p[0] << 8 ) | p[1];
                    const int n = p[2];

                    for ( int i = 0; i <= n; i++ )
                        _sids += sid + i;
                }
                break;
            }
            case 2:
            {
                for (; _sids.size() < nglyphs; p += 4)
                {
                    const int sid = ( p[0] << 8 ) | p[1];
                    const int n = ( p[2] << 8 ) | p[3];

                    for ( int i = 0; i <= n; i++ )
                        _sids += sid + i;
                }
                break;
            }
        }

        return _sids;
    }

    static int dictValue( const uint8_t* d, int op )
    {
        /*
            see https://adobe-type-tools.github.io/font-tech-notes/pdfs/5176.CFF.pdf
            We are intersted in the offsets ( operator 15/16/17 ).
         */

        const uint8_t* data = indexData( d );

        int value = 0;

        Q_FOREVER
        {
            // operand
            const auto valueType = data[0];

            if ( valueType == 28 )
            {
                value = ( data[1] << 8 ) | data[2];
                data += 3;
            }
            else if ( valueType == 29 )
            {
                value = ( data[1] << 24 ) | ( data[2] << 16 )
                    | ( data[3] << 8 ) | data[4];

                data += 5;
            }
            else if ( valueType == 30 )
            {
                /*
                    Assuming, that an offset is never a double
                    we skip the operand without reading it
                 */
                while( ++data )
                {
                    const int d = *data;
                    if ( ( d & 0x0f ) == 0x0f || ( d & 0xf0 ) == 0xf0 )
                        break; // 0xf nibble indicating the end
                }

                data++;
            }
            else if ( valueType >= 31 && valueType <= 246 )
            {
                value = data[0] - 139;
                data++;
            }
            else if ( valueType >= 247 && valueType <= 250 )
            {
                value = ( ( data[0] - 247 ) << 8 ) + data[1] + 108;
                data += 2;
            }
            else if ( valueType >= 251 && valueType <= 254 )
            {
                value = -( ( data[0] - 251 ) << 8 ) - data[1] - 108;
                data += 2;
            }

            // operator

            if ( op == data[0] )
                return value;

            data += ( data[0] == 12 ) ? 2 : 1;
        }

        return 0; // default value
    }

    class StringTable
    {
      public:
        StringTable(const uint8_t* data)
        {
            const int nitems = ( data[0] << 8 ) | data[1];
            if ( nitems == 0 )
            {
                _contents = data + 2;
                _offset = nullptr;
                _offsize = 0;
            }
            else
            {
                _offsize = data[2];
                _offset = data + 3;

                _contents = _offset + nitems * _offsize + _offsize - 1;
            }
        }

        inline QByteArray operator[](int which) const
        {
            const auto x = _offset + which * _offsize;

            const auto d1 = _contents + offsetAt(x, _offsize);
            const auto d2 = _contents + offsetAt(x + _offsize, _offsize);

            return QByteArray( reinterpret_cast< const char* >( d1 ), d2 - d1 );
        }

      private:
        const uint8_t* _contents = nullptr;
        const uint8_t* _offset = nullptr;
        int _offsize = -1;
    };
    QHash< QByteArray, int > glyphTable( const QByteArray& blob, int glyphCount )
    {
        auto data = reinterpret_cast< const uint8_t* >( blob.constData() );

        auto nameIndex = skipHeader( data );
        auto dictIndex = skipIndex( nameIndex );
        auto stringIndex = skipIndex( dictIndex );

        auto charset = data + dictValue( dictIndex, 15 ); // 15: charset offset

        const QVector< int > sids = stringIds( charset, glyphCount );

        const StringTable table( stringIndex );

        QHash< QByteArray, int > names;

        for ( int i = 0; i < glyphCount; i++ )
        {
            /*
                The first 391 SIDs are reserved for standard strings
                ( Appendix A ) that are not from the charset table.
             */

            const auto idx = sids[i] - 391;

            if ( idx >= 0 )
                names.insert( table[idx], i );
        }

        return names;
    }
}

static int glyphCount( const QRawFont& font )
{
    if ( !font.isValid() )
        return 0;

#if 0
    // https://learn.microsoft.com/en-us/typography/opentype/spec/maxp

    const auto table = font.fontTable( "maxp");
    if ( table.size() < 6 )
        return 0;

    //Big Endian
    const uint8_t* d = reinterpret_cast< const uint8_t* >( table.constData() ) + 4;
    return d[1] + ( d[0] << 8 );
#else
    return QRawFontPrivate::get( font )->fontEngine->glyphCount();
#endif
}

QHash< QByteArray, int > GlyphTable::nameTable( const QRawFont& font )
{
    if ( const auto count = glyphCount( font ) )
    {
        auto blob = font.fontTable( "CFF ");
        if ( !blob.isEmpty() )
            return CompactFontFormatTable::glyphTable( blob, count );

        blob = font.fontTable( "post" );
        if ( !blob.isEmpty() )
            return PostTable::glyphTab( blob );
    }

    return QHash< QByteArray, int >();
}

quint32 GlyphTable::ucs4ToIndex( const QRawFont& font, char32_t ucs4 )
{
    QString s;
    s += QChar::fromUcs4( ucs4 );

    const auto idxs = font.glyphIndexesForString( s );
    Q_ASSERT( idxs.size() == 1 );
    return idxs.size() == 1 ? idxs[0] : 0;
}

void GlyphTable::dump( const QHash< QByteArray, int >& hashTab )
{
    QMap< int, QByteArray > sorted;

    for ( auto it = hashTab.constBegin(); it != hashTab.constEnd(); ++it )
        sorted.insert( it.value(), it.key() );

    for ( auto it = sorted.constBegin(); it != sorted.constEnd(); ++it )
        printf("%d %s\n", it.key(), qPrintable( it.value() ) );
}

int GlyphTable::dump( const QHash< QByteArray, int >& table,
    const QRawFont& font, char32_t from, char32_t to )
{
    QHash< int, QByteArray > inverted;

    for ( auto it = table.constBegin(); it != table.constEnd(); ++it )
        inverted.insert( it.value(), it.key() );

    int n = 0;

    for ( auto c = from; c <= to; c++ )
    {
        if ( const auto glyphIndex = ucs4ToIndex( font, c ) )
        {
            const auto it = inverted.constFind( glyphIndex );
            if ( it != inverted.constEnd() )
            {
                printf("%X %d %s\n", c, it.key(), qPrintable( it.value() ) );

                n++;
            }
        }
    }

    return n;
}

void GlyphTable::dumpSymbols(
    const QHash< QByteArray, int >& table, const QRawFont& font )
{
    /*
        Unicode Private Use Areas (PUA)
        see https://en.wikipedia.org/wiki/Private_Use_Areas
     */

#if 0
    int count = 0;
    count += dump( table, font, 0xe000, 0xf8ff );
    count += dump( table, font, 0xf0000, 0xffffd );
    count += dump( table, font, 0x100000, 0x10fffd );

    qDebug() << "GlyphNames:" << table.count() << count << glyphCount( font );
#else
    dump( table );
    qDebug() << "GlyphNames:" << table.count() << glyphCount( font );
#endif
}

