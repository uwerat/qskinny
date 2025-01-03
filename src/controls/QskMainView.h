/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_MAIN_VIEW_H
#define QSK_MAIN_VIEW_H

#include "QskLinearBox.h"

class QSK_EXPORT QskMainView : public QskLinearBox
{
    Q_OBJECT

    using Inherited = QskLinearBox;

  public:
    QskMainView( QQuickItem* parent = nullptr );
    ~QskMainView() override;

    QskControl* header() const;
    void setHeader( QskControl* );

    QskControl* body() const;
    void setBody( QskControl* );

    QskControl* footer() const;
    void setFooter( QskControl* );

  protected:
    void focusInEvent( QFocusEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
