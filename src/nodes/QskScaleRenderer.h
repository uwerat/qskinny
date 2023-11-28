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
class QskTickmarks;
class QskTextColors;
class QskColorFilter;

class QSGNode;
class QVariant;
class QRectF;
class QPointF;
class QSizeF;
class QTransform;

class QSK_EXPORT QskScaleRenderer
{
    Q_GADGET

  public:
    enum Flag
    {
        Backbone          = 1 << 0,
        CenteredTickmarks = 1 << 1,
        ClampedLabels     = 1 << 2
    };

    Q_ENUM( Flag )
    Q_DECLARE_FLAGS( Flags, Flag )

    QskScaleRenderer();
    virtual ~QskScaleRenderer();

    void setEdge( Qt::Edge );
    Qt::Edge edge() const;

    void setFlags( Flags );
    Flags flags() const;

    void setFlag( Flag, bool );

    // scale coordinates
    void setBoundaries( qreal lowerBound, qreal upperBound );
    void setBoundaries( const QskIntervalF& );
    QskIntervalF boundaries() const;

    // item coordiates
    qreal position() const;
    void setPosition( qreal );

    void setRange( qreal from, qreal to );
    void setRange( const QskIntervalF& );
    QskIntervalF range() const;

    void setTickmarks( const QskTickmarks& );
    const QskTickmarks& tickmarks() const;

    void setSpacing( qreal );
    qreal spacing() const;

    void setTickColor( const QColor& );
    QColor tickColor() const;

    void setTickLength( qreal );
    qreal tickLength() const;

    void setTickWidth( qreal );
    qreal tickWidth() const;

    void setFont( const QFont& );
    QFont font() const;

    void setTextColors( const QskTextColors& );
    QskTextColors textColors() const;

    void setColorFilter( const QskColorFilter& );
    const QskColorFilter& colorFilter() const;

    QSGNode* updateNode( const QskSkinnable*, QSGNode* );

    virtual QVariant labelAt( qreal pos ) const;
    QSizeF boundingLabelSize() const;

  protected:
    virtual QSGNode* updateTicksNode( const QTransform&, QSGNode* ) const;

    virtual QSGNode* updateLabelsNode(
        const QskSkinnable*, const QTransform&, QSGNode* ) const;

  private:
    Q_DISABLE_COPY( QskScaleRenderer )

    QRectF labelRect( const QTransform&, qreal, const QSizeF& ) const;

    QSGNode* updateTickLabelNode( const QskSkinnable*,
        QSGNode*, const QVariant&, const QRectF& ) const;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QskScaleRenderer::Flags )

#endif
