/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBox.h>

class QskGradient;
class QskShadowMetrics;
class QskBoxShapeMetrics;
class QskBoxBorderColors;

class ShadowedBox : public QskBox
{
    Q_OBJECT

  public:
    ShadowedBox(QQuickItem* parent = nullptr);
    ~ShadowedBox() override;

    void setShape( const QskBoxShapeMetrics& );
    void setGradient( const QskGradient& );

    void setBorderWidth( qreal width );
    void setBorderColors( const QskBoxBorderColors& );

    void setShadow( const QskShadowMetrics& );
    void setShadowColor( const QColor& );
};
