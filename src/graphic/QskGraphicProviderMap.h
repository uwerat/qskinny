/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_PROVIDER_MAP_H
#define QSK_GRAPHIC_PROVIDER_MAP_H

#include "QskGlobal.h"
#include <memory>

class QskGraphicProvider;
class QString;

class QSK_EXPORT QskGraphicProviderMap
{
  public:
    QskGraphicProviderMap();
    ~QskGraphicProviderMap();

    void insert( const QString& providerId, QskGraphicProvider* );

    void remove( const QString& providerId );
    void remove( QskGraphicProvider* );

    QskGraphicProvider* take( const QString& providerId );

    QskGraphicProvider* provider( const QString& providerId ) const;

    int size() const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
