/**********************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ARC_NODE_H
#define QSK_ARC_NODE_H

#include "QskGradient.h"
#include "QskPaintedNode.h"

#include <QGradient>

class QSK_EXPORT QskArcNode : public QskPaintedNode
{
  public:
    QskArcNode();
    ~QskArcNode() override;

    void setArcData( const QskGradient&, QGradient::Type, double, double,
        double, double, bool, double );

    void paint( QPainter* painter, const QSizeF& size ) override;
    uint hash() const override;

  private:
    QskGradient m_gradient;
    QGradient::Type m_gradientType;
    double m_width;
    double m_value;
    double m_origin;
    double m_maximum;
    bool m_isIndeterminate;
    double m_position;
};

#endif
