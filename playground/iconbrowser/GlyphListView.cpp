/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "GlyphListView.h"

#include <QskFunctions.h>
#include <QskGraphic.h>
#include <QskFontRole.h>

#include <QFontMetricsF>
#include <QRawFont>

constexpr int glyphSize = 100;

GlyphListView::GlyphListView( QQuickItem* parentItem )
    : Inherited( parentItem )
{
}

GlyphListView::GlyphListView( const QString& fontName, QQuickItem* parentItem )
    : GlyphListView( parentItem )
{
    setFont( QRawFont( fontName, 16 ) );
    setFontRoleHint( Text, QskFontRole::Title );
}

void GlyphListView::setFontPath( const QString& fontPath )
{
    setFont( QRawFont( fontPath, 16 ) );
}

void GlyphListView::setFont( const QRawFont& font )
{
    m_glyphTable.setIconFont( font );

    const auto names = m_glyphTable.nameTable();

    m_nameTable.clear();
    m_nameTable.reserve( names.size() );

    m_maxNameWidth = 0;

    {
        const QFontMetricsF fm( effectiveFont( Text ) );

        for ( auto it = names.constBegin(); it != names.constEnd(); ++it )
        {
            m_nameTable.insert( it.value(), it.key() );

            const qreal w = qskHorizontalAdvance( fm, it.key() );
            if ( w > m_maxNameWidth )
                m_maxNameWidth = w;
        }
    }

    updateScrollableSize();
    update();
}

QRawFont GlyphListView::font() const
{
    return m_glyphTable.iconFont();
}

int GlyphListView::rowCount() const
{
    return m_glyphTable.glyphCount() - 1;
}

int GlyphListView::columnCount() const
{
    return 3;
}

qreal GlyphListView::columnWidth( int col ) const
{
    switch( col )
    {
        case 0:
        {
            const QFontMetricsF fm( effectiveFont( Text ) );
            return qskHorizontalAdvance( fm, "999999" );
        }

        case 1:
            return glyphSize;

        case 2:
            return m_maxNameWidth;
    }

    return 0;
}

qreal GlyphListView::rowHeight() const
{
    return glyphSize + 20;
}

QVariant GlyphListView::valueAt( int row, int col ) const
{
    const auto glyphIndex = row + 1;
    switch( col )
    {
        case 0:
            return QVariant::fromValue( QString::number( glyphIndex ) );

        case 1:
            return QVariant::fromValue( m_glyphTable.glyphGraphic( glyphIndex ) );

        case 2:
            return QVariant::fromValue( m_nameTable.value( glyphIndex ) );
    }

    return QVariant();
}

#include "moc_GlyphListView.cpp"
