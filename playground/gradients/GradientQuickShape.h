/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

class QskGradient;
class QSGNode;
class QQuickWindow;
class QRectF;

namespace GradientQuickShape
{
    QSGNode* updateNode( QQuickWindow*, const QRectF&,
        const QskGradient&, QSGNode* );
}
