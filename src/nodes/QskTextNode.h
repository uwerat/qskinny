/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_TEXT_NODE_H
#define QSK_TEXT_NODE_H

#include "QskGlobal.h"
#include "QskNamespace.h"

#include <QSGTransformNode>
#include <Qt>

class QskTextOptions;
class QString;

class QSK_EXPORT QskTextNode : public QSGTransformNode
{
public:
    QskTextNode();
    virtual ~QskTextNode();

    bool setTextData( const QString& text, const QSizeF&, const QFont&,
        const QskTextOptions&, Qt::Alignment, Qsk::TextStyle,
        const QColor& textColor, const QColor& styleColor, const QColor& linkColor );
private:
    uint m_hash;
};

#endif
