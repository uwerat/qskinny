/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef LIGHTINTENSITY_H
#define LIGHTINTENSITY_H

#include "Box.h"

#include <QskTextLabel.h>

#include <QQuickPaintedItem>
#include <QRadialGradient>

class DimmerAnimator;
class QskTextLabel;

class LightIntensityValueLabel : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Text )

        LightIntensityValueLabel( const QString& text, QQuickItem* parent ) : QskTextLabel( text, parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskTextLabel::Text )
            {
                return Text;
            }

            return subControl;
        }
};

class LightDimmer: public QQuickPaintedItem
{
        Q_OBJECT

    public:
        LightDimmer( const QskGradient& coldGradient, const QskGradient& warmGradient, QQuickItem* parent );

        double thickness() const
        {
            return m_thickness;
        }

        void setThickness( double thickness )
        {
            m_thickness = thickness;
        }

        QColor backgroundColor() const
        {
            return m_backgroundColor;
        }

        void setBackgroundColor( const QColor& color )
        {
            m_backgroundColor = color;
        }

        QRadialGradient ringGradient() const
        {
            return m_ringGradient;
        }

        void setRingGradient( const QRadialGradient& gradient )
        {
            m_ringGradient = gradient;
        }

    private:
        double m_thickness = 17.57;
        QColor m_backgroundColor;
        QRadialGradient m_ringGradient;
        QskGradient m_coldGradient;
        QskGradient m_warmGradient;

        virtual void paint( QPainter* painter ) override;
};

class LightDisplay : public QskControl
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel, ColdPart, WarmPart )

        LightDisplay( QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

    protected:
        void updateLayout() override;

    private:
        QskTextLabel* m_leftLabel;
        LightIntensityValueLabel* m_centreLabel;
        QskTextLabel* m_rightLabel;
        LightDimmer* m_dimmer;
        DimmerAnimator* m_animator;
};

class LightIntensity : public Box
{
        Q_OBJECT

    public:
        LightIntensity( QQuickItem* parent );
};

#endif // LIGHTINTENSITY_H
