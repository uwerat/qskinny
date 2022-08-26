/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_ELEMENT_H
#define QSK_LAYOUT_ELEMENT_H

#include "QskGlobal.h"
#include <qnamespace.h>
#include <qrect.h>

class QskSizePolicy;
class QskLayoutMetrics;

class QskLayoutElement
{
  public:
    QskLayoutElement();
    virtual ~QskLayoutElement();

    QskLayoutMetrics metrics( Qt::Orientation, qreal constraint ) const;

    virtual QskSizePolicy sizePolicy() const = 0;
    virtual Qt::Alignment alignment() const = 0;

    QSizeF constrainedSize( const QSizeF& ) const;

    QSizeF sizeConstraint( Qt::SizeHint, const QSizeF& constraint ) const;

    virtual QSizeF sizeHint( Qt::SizeHint,
        const QSizeF& constraint = QSizeF() ) const = 0;

    qreal heightForWidth( qreal ) const;
    qreal widthForHeight( qreal ) const;

  private:
    Q_DISABLE_COPY( QskLayoutElement )

    qreal metric( Qt::Orientation, Qt::SizeHint, qreal constraint ) const;

    qreal boundedSize( Qt::Orientation, qreal ) const;
};

inline qreal QskLayoutElement::heightForWidth( qreal width ) const
{
    return sizeHint( Qt::PreferredSize, QSizeF( width, -1.0 ) ).height();
}

inline qreal QskLayoutElement::widthForHeight( qreal height ) const
{
    return sizeHint( Qt::PreferredSize, QSizeF( -1.0, height ) ).width();
}

class QQuickItem;

class QskItemLayoutElement final : public QskLayoutElement
{
  public:
    QskItemLayoutElement( const QQuickItem* );

    QskSizePolicy sizePolicy() const override;
    Qt::Alignment alignment() const override;

  private:
    QSizeF sizeHint( Qt::SizeHint, const QSizeF& ) const override;

    const QQuickItem* m_item = nullptr;
};

#endif
