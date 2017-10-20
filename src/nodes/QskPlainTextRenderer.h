/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PLAIN_TEXT_RENDERER_H
#define QSK_PLAIN_TEXT_RENDERER_H

#include "QskGlobal.h"
#include "QskNamespace.h"
#include "QskTextOptions.h"

#include <QFont>
#include <QFontMetricsF>

class QskTextColors;

class QSGNode;
class QQuickItem;
class QColor;
class QSGTransformNode;
class QQuickItem;

class QSK_EXPORT QskPlainTextRenderer
{
public:
    QskPlainTextRenderer();
    ~QskPlainTextRenderer();

    void setFont( const QFont& );
    void setOptions( const QskTextOptions& );
    void setAlignment( Qt::Alignment );

    void updateNode( const QString&, const QSizeF&,
        Qsk::TextStyle, const QskTextColors&,
        const QQuickItem*, QSGTransformNode* );

    void updateNode( const QString&, const QRectF&, 
        Qsk::TextStyle, const QskTextColors&,
        const QQuickItem*, QSGTransformNode* );

    static void updateNodeColor( QSGNode* parentNode,
        const QColor& textColor, Qsk::TextStyle, const QColor& styleColor );

    QSizeF textSize( const QString& ) const;
    QRectF textRect( const QSizeF&, const QString& ) const;

private:
    int flags() const;

    QFont m_font;
    QFontMetricsF m_fontMetrics;
    QskTextOptions m_options;
    Qt::Alignment m_alignment;
};

#endif
