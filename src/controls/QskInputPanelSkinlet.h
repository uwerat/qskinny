/******************************************************************************
 * Qsk Controls
 * Copyright (C) 2016 Uwe Rathmann
 *
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef QSK_INPUT_PANEL_SKINLET_H
#define QSK_INPUT_PANEL_SKINLET_H

#include "QskSkinlet.h"

class QskInputPanel;

class QSK_EXPORT QskInputPanelSkinlet : public QskSkinlet
{
    Q_GADGET

    using Inherited = QskSkinlet;

    enum NodeRole
    {
        Panel0,
        Panel1,
        Panel2
    };

public:
    Q_INVOKABLE QskInputPanelSkinlet( QskSkin* = nullptr );
    virtual ~QskInputPanelSkinlet();

protected:
    virtual QSGNode* updateSubNode( const QskSkinnable*,
        quint8, QSGNode* ) const override;

    virtual QSGNode* updatePanelNode( const QskInputPanel*, QSGNode* ) const;

    virtual QSGNode* updateKeyFrameNode( const QskInputPanel*,
        QSGNode*, const QRectF&, Qt::Key ) const;

    virtual QSGNode* updateKeyGlyphNode( const QskInputPanel*,
        QSGNode*, const QRectF&, Qt::Key ) const;
};

#endif 
