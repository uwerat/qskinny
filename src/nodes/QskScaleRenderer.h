/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
class QSizeF;

class QSK_EXPORT QskScaleRenderer
{
  public:
    QskScaleRenderer();
    virtual ~QskScaleRenderer();

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setAlignment( Qt::Alignment );
    Qt::Alignment aligment() const;

    void setBoundaries( qreal lowerBound, qreal upperBound );
    void setBoundaries( const QskIntervalF& );
    QskIntervalF boundaries() const;

    void setTickmarks( const QskScaleTickmarks& );
    const QskScaleTickmarks& tickmarks() const;

    void setTickColor( const QColor& );
    QColor tickColor() const;

    void setTickWidth( qreal );
    qreal tickWidth() const;

    void setFont( const QFont& );
    QFont font() const;

    void setTextColors( const QskTextColors& );
    QskTextColors textColors() const;

    void setColorFilter( const QskColorFilter& );
    const QskColorFilter& colorFilter() const;

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
