/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MATERIAL3_PROGRESSBAR_SKINLET_H
#define QSK_MATERIAL3_PROGRESSBAR_SKINLET_H

#include <QskProgressBarSkinlet.h>

class QskProgressBar;

class QskMaterial3ProgressBarSkinlet : QskProgressBarSkinlet
{
    Q_GADGET

    using Inherited = QskProgressBarSkinlet;

  public:
    enum NodeRole
    {
        StopIndicatorRole = Inherited::RoleCount,
        RoleCount
    };

    Q_INVOKABLE QskMaterial3ProgressBarSkinlet( QskSkin* = nullptr );

  protected:
    QSGNode* updateSubNode( const QskSkinnable*,
        quint8 nodeRole, QSGNode* ) const override;

  private:
    QSGNode* updateStopIndicatorNode( const QskProgressBar*, QSGNode* ) const;
    QSGNode* updateGrooveClipNode( const QskProgressBar*, QSGNode* ) const;
};

#endif
