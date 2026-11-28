/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_WINDOW_OR_SUBWINDOW_H
#define QSK_WINDOW_OR_SUBWINDOW_H

#include "QskDialogSubWindow.h"
#include "QskDialogWindow.h"

template< typename W >
class QskWindowOrSubWindow : public W
{
  public:
    QskWindowOrSubWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction );
};

template<>
class QskWindowOrSubWindow< QskDialogWindow > : public QskDialogWindow
{
  public:
    QskWindowOrSubWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction );

    QskDialog::DialogCode exec();

    void setContentItem( QQuickItem* item );
};

template<>
class QskWindowOrSubWindow< QskDialogSubWindow > : public QskDialogSubWindow
{
  public:
    QskWindowOrSubWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction );

    QskDialog::DialogCode exec();

    void setContentItem( QQuickItem* item );
};

#endif
