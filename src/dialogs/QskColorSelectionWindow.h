/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_COLOR_SELECTION_WINDODW_H
#define QSK_COLOR_SELECTION_WINDODW_H

#include "QskWindowOrSubWindow.h"

class QskColorPicker;

template< typename W >
class QskColorSelectionWindow : public QskWindowOrSubWindow< W >
{
    using Inherited = QskWindowOrSubWindow< W >;

  public:

    QskColorSelectionWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction );
    ~QskColorSelectionWindow();

    QColor selectedColor() const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
