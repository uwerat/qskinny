/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_INDICATOR_SKINLET_H
#define QSK_PROGRESS_INDICATOR_SKINLET_H

#include "QskSkinlet.h"

class QskProgressIndicator;

class QSK_EXPORT QskProgressIndicatorSkinlet : public QskSkinlet
{
    using Inherited = QskSkinlet;

  public:
    enum NodeRole : quint8
    {
        GrooveRole,
        FillRole,

        RoleCount
    };

    ~QskProgressIndicatorSkinlet() override;

  protected:
    QskProgressIndicatorSkinlet( QskSkin* = nullptr );

    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

    virtual QSGNode* updateGrooveNode( const QskProgressIndicator*, QSGNode* ) const = 0;
    virtual QSGNode* updateFillNode( const QskProgressIndicator*, QSGNode* ) const = 0;
};

#endif
