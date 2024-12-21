/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SYMBOL_PROVIDER_H
#define QSK_SYMBOL_PROVIDER_H

#include "QskGraphicProvider.h"

class QFont;
class QRawFont;
class QByteArray;

class QskSymbolProvider : public QskGraphicProvider
{
    using Inherited = QskGraphicProvider;

  public:
    QskSymbolProvider( QObject* parent = nullptr );
    ~QskSymbolProvider() override;

    void setFont( const QFont & );
    void setRawFont( const QRawFont & );
    void setFontData( const QByteArray & );

    QskGraphic* requestSymbol( uint ) const;
    QskGraphic* requestSymbol( QChar ) const;

  protected:
    const QskGraphic* loadGraphic( const QString& ) const override final;
    virtual uint idToUnicode( const QString& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline QskGraphic* QskSymbolProvider::requestSymbol( QChar c ) const
{
    return requestSymbol( c.unicode() );
}

#endif
