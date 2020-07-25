/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SPEEDOMETER_H
#define SPEEDOMETER_H

#include <QskBoundedValueControl.h>

class Speedometer : public QskBoundedValueControl
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, Labels, NeedleHead, Needle )

    Speedometer( QQuickItem* parent = nullptr );

    QVector< QString > labels() const;
    void setLabels( const QVector< QString >& labels );

  private:
    QVector< QString > m_labels;
};

#endif
