/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TEXT_NODE_H
#define QSK_TEXT_NODE_H

#include "QskNamespace.h"

#include <qrect.h>
#include <qsgnode.h>

class QskTextOptions;
class QskTextColors;
class QString;
class QFont;
class QQuickItem;

class QSK_EXPORT QskTextNode : public QSGTransformNode
{
  public:
    QskTextNode();
    ~QskTextNode() override;

    void setTextData( const QQuickItem* item,
        const QString& text, const QRectF&, const QFont&,
        const QskTextOptions&, const QskTextColors&,
        Qt::Alignment, Qsk::TextStyle );

  private:
    QskHashValue m_hash;
};

#endif
