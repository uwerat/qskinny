/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SEPARATOR_SKINLET_H
#define QSK_SEPARATOR_SKINLET_H

#include "QskSkinlet.h"

class QskSeparator;

class QSK_EXPORT QskSeparatorSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

  public:
    enum NodeRole
    {
        PanelRole,
    };

    Q_INVOKABLE QskSeparatorSkinlet( QskSkin* = nullptr );
    ~QskSeparatorSkinlet() override;

    QRectF subControlRect( const QskSkinnable*,
        const QRectF& rect, QskAspect::Subcontrol ) const override;

    QSizeF sizeHint( const QskSkinnable*,
        Qt::SizeHint, const QSizeF& ) const override;

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QRectF panelRect( const QskSeparator*, const QRectF& rect ) const;
};

#endif
