/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef BOX_H
#define BOX_H

#include <QskLinearBox.h>

class QskTextLabel;

class Box : public QskLinearBox
{
        Q_OBJECT
    public:
        QSK_SUBCONTROLS( Panel )

        Box( const QString& title, QQuickItem* parent );

        QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override;

    private:
        QString m_title;
        QskTextLabel* m_label;
        QskControl* m_content;
};

#endif // BOX_H
