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

#ifndef MAINCONTENT_H
#define MAINCONTENT_H

#include <QskGridBox.h>
#include <QskLinearBox.h>

class ShadowedRectangle;

class ShadowPositioner : public QskControl
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        ShadowPositioner( QQuickItem* parent );

        void updateLayout() override;
        void setGridBox( QskGridBox* gridBox );

    private:
        QskGridBox* m_gridBox;
        QVector<ShadowedRectangle*> m_rectangles;
};

class MainContentGridBox : public QskGridBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        MainContentGridBox( QQuickItem* parent = nullptr )
            : QskGridBox( parent )
        {
        }

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final
        {
            if( subControl == QskGridBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }
};

class MainContent : public QskLinearBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel )

        MainContent( QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override final;

    protected:
        void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    private:
        QList< QskLinearBox* > m_columns;
        ShadowPositioner* m_shadowPositioner;
};

#endif // MAINCONTENT_H
