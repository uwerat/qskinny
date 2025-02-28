/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GLYPH_GRAPHIC_PROVIDER_H
#define QSK_GLYPH_GRAPHIC_PROVIDER_H

#include "QskGraphicProvider.h"

class QRawFont;

class QSK_EXPORT QskGlyphGraphicProvider : public QskGraphicProvider
{
    using Inherited = QskGraphicProvider;

  public:
    QskGlyphGraphicProvider( QObject* parent = nullptr );
    ~QskGlyphGraphicProvider() override;

    void setIconFont( const QRawFont& );
    QRawFont iconFont() const;

    QskGraphic glyphGraphic( uint glyphIndex ) const;

  protected:
    const QskGraphic* loadGraphic( const QString& ) const override final;
    virtual uint glyphIndex( const QString& ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
