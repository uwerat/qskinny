#ifndef LIGHTINTENSITY_H
#define LIGHTINTENSITY_H

#include "Box.h"

#include <QQuickPaintedItem>

class QskTextLabel;

class LightDimmer: public QQuickPaintedItem
{
        Q_OBJECT

    public:
        LightDimmer( QQuickItem* parent );

        double thickness() const
        {
            return m_thickness;
        }

        void setThickness( double thickness )
        {
            m_thickness = thickness;
        }

    private:
        double m_thickness = 17.57;

        virtual void paint( QPainter* painter ) override;
};

class LightDisplay : public QskControl
{
        Q_OBJECT

    public:
        LightDisplay( QQuickItem* parent );

    protected:
        void updateLayout() override;

    private:
        QskTextLabel* m_leftLabel;
        QskTextLabel* m_centreLabel;
        QskTextLabel* m_rightLabel;
        LightDimmer* m_dimmer;
};

class LightIntensity : public Box
{
        Q_OBJECT

    public:
        LightIntensity( QQuickItem* parent );
};

#endif // LIGHTINTENSITY_H
