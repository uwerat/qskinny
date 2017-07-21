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

class QSGNode;
class QQuickItem;
class QColor;

class QSK_EXPORT QskPlainTextRenderer
{
public:
    QskPlainTextRenderer();
    ~QskPlainTextRenderer();

    void setFont( const QFont& );
    void setOptions( const QskTextOptions& );
    void setAlignment( Qt::Alignment );

    void updateNode(
        const QQuickItem*, const QRectF&, const QString&, QSGNode* parentNode,
        const QColor& textColor, Qsk::TextStyle, const QColor& styleColor );

    void updateNode(
        const QQuickItem*, const QSizeF &size, const QString&, QSGNode* parentNode,
        const QColor& textColor, Qsk::TextStyle, const QColor& styleColor );

    static void updateNodeColor( QSGNode* parentNode,
        const QColor& textColor, Qsk::TextStyle, const QColor& styleColor );

    QSizeF implicitSizeHint( const QString& ) const;

    QRectF textRect( const QSizeF&, const QString& ) const;

private:
    int flags() const;

    QFont m_font;
    QFontMetricsF m_fontMetrics;
    QskTextOptions m_options;
    Qt::Alignment m_alignment;
};

inline void QskPlainTextRenderer::updateNode(
    const QQuickItem* item, const QSizeF& size, const QString& text, QSGNode* parentNode,
    const QColor& textColor, Qsk::TextStyle style, const QColor& styleColor )
{
    updateNode( item, QRectF( QPointF(), size ),
        text, parentNode, textColor, style, styleColor );
}

#endif
