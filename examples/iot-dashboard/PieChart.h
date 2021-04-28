/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef PIECHART_H
#define PIECHART_H

#include <QskControl.h>

class PieChart : public QskControl
{
        Q_OBJECT

    public:
        QSK_SUBCONTROLS( Panel, Labels )

        PieChart( QQuickItem* parent = nullptr );

        QVector< float > angles() const;
        void setAngles( const QVector< float >& angles );

        QVector< QString > labels() const;
        void setLabels( const QVector< QString >& labels );

    private:
        QVector< float > m_angles;
        QVector< QString > m_labels;
};

#endif // PIECHART_H
