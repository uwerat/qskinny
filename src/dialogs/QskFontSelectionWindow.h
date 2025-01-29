/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FONT_SELECTION_WINDODW_H
#define QSK_FONT_SELECTION_WINDODW_H

#include "QskWindowOrSubWindow.h"

template< typename W >
class QskFontSelectionWindow : public QskWindowOrSubWindow< W >
{
    using Inherited = QskWindowOrSubWindow< W >;

  public:

    QskFontSelectionWindow( QObject* parent, const QString& title,
        QskDialog::Actions actions, QskDialog::Action defaultAction );
    ~QskFontSelectionWindow();

    QFont selectedFont() const;

  private:
    void setupControls( QQuickItem* );
    void loadFontInfo();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
