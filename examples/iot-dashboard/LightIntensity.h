/****************************************************************************
**
** Copyright 2021 Edelhirsch Software GmbH. All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of the copyright holder nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

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
