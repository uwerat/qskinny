/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SQUIEK_SKIN_H
#define QSK_SQUIEK_SKIN_H

#include "QskSquiekGlobal.h"
#include <QskSkin.h>
#include <memory>

class QSK_SQUIEK_EXPORT QskSquiekSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskSquiekSkin( QObject* parent = nullptr );
    ~QskSquiekSkin() override;

  private:
    void resetColors( const QColor& accent ) override;
    void addGraphicRole( int role, const QColor& );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
