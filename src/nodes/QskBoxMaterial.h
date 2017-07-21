/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_MATERIAL_H
#define QSK_BOX_MATERIAL_H

#include <QskGlobal.h>
#include <QSGMaterial>

class QRectF;
class QSizeF;
class QSGTexture;
class QskBoxOptions;

class QSK_EXPORT QskBoxMaterial : public QSGMaterial
{
public:
    class TextureData;

    QskBoxMaterial();
    virtual ~QskBoxMaterial();

    void setBoxOptions( const QskBoxOptions& );

    QSizeF textureSize() const;
    QRectF textureCoordinates() const;

    virtual QSGMaterialShader* createShader() const override;
    virtual QSGMaterialType* type() const override;
    virtual int compare( const QSGMaterial* ) const override;

    bool isValid() const;

private:
    void releaseTexture();

    TextureData* m_data;
};

#endif
