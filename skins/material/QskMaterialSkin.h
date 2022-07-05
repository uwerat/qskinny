/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_MATERIAL_SKIN_H
#define QSK_MATERIAL_SKIN_H

#include "QskMaterialGlobal.h"
#include <QskSkin.h>
#include <memory>

class QSK_MATERIAL_EXPORT QskMaterialSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskMaterialSkin( QObject* parent = nullptr );
    ~QskMaterialSkin() override;

  private:
    void resetColors( const QColor& accent ) override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
