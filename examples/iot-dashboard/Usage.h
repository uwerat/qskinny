/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef USAGE_H
#define USAGE_H

#include "Box.h"

#include <QskLinearBox.h>
#include <QskTextLabel.h>

class UsageSpacer : public QskTextLabel
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Text )

        UsageSpacer( QQuickItem* parent = nullptr )
            : QskTextLabel( "_____", parent )
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

class Usage : public Box
{
    public:
        Usage( QQuickItem* parent );
};

#endif // USAGE_H
