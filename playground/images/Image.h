/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "QskGlobal.h"

#ifdef QT_NO_KEYWORDS

// qquickimage_p.h uses emit
#define emit Q_EMIT
#include <private/qquickimage_p.h>
#undef emit

#else
#include <private/qquickimage_p.h>
#endif

#include <memory>

class Image : public QQuickImage
{
    Q_OBJECT

    Q_PROPERTY( bool sourceSizeAdjustment READ sourceSizeAdjustment
        WRITE setSourceSizeAdjustment NOTIFY sourceSizeAdjustmentChanged FINAL )

    Q_PROPERTY( bool deferredUpdates READ deferredUpdates
        WRITE setDeferredUpdates NOTIFY deferredUpdatesChanged FINAL )

    using Inherited = QQuickImage;

public:
    Image( QQuickItem* parent = nullptr );
    virtual ~Image();

    void setSourceSizeAdjustment( bool );
    bool sourceSizeAdjustment() const;

    void setDeferredUpdates( bool );
    bool deferredUpdates() const;

    Q_INVOKABLE virtual bool hasHeightForWidth() const;
    Q_INVOKABLE virtual qreal heightForWidth( qreal width ) const;

    Q_INVOKABLE virtual bool hasWidthForHeight() const;
    Q_INVOKABLE virtual qreal widthForHeight( qreal height ) const;

public Q_SLOTS:
    void show();
    void hide();
    void setVisible( bool );

Q_SIGNALS:
    void sourceSizeAdjustmentChanged();
    void deferredUpdatesChanged();

protected:
    virtual void geometryChanged( const QRectF&, const QRectF& ) override;
    virtual void itemChange( ItemChange, const ItemChangeData& ) override;
    virtual void componentComplete() override;

    virtual QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override;
    virtual void updatePolish() override;

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
