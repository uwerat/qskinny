/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef ROUNDEDICON_H
#define ROUNDEDICON_H

#include <QskBox.h>
#include <QskGradient.h>

class QskGraphicLabel;

class RoundedIcon : public QskBox
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel, Icon )
        QSK_STATES( Bright, Small ) // to differentiate between orange and purple and small vs. big

        RoundedIcon( const QString& iconName, bool isBright, bool isSmall, QQuickItem* parent = nullptr );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if( subControl == QskBox::Panel )
            {
                return Panel;
            }

            return subControl;
        }

    protected:
        void updateLayout() override;
        virtual QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    private:
        QString m_iconName;
        QskGraphicLabel* m_graphicLabel = nullptr;
};

#endif // ROUNDEDICON_H
