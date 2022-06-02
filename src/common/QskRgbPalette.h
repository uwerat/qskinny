/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RGB_PALETTE_H
#define QSK_RGB_PALETTE_H

#include "QskGlobal.h"
#include "QskGradient.h"

#include <qmetaobject.h>
#include <qcolor.h>

class QSK_EXPORT QskRgbPalette
{
    Q_GADGET

  public:
    enum Weight
    {
        W0,
        W10,
        W20,
        W30,
        W40,
        W50,
        W60,
        W70,
        W80,
        W90,
        W95,
        W99,
        W100,

        NumWeights
    };
    Q_ENUM( Weight )

    enum Theme
    {
        DefaultMaterialPrimary,
        DefaultMaterialSecondary,
        DefaultMaterialTertiary,
        DefaultMaterialError,
        DefaultMaterialNeutral,
        DefaultMaterialNeutralVariant,

        NumThemes
    };
    Q_ENUM( Theme )

    static QskRgbPalette palette( Theme );

    inline QRgb rgb( Weight weight ) const
    {
        if ( weight < 0 || weight >= NumWeights )
            return 0;

        return m_rgb[ weight ];
    }

    inline QColor color( Weight weight ) const
    {
        return QColor::fromRgba( rgb( weight ) );
    }

    QskGradientStops colorStops( bool discrete = false ) const;

    static QskGradientStops colorStops( Theme, bool discrete = false );

    static QskGradientStops colorStops(
        const QVector< QRgb >&, bool discrete = false );

  protected:
    const QRgb* m_rgb;
};

#endif
