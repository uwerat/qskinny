/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>
#include <QPen>
#include <QPainterPath>

class ShapeItem : public QskControl
{
    Q_OBJECT

  public:
    ShapeItem( QQuickItem* parent = nullptr );
    ~ShapeItem() override;

    void setPen( const QPen& );
    QPen pen() const;

    void setFillColor( const QColor& );
    QColor fillColor() const;

    void setPath( const QPainterPath& );
    QPainterPath path() const;

  protected:
    void updateNode( QSGNode* ) override;

  private:
    QPen m_pen;
    QColor m_fillColor;
    QPainterPath m_path;
};
