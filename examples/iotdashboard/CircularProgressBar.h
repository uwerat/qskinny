/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef CIRCULARPROGRESSBAR_H
#define CIRCULARPROGRESSBAR_H

#include <QskGradient.h>

#include <QGradient>
#include <QQuickPaintedItem>

class CircularProgressBar : public QQuickPaintedItem
{
    public:
        CircularProgressBar( const QskGradient& gradient, int progress, QQuickItem* parent = nullptr );

        virtual void paint( QPainter* painter ) override;

        double width() const
        {
            return m_width;
        }

        void setWidth( double width )
        {
            m_width = width;
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
        QGradient m_gradient;
        QColor m_backgroundColor;
        QRadialGradient m_ringGradient;
        double m_width = 20;
        int m_progress;
};

#endif // CIRCULARPROGRESSBAR_H
