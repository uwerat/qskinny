/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class QSK_EXPORT QskSpinBoxSkinlet : public QskSkinlet
{
  Q_GADGET
  using Inherited = QskSkinlet;
public:
  enum NodeRole
  {
    IncPanel, IncText, DecPanel, DecText, TextPanel, TextText, RoleCount
  };
  Q_INVOKABLE QskSpinBoxSkinlet( QskSkin* = nullptr );
protected:
  int sampleCount( const QskSkinnable*, QskAspect::Subcontrol ) const override;
  QRectF sampleRect( const QskSkinnable*, const QRectF&, QskAspect::Subcontrol, int index ) const override;
  QskAspect::States sampleStates(const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index ) const override;
  QSizeF sizeHint( const QskSkinnable*, Qt::SizeHint, const QSizeF& ) const override;
  QRectF subControlRect( const QskSkinnable*, const QRectF&, QskAspect::Subcontrol ) const override;
  QSGNode* updateSubNode( const QskSkinnable*, quint8 nodeRole, QSGNode* ) const override;
  QSGNode* updateSampleNode( const QskSkinnable* skinnable, QskAspect::Subcontrol subControl, int index, QSGNode* node ) const override;
};
