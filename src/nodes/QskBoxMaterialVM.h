/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_MATERIAL_VM_H
#define QSK_BOX_MATERIAL_VM_H

#include <QskGlobal.h>
#include <QSGMaterial>

class QRectF;
class QSizeF;
class QSGTexture;
class QskBoxOptions;

template class QList< QSGTexture* >;

// as long as QskBoxMaterial does not work for certain VMs
// we need to keep this implementation

class QSK_EXPORT QskBoxMaterialVM : public QSGMaterial
{
public:
    QskBoxMaterialVM();
    virtual ~QskBoxMaterialVM();

    void setBoxOptions( const QskBoxOptions&, uint key );

    QSizeF textureSize() const;
    QRectF textureCoordinates() const;

    virtual QSGMaterialShader* createShader() const override;
    virtual QSGMaterialType* type() const override;
    virtual int compare( const QSGMaterial* ) const override;

    bool isValid() const;

    void release( uint hash );

private:
    friend class QskBoxMaterialShaderVM;

    void releaseTexture( QSGTexture* );
    void releaseTextures();

    QList< QSGTexture* > m_textures;
};

#endif
