/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef FRAME_H
#define FRAME_H

#include "QskControl.h"

class QskBoxNode;

class Frame : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( Style style READ style WRITE setStyle NOTIFY styleChanged FINAL )

    Q_PROPERTY( qreal frameWidth READ frameWidth
        WRITE setFrameWidth NOTIFY frameWidthChanged FINAL )

    Q_PROPERTY( qreal radius READ radius
        WRITE setRadius NOTIFY radiusChanged )

    Q_PROPERTY( QColor color READ color
        WRITE setColor NOTIFY colorChanged )

    using Inherited = QskControl;

  public:
    enum Style
    {
        Plain,
        Raised,
        Sunken
    };

    Q_ENUM( Style )

    Frame( QQuickItem* parent = nullptr );
    ~Frame() override;

    void setStyle( Style );
    Style style() const;

    void setFrameWidth( qreal );
    qreal frameWidth() const;

    // Qt::RelativeSize: a percentage in the range [0-100]
    void setRadius( qreal );
    qreal radius() const;

    void setColor( const QColor& );
    QColor color() const;

  Q_SIGNALS:
    void styleChanged();
    void frameWidthChanged();
    void radiusChanged();
    void colorChanged();

  protected:
    void updateNode( QSGNode* ) override;

  private:
    void updateFrameNode( const QRectF&, QskBoxNode* );

    Style m_style;
    QColor m_color;
    qreal m_frameWidth;
    qreal m_radius;
};

#endif
