/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRAPHIC_PROVIDER_MAP_H
#define QSK_GRAPHIC_PROVIDER_MAP_H

#include "QskGlobal.h"
#include <memory>

class QskGraphicProvider;
class QString;

class QskGraphicProviderMap
{
  public:
    QskGraphicProviderMap();
    ~QskGraphicProviderMap();

    void insert( const QString& providerId, QskGraphicProvider* );

    void remove( const QString& providerId );
    void remove( QskGraphicProvider* );

    void clear();

    QskGraphicProvider* take( const QString& providerId );
    QskGraphicProvider* provider( const QString& providerId ) const;

    int size() const;

  private:
    Q_DISABLE_COPY( QskGraphicProviderMap )

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
