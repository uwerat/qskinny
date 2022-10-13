/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_MATERIAL
#define QSK_GRADIENT_MATERIAL

#include "QskGlobal.h"
#include <qbrush.h>
#include <qsgmaterial.h>

class QSK_EXPORT QskGradientMaterial : public QSGMaterial
{
  public:
    static QskGradientMaterial* createMaterial( QGradient::Type );

    bool updateGradient( const QRectF&, const QGradient*, qreal );
    QGradient::Type gradientType() const;

    const QGradientStops& stops() const;
    QGradient::Spread spread() const;

  protected:
    QskGradientMaterial( QGradient::Type );

    void setStops( const QGradientStops& );
    void setSpread( QGradient::Spread );

  private:
    const QGradient::Type m_gradientType;

    QGradientStops m_stops;
    QGradient::Spread m_spread = QGradient::PadSpread;
};

inline QGradient::Type QskGradientMaterial::gradientType() const
{
    return m_gradientType;
}

inline void QskGradientMaterial::setStops( const QGradientStops& stops )
{
    m_stops = stops;
}

inline void QskGradientMaterial::setSpread( QGradient::Spread spread )
{
    m_spread = spread;
}

inline const QGradientStops& QskGradientMaterial::stops() const
{
    return m_stops;
}

inline QGradient::Spread QskGradientMaterial::spread() const
{
    return m_spread;
}

#endif
