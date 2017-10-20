/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_RENDERER_H
#define QSK_TEXT_RENDERER_H

#include "QskGlobal.h"
#include "QskNamespace.h"
#include "QskTextOptions.h"

#include <QFont>
#include <QRectF>

class QskTextHelperItem;
class QskTextColors;

class QQuickItem;
class QQuickWindow;
class QSGTransformNode;
class QColor;

// Hacking a QQuickTextNode using a QQuickText helper item
// How to create the nodes in a better way needs further
// investigations

class QSK_EXPORT QskTextRenderer
{
public:
    void setFont( const QFont& font );
    void setOptions( const QskTextOptions& );
    void setAlignment( Qt::Alignment );

    void updateNode( const QString&, const QSizeF&,
        Qsk::TextStyle, const QskTextColors&,
        const QQuickItem*, QSGTransformNode* );

    void updateNode( const QString&, const QRectF&, 
        Qsk::TextStyle, const QskTextColors&,
        const QQuickItem*, QSGTransformNode* );

    QSizeF textSize( const QString& ) const;
    QRectF textRect( const QSizeF&, const QString& ) const;

private:
    void setupItem( QskTextHelperItem* ) const;

    QFont m_font;
    QskTextOptions m_options;
    Qt::Alignment m_alignment;
};

#endif
