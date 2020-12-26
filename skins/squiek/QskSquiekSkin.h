/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
