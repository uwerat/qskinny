/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef CUSTOM_SLIDER_SKINLET_H
#define CUSTOM_SLIDER_SKINLET_H

#include <QskSliderSkinlet.h>

class QSGTransformNode;

class CustomSliderSkinlet : public QskSliderSkinlet
{
    using Inherited = QskSliderSkinlet;

  public:
    enum NodeRole
    {
        // we have a scale instead of a groove
        ScaleRole = QskSliderSkinlet::HandleRole + 1,
        DecorationRole
    };

    CustomSliderSkinlet();

    QRectF subControlRect( const QskSkinnable*,
        const QRectF&, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateScaleNode( const QskSlider*, QSGNode* ) const;
    QSGNode* updateDecorationNode( const QskSlider*, QSGNode* ) const;
    QSGNode* updateHandleNode( const QskSlider*, QSGNode* ) const;

    QRectF scaleRect( const QRectF& ) const;
    QRectF fillRect( const QskSlider*, const QRectF& ) const;
    QRectF decorationRect( const QskSlider*, const QRectF& ) const;
    QRectF handleRect( const QskSlider*, const QRectF& ) const;
};

#endif
