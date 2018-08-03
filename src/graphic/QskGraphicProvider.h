/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRAPHIC_PROVIDER_H
#define QSK_GRAPHIC_PROVIDER_H

#include "QskGlobal.h"

#include <qobject.h>
#include <memory>

class QskGraphic;
class QUrl;

class QSK_EXPORT QskGraphicProvider : public QObject
{
  public:
    QskGraphicProvider( QObject* parent = nullptr );
    ~QskGraphicProvider() override;

    void setCacheSize( int );
    int cacheSize() const;

    void clearCache();

    const QskGraphic* requestGraphic( const QString& id ) const;

  protected:
    virtual const QskGraphic* loadGraphic( const QString& id ) const = 0;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

namespace Qsk
{
    QSK_EXPORT void addGraphicProvider( const QString& providerId, QskGraphicProvider* );
    QSK_EXPORT QskGraphicProvider* graphicProvider( const QString& providerId );

    QSK_EXPORT QskGraphic loadGraphic( const QUrl& url );
    QSK_EXPORT QskGraphic loadGraphic( const char* source );
}

#endif
