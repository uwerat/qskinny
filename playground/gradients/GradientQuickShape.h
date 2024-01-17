/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
