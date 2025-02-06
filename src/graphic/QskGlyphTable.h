/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GLYPH_TABLE_H
#define QSK_GLYPH_TABLE_H

#include "QskGlobal.h"
#include <memory>

class QskGraphic;
class QRawFont;
class QString;
class QPainterPath;
class QByteArray;
class QChar;

template< typename Key, typename T > class QHash;

class QSK_EXPORT QskGlyphTable
{
  public:
    QskGlyphTable();
    QskGlyphTable( const QRawFont& );
    QskGlyphTable( const QskGlyphTable& );

    ~QskGlyphTable();

    QskGlyphTable& operator=( const QskGlyphTable& );

    bool isValid() const;

    void setIconFont( const QRawFont& );
    QRawFont iconFont() const;

    uint glyphCount() const;

    QPainterPath glyphPath( uint glyphIndex ) const;
    QskGraphic glyphGraphic( uint glyphIndex ) const;

    /*
        Most icon fonts use code points from the Unicode Private Use Areas (PUA)
        ( see https://en.wikipedia.org/wiki/Private_Use_Areas ):

        - [0x00e000, 0x00f8ff]
        - [0x0f0000, 0x0ffffd]
        - [0x100000, 0x10fffd]

        Note that QChar is 16-bit entity only that does not cover the higher PUA blocks.
     */
    uint codeToIndex( char32_t ) const;

    /*
        True/OpenType fonts often include a table with glyph names, that can be used
        instead of the glyph index
     */
    uint nameToIndex( const QString& ) const;

    QHash< QString, uint > nameTable() const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline bool QskGlyphTable::isValid() const
{
    return glyphCount() > 0;
}

#endif
