/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SLIDER_SKINLET_H
#define SLIDER_SKINLET_H

#include <QskSliderSkinlet.h>

class QSGTransformNode;

class SliderSkinlet : public QskSliderSkinlet
{
    using Inherited = QskSliderSkinlet;

public:
    enum NodeRole
    {
        // we have a scale instead of a groove
        ScaleRole = QskSliderSkinlet::HandleRole + 1,
        DecorationRole
    };

    SliderSkinlet();
    ~SliderSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        QskAspect::Subcontrol ) const override;

protected:
    QSGNode* updateSubNode(
        const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;

private:
    QSGNode* updateScaleNode( const QskSlider*, QSGNode* ) const;
    QSGNode* updateDecorationNode( const QskSlider*, QSGNode* ) const;
    QSGNode* updateHandleNode( const QskSlider*, QSGNode* ) const;

    QRectF fillRect( const QskSlider* ) const;
    QRectF scaleRect( const QskSlider* ) const;
    QRectF decorationRect( const QskSlider* ) const;
    QRectF handleRect( const QskSlider* ) const;
};

#endif
