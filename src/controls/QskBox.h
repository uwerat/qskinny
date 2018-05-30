/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_H
#define QSK_BOX_H

#include "QskControl.h"

class QSK_EXPORT QskBox : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel )

    QskBox( QQuickItem* parent = nullptr );
    virtual ~QskBox();

    virtual QRectF layoutRect() const override;
    virtual QSizeF contentsSizeHint() const override;

    virtual qreal heightForWidth( qreal width ) const override;
    virtual qreal widthForHeight( qreal height ) const override;
};

#endif
