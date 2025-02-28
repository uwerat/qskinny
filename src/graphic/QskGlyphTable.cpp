/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGlyphTable.h"
#include "QskGraphic.h"
#include "QskInternalMacros.h"

#include <qrawfont.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qendian.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qrawfont_p.h>
QSK_QT_PRIVATE_END

typedef QHash< QString, uint > GlyphNameTable;

/*
    The "parsers" below do no validation checks as the font has
    already been validated by QRawFont 

    Hope QRawFont will extend its API some day ( the underlying
    font libraries do support glyph names ) and we can remove the nasty
    code below. see https://bugreports.qt.io/browse/QTBUG-132629
 */
namespace PostTableParser
{
    // https://learn.microsoft.com/en-us/typography/opentype/spec/post

    static inline QString toString( const uint8_t* s )
    {
        // Pascal string. Valid characters are: [A–Z] [a–z] [0–9] '.' '_'
        return QString::fromUtf8(
            reinterpret_cast< const char* > ( s + 1 ), *s );
    }

    static GlyphNameTable glyphNames( const QByteArray& blob )
    {
        GlyphNameTable names;

        const auto* glyphData = reinterpret_cast< const uint16_t* >( blob.constData() + 32 );
        if ( const auto nglyphs = qFromBigEndian( *glyphData++ ) )
        {
            QVarLengthArray< const uint8_t* > strings;
            strings.reserve( nglyphs );

            const auto from = reinterpret_cast< const uint8_t* >( glyphData + nglyphs );
            const auto to = reinterpret_cast< const uint8_t* >( blob.data() + blob.size() );

            if ( to > from )
            {
                for ( auto s = from; s < to; s += *s + 1 )
                    strings += s;

                for ( int i = 0; i < nglyphs; i++ )
                {
                    const int idx = qFromBigEndian( glyphData[i] ) - 258;

                    if ( idx >= 0 && idx < strings.size() )
                        names.insert( toString( strings[idx] ), i );
                }
            }
        }

        return names;
    }
}

namespace CFFTableParser
{
    // https://adobe-type-tools.github.io/font-tech-notes/pdfs/5176.CFF.pdf

    static uint32_t offsetAt( const uint8_t* d, int size )
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

    static int indexDataSize( const uint8_t* d )
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
            We are intersted in the offset ( operator 15 ).
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
                    const int b = *data;
                    if ( ( b & 0x0f ) == 0x0f || ( b & 0xf0 ) == 0xf0 )
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
        StringTable( const uint8_t* data )
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

        inline QString operator[]( int which ) const
        {
            const auto x = _offset + which * _offsize;

            const auto d1 = _contents + offsetAt(x, _offsize);
            const auto d2 = _contents + offsetAt(x + _offsize, _offsize);

            return QString::fromUtf8(
                reinterpret_cast< const char* >( d1 ), d2 - d1 );
        }

      private:
        const uint8_t* _contents = nullptr;
        const uint8_t* _offset = nullptr;
        int _offsize = -1;
    };

    static GlyphNameTable glyphNames( const QByteArray& blob, int glyphCount )
    {
        auto data = reinterpret_cast< const uint8_t* >( blob.constData() );

        auto nameIndex = skipHeader( data );
        auto dictIndex = skipIndex( nameIndex );
        auto stringIndex = skipIndex( dictIndex );

        auto charset = data + dictValue( dictIndex, 15 ); // 15: charset offset

        const QVector< int > sids = stringIds( charset, glyphCount );

        const StringTable table( stringIndex );

        GlyphNameTable names;

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

static uint qskGlyphCount( const QRawFont& font )
{
    /*
        we could also read the count from the "maxp" table:
        https://learn.microsoft.com/en-us/typography/opentype/spec/maxp
     */

    const auto fontEngine = QRawFontPrivate::get( font )->fontEngine;
    return fontEngine ? fontEngine->glyphCount() : 0;
}

static qreal qskPixelSize( const QRawFont& font )
{
    const auto fontEngine = QRawFontPrivate::get( font )->fontEngine;
    return fontEngine ? fontEngine->fontDef.pixelSize : 0.0;
}

static GlyphNameTable qskGlyphNameTable( const QRawFont& font )
{
    const auto count = qskGlyphCount( font );
    if ( count > 0 )
    {
        /*
            The Compact Font Format ( CFF ) table has been introduced with
            the OpenType specification. For not complying fonts the names
            might be found in the Post table
         */
        auto blob = font.fontTable( "CFF ");
        if ( !blob.isEmpty() )
            return CFFTableParser::glyphNames( blob, count );

        blob = font.fontTable( "post" );
        if ( !blob.isEmpty() )
            return PostTableParser::glyphNames( blob );
    }

    return GlyphNameTable();
}

static inline quint32 qskGlyphIndex( const QRawFont& font, char32_t ucs4 )
{
    if ( qskGlyphCount( font ) == 0 )
        return 0;

    const auto idxs = font.glyphIndexesForString(
        QString::fromUcs4( &ucs4, 1 ) );

    // fingers crossed: icon fonts will map code points to single glyphs only
    Q_ASSERT( idxs.size() == 1 );

    return idxs.size() == 1 ? idxs[0] : 0;
}

class QskGlyphTable::PrivateData
{
  public:
    inline const GlyphNameTable& glyphNameTable() const
    {
        if ( !validNames )
        {
            auto that = const_cast< PrivateData* >( this );
            that->nameTable = qskGlyphNameTable( font );
            that->validNames = true;
        }

        return nameTable;
    }

    QRawFont font;
    GlyphNameTable nameTable;
    bool validNames = false;
};

QskGlyphTable::QskGlyphTable()
    : m_data( new PrivateData )
{
}

QskGlyphTable::QskGlyphTable( const QRawFont& font )
    : QskGlyphTable()
{
    m_data->font = font;
}

QskGlyphTable::QskGlyphTable( const QskGlyphTable& other )
    : QskGlyphTable()
{
    *m_data = *other.m_data;
}

QskGlyphTable::~QskGlyphTable()
{
}

QskGlyphTable& QskGlyphTable::operator=( const QskGlyphTable& other )
{
    if ( m_data != other.m_data )
        *m_data = *other.m_data;

    return *this;
}

void QskGlyphTable::setIconFont( const QRawFont& font )
{
    if ( font != m_data->font )
    {
        m_data->font = font;
        m_data->nameTable.clear();
        m_data->validNames = false;
    }
}

QRawFont QskGlyphTable::iconFont() const
{
    return m_data->font;
}

QPainterPath QskGlyphTable::glyphPath( uint glyphIndex ) const
{
    QPainterPath path;

    /*
        Unfortunately QRawFont::pathForGlyph runs into failing checks
        when being called from a different thread - f.e the scene graph thread.
        So we need to bypass QRawFont and retrieve from its fontEngine.
     */
    if ( auto fontEngine = QRawFontPrivate::get( m_data->font )->fontEngine )
    {
        QFixedPoint position;
        quint32 idx = glyphIndex;

        fontEngine->addGlyphsToPath( &idx, &position, 1, &path, {} );
    }

    return path;
}

QskGraphic QskGlyphTable::glyphGraphic( uint glyphIndex ) const
{
    QskGraphic graphic;

    if ( glyphIndex > 0 && qskGlyphCount( m_data->font ) > 0 )
    {
        const auto path = glyphPath( glyphIndex );

        if ( !path.isEmpty() )
        {
            // vertical glyph coordinates are in the range [-sz, 0.0]
            const auto sz = qskPixelSize( m_data->font );
            graphic.setViewBox( QRectF( 0.0, -sz, sz, sz ) );

            QPainter painter( &graphic );
            painter.setRenderHint( QPainter::Antialiasing, true );
            painter.fillPath( path, Qt::black );
        }
    }

    return graphic;
}

uint QskGlyphTable::glyphCount() const
{
    return qskGlyphCount( m_data->font );
}

uint QskGlyphTable::codeToIndex( char32_t ucs4 ) const
{
    return qskGlyphIndex( m_data->font, ucs4 );
}

uint QskGlyphTable::nameToIndex( const QString& name ) const
{
    /*
        Names/Codes that do not correspond to any glyph in should
        be mapped to glyph index 0.

        see https://learn.microsoft.com/en-us/typography/opentype/spec/cmap
     */
    return m_data->glyphNameTable().value( name.toLatin1(), 0 );
}

QHash< QString, uint > QskGlyphTable::nameTable() const
{
    return m_data->glyphNameTable();
}
