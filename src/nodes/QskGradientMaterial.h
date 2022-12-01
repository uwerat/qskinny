/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_MATERIAL
#define QSK_GRADIENT_MATERIAL

#include "QskGlobal.h"
#include "QskGradient.h"
#include <qsgmaterial.h>

class QSK_EXPORT QskGradientMaterial : public QSGMaterial
{
  public:
    static QskGradientMaterial* createMaterial( QskGradient::Type );

    bool updateGradient( const QRectF&, const QskGradient& );
    QskGradient::Type gradientType() const;

    const QskGradientStops& stops() const;
    QskGradient::Spread spread() const;

  protected:
    QskGradientMaterial( QskGradient::Type );

    void setStops( const QskGradientStops& );
    void setSpread( QskGradient::Spread );

  private:
    const QskGradient::Type m_gradientType;

    QskGradientStops m_stops;
    QskGradient::Spread m_spread = QskGradient::PadSpread;
};

inline QskGradient::Type QskGradientMaterial::gradientType() const
{
    return m_gradientType;
}

inline void QskGradientMaterial::setStops( const QskGradientStops& stops )
{
    m_stops = stops;
}

inline void QskGradientMaterial::setSpread( QskGradient::Spread spread )
{
    m_spread = spread;
}

inline const QskGradientStops& QskGradientMaterial::stops() const
{
    return m_stops;
}

inline QskGradient::Spread QskGradientMaterial::spread() const
{
    return m_spread;
}

#endif
