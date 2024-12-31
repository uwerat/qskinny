
/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSymbolProvider.h"
#include "QskGraphic.h"

#include <qrawfont.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qdebug.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qrawfont_p.h>
QSK_QT_PRIVATE_END

static void qskDumpGlyphNames( const QRawFont& rawFont )
{
    auto d = QRawFontPrivate::get( rawFont );
    qDebug() << d->fontEngine->type();
    /*
        - FT_Get_Name_Index
        - https://stackoverflow.com/questions/23185221/getting-the-glyph-name-from-a-ttf-or-otf-font-file
     */
}

static QskGraphic qskRenderGraphic( const QRawFont& rawFont, QChar c )
{
    quint32 indices[10];
    int numIndices = sizeof( indices ) / sizeof( indices[0] );

    (void) rawFont.glyphIndexesForChars( &c, 1, indices, &numIndices );
    Q_ASSERT( numIndices > 0 );

#if 1
    if ( numIndices > 1 )
        qWarning() << "character has more than one glyph:" << c;
#endif

    QskGraphic graphic;

    QPainter painter( &graphic );
    painter.setRenderHint( QPainter::Antialiasing, true );

    for ( int i = 0; i < numIndices; i++ )
    {
        auto path = rawFont.pathForGlyph( indices[0] );
        if ( path.isEmpty() )
        {
            qWarning() << "character has no path:" << c;
        }
        else
        {
            path.setFillRule(Qt::WindingFill);
            painter.fillPath( path, Qt::black );
        }
    }

    return graphic;
}

class QskSymbolProvider::PrivateData
{
  public:
    QRawFont font;
};

QskSymbolProvider::QskSymbolProvider( QObject* parent )
    : QskGraphicProvider( parent )
    , m_data( new PrivateData )
{
}

QskSymbolProvider::~QskSymbolProvider()
{
}

void QskSymbolProvider::setRawFont( const QRawFont& font )
{
    m_data->font = font;
}

void QskSymbolProvider::setFont( const QFont& font )
{
    m_data->font = QRawFont::fromFont( font );
}

void QskSymbolProvider::setFontData( const QByteArray& data )
{
    m_data->font.loadFromData( data, 16, QFont::PreferDefaultHinting );
#if 1
    qskDumpGlyphNames( m_data->font );
#endif
}

const QskGraphic* QskSymbolProvider::loadGraphic( const QString& ) const
{
    return nullptr;
}

QskGraphic* QskSymbolProvider::requestSymbol( uint ucs4 ) const
{
    if ( ucs4 != 0xffff )
    {
        const auto graphic = qskRenderGraphic( m_data->font, QChar( ucs4 ) );
        if ( !graphic.isNull() )
            return new QskGraphic( graphic );
    }

    return nullptr;
}

uint QskSymbolProvider::glyphIndex( const QString& glyphName ) const
{
    Q_UNUSED( glyphName );
    return 0;
}

QStringList QskSymbolProvider::glyphNames() const
{
    return QStringList();
}

