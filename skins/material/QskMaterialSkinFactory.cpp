/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "QskMaterialSkinFactory.h"
#include "QskMaterialSkin.h"

static const QString materialLightSkinName = QStringLiteral( "materialLight" );
static const QString materialDarkSkinName = QStringLiteral( "materialDark" );

QskMaterialSkinFactory::QskMaterialSkinFactory( QObject* parent )
    : QskSkinFactory( parent )
{
    using Q = QskRgbPalette;

    m_palettes[ Primary ] = Q::palette( Q::DefaultMaterialPrimary );
    m_palettes[ Secondary ] = Q::palette( Q::DefaultMaterialSecondary );
    m_palettes[ Tertiary ] = Q::palette( Q::DefaultMaterialTertiary );
    m_palettes[ Error ] = Q::palette( Q::DefaultMaterialError );
    m_palettes[ Neutral ] = Q::palette( Q::DefaultMaterialNeutral );
    m_palettes[ NeutralVariant ] = Q::palette( Q::DefaultMaterialNeutralVariant );
}

QskMaterialSkinFactory::~QskMaterialSkinFactory()
{
}

QStringList QskMaterialSkinFactory::skinNames() const
{
    return { materialLightSkinName, materialDarkSkinName };
}

QskSkin* QskMaterialSkinFactory::createSkin( const QString& skinName )
{
    if ( QString::compare( skinName, materialLightSkinName, Qt::CaseInsensitive ) == 0 )
    {
        // ### Move this to QskMaterialSkin?
        QskMaterialPalette pal;

        using Q = QskRgbPalette;

        pal.primary = m_palettes[ Primary ].rgb( Q::W40 );
        pal.onPrimary = m_palettes[ Primary ].rgb( Q::W100 );
        pal.primaryContainer = m_palettes[ Primary ].rgb( Q::W90 );
        pal.onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W10 );

        pal.primary = m_palettes[ Secondary ].rgb( Q::W40 );
        pal.onSecondary = m_palettes[ Secondary ].rgb( Q::W100 );
        pal.secondaryContainer = m_palettes[ Secondary ].rgb( Q::W90 );
        pal.onSecondaryContainer = m_palettes[ Secondary ].rgb( Q::W10 );

        pal.tertiary = m_palettes[ Tertiary ].rgb( Q::W40 );
        pal.onTertiary = m_palettes[ Tertiary ].rgb( Q::W100 );
        pal.tertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W90 );
        pal.onTertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W10 );

        pal.error = m_palettes[ Error ].rgb( Q::W40 );
        pal.onError = m_palettes[ Error ].rgb( Q::W100 );
        pal.errorContainer = m_palettes[ Error ].rgb( Q::W90 );
        pal.onErrorContainer = m_palettes[ Error ].rgb( Q::W10 );

        pal.background = m_palettes[ Neutral ].rgb( Q::W99 );
        pal.onBackground = m_palettes[ Neutral ].rgb( Q::W10 );
        pal.surface = m_palettes[ Neutral ].rgb( Q::W99 );
        pal.onSurface = m_palettes[ Neutral ].rgb( Q::W10 );

        pal.surfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W90 );
        pal.onSurfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W30 );
        pal.outline = m_palettes[ NeutralVariant ].rgb( Q::W50 );

        return new QskMaterialSkin( pal );
    }

    if ( QString::compare( skinName, materialDarkSkinName, Qt::CaseInsensitive ) == 0 )
    {
        QskMaterialPalette pal;

        using Q = QskRgbPalette;

        pal.primary = m_palettes[ Primary ].rgb( Q::W80 );
        pal.onPrimary = m_palettes[ Primary ].rgb( Q::W20 );
        pal.primaryContainer = m_palettes[ Primary ].rgb( Q::W30 );
        pal.onPrimaryContainer = m_palettes[ Primary ].rgb( Q::W90 );

        pal.primary = m_palettes[ Secondary ].rgb( Q::W80 );
        pal.onSecondary = m_palettes[ Secondary ].rgb( Q::W20 );
        pal.secondaryContainer = m_palettes[ Secondary ].rgb( Q::W30 );
        pal.onSecondaryContainer = m_palettes[ Secondary ].rgb( Q::W90 );

        pal.tertiary = m_palettes[ Tertiary ].rgb( Q::W80 );
        pal.onTertiary = m_palettes[ Tertiary ].rgb( Q::W20 );
        pal.tertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W30 );
        pal.onTertiaryContainer = m_palettes[ Tertiary ].rgb( Q::W90 );

        pal.error = m_palettes[ Error ].rgb( Q::W80 );
        pal.onError = m_palettes[ Error ].rgb( Q::W20 );
        pal.errorContainer = m_palettes[ Error ].rgb( Q::W30 );
        pal.onErrorContainer = m_palettes[ Error ].rgb( Q::W90 );

        pal.background = m_palettes[ Neutral ].rgb( Q::W10 );
        pal.onBackground = m_palettes[ Neutral ].rgb( Q::W90 );
        pal.surface = m_palettes[ Neutral ].rgb( Q::W10 );
        pal.onSurface = m_palettes[ Neutral ].rgb( Q::W80 );

        pal.surfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W30 );
        pal.onSurfaceVariant = m_palettes[ NeutralVariant ].rgb( Q::W80 );
        pal.outline = m_palettes[ NeutralVariant ].rgb( Q::W60 );

        return new QskMaterialSkin( pal );
    }

    return nullptr;
}

#include "moc_QskMaterialSkinFactory.cpp"
