/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGlyphGraphicProvider.h"
#include "QskGraphic.h"
#include "QskGlyphTable.h"

#include <qrawfont.h>
#include <qpainter.h>
#include <qpainterpath.h>

class QskGlyphGraphicProvider::PrivateData
{
  public:
    QskGlyphTable glyphTable;
};

QskGlyphGraphicProvider::QskGlyphGraphicProvider( QObject* parent )
    : QskGraphicProvider( parent )
    , m_data( new PrivateData )
{
}

QskGlyphGraphicProvider::~QskGlyphGraphicProvider()
{
}

void QskGlyphGraphicProvider::setIconFont( const QRawFont& font )
{
    m_data->glyphTable.setIconFont( font );
}

QRawFont QskGlyphGraphicProvider::iconFont() const
{
    return m_data->glyphTable.iconFont();
}

QskGraphic QskGlyphGraphicProvider::glyphGraphic( uint index ) const
{
    return m_data->glyphTable.glyphGraphic( index );
}

const QskGraphic* QskGlyphGraphicProvider::loadGraphic( const QString& key ) const
{
    if ( const auto index = glyphIndex( key ) )
    {
        const auto graphic = glyphGraphic( index );
        if ( !graphic.isNull() )
            return new QskGraphic( graphic );
    }

    return nullptr;
}

uint QskGlyphGraphicProvider::glyphIndex( const QString& key ) const
{
    const auto& table = m_data->glyphTable;

    if ( ( table.glyphCount() > 0 ) && !key.isEmpty() )
    {
        if ( key.startsWith( '#' ) )
        {
            bool ok;
            const auto glyphIndex = key.toUInt( &ok );
            if ( ok )
                return glyphIndex;
        }
        else if ( key.startsWith( "U+" ) )
        {
            bool ok;
            const char32_t code = key.mid( 2 ).toUInt( &ok, 16 );
            if ( ok )
                return table.codeToIndex( code );
        }
        else
        {
            return table.nameToIndex( key );
        }
    }

    return 0;
}
