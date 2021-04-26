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
