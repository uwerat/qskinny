/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

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
