/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskSkinlet.h>

class StorageBarSkinlet final : public QskSkinlet
{
    Q_GADGET
    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        Pictures,
        Music,
        Videos,
        Documents,
        Others,
        Free
    };
    Q_INVOKABLE StorageBarSkinlet( QskSkin* skin = nullptr );

  private:
    QRectF subControlRect( const QskSkinnable* skinnable, const QRectF& contentsRect,
        QskAspect::Subcontrol subControl ) const override;
    QSGNode* updateSubNode(
        const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const override;
};
