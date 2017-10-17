/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_RENDERER_H
#define QSK_SKIN_RENDERER_H

#include "QskGlobal.h"
#include "QskAspect.h"

#include <Qt>

class QskSkinnable;
class QskTextOptions;
class QskBoxOptions;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskTextNode;

class QPointF;
class QRectF;
class QSizeF;
class QMarginsF;
class QString;

namespace QskSkinRenderer
{
    // a loose collection of functions, waiting to be
    // organized somehow. TODO ...

    QSK_EXPORT void updateText( const QskSkinnable*,
        const QRectF&, Qt::Alignment,
        const QString&, const QskTextOptions&,
        QskTextNode*, QskAspect::Subcontrol );

    QSK_EXPORT void updateTextAt( const QskSkinnable*,
        const QPointF&, const QString&, const QskTextOptions&,
        QskTextNode*, QskAspect::Subcontrol );

    QSK_EXPORT QSizeF textSize( const QskSkinnable*,
        const QString&, const QskTextOptions&, QskAspect::Subcontrol );

    QSK_EXPORT QSizeF textSize( const QskSkinnable*,
        const QSizeF& boundingSize, const QString&,
        const QskTextOptions&, QskAspect::Subcontrol );

    QSK_EXPORT QskBoxOptions boxOptions( const QskSkinnable*,
        const QSizeF&, QskAspect::Subcontrol );

    QSK_EXPORT QMarginsF paddingHint(
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, const QSizeF&, bool inner );
}

#endif
