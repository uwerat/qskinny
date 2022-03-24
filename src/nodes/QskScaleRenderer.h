/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCALE_RENDERER_H
#define QSK_SCALE_RENDERER_H

#include "QskGlobal.h"

#include <qnamespace.h>
#include <qfont.h>
#include <qcolor.h>

#include <memory>

class QskSkinnable;
class QskIntervalF;
class QskScaleTickmarks;
class QskTextColors;
class QskColorFilter;

class QSGNode;
class QVariant;
class QRectF;

class QSK_EXPORT QskScaleRenderer
{
  public:
    QskScaleRenderer();
    ~QskScaleRenderer();

    void setOrientation( Qt::Orientation );

    void setBoundaries( const QskIntervalF& );
    void setTickmarks( const QskScaleTickmarks& );

    void setTickColor( const QColor& );
    void setTickWidth( qreal );

    void setFont( const QFont& );
    void setTextColors( const QskTextColors& );

    void setColorFilter( const QskColorFilter& );

    QSGNode* updateScaleNode( const QskSkinnable*,
        const QRectF& tickmarksRect, const QRectF& labelsRect, QSGNode* );

    virtual QVariant labelAt( qreal pos ) const;
    QSizeF boundingLabelSize() const;

    virtual QSGNode* updateTicksNode(
        const QskSkinnable*, const QRectF&, QSGNode* ) const;

    virtual QSGNode* updateLabelsNode(
        const QskSkinnable*, const QRectF& ticksRect,
        const QRectF& labelsRect, QSGNode* node ) const;

  private:
    Q_DISABLE_COPY( QskScaleRenderer )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
