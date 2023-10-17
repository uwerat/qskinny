/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SLIDE_IN_NODE_H
#define QSK_SLIDE_IN_NODE_H

#include "QskGlobal.h"

#include <qsgnode.h>
#include <qnamespace.h>

class QskSlideInNodePrivate;

class QSK_EXPORT QskSlideInNode : public QSGNode
{
  public:
    QskSlideInNode();
    ~QskSlideInNode() override;

    void updateTranslation( const QRectF&, Qt::Edge, qreal progress );

    void setContentsNode( QSGNode* );

    QSGNode* contentsNode();
    const QSGNode* contentsNode() const;

  private:
    Q_DECLARE_PRIVATE( QskSlideInNode )
};

#endif
