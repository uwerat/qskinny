/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <QPen>
#include <QPainterPath>
#include <QGradient>

class ShapeItem : public QskControl
{
    Q_OBJECT

  public:
    ShapeItem( QQuickItem* parent = nullptr );
    ~ShapeItem() override;

    void setPen( const QPen& );
    QPen pen() const;

    void setGradient( const QColor&, const QColor& );
    void setGradient( QGradient::Preset );

    void setPath( const QPainterPath& );
    QPainterPath path() const;

  protected:
    void updateNode( QSGNode* ) override;

  private:
    QPainterPath scaledPath( const QRectF& ) const;

    QPen m_pen;
    QColor m_fillColor[2];
    QPainterPath m_path;
};
