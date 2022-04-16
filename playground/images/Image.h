/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "QskGlobal.h"

QSK_QT_PRIVATE_BEGIN

#ifndef emit
    #define emit
    #include <private/qquickimage_p.h>
    #undef emit
#else
    #include <private/qquickimage_p.h>
#endif

QSK_QT_PRIVATE_END

class ImagePrivate;

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
    ~Image() override;

    void setSourceSizeAdjustment( bool );
    bool sourceSizeAdjustment() const;

    void setDeferredUpdates( bool );
    bool deferredUpdates() const;

    Q_INVOKABLE virtual bool hasHeightForWidth() const;
    Q_INVOKABLE virtual qreal heightForWidth( qreal width ) const;

    Q_INVOKABLE virtual bool hasWidthForHeight() const;
    Q_INVOKABLE virtual qreal widthForHeight( qreal height ) const;

    void setSourceSize( const QSize& ) override;

  public Q_SLOTS:
    void show();
    void hide();

#ifdef Q_MOC_RUN
    // methods from QQuickItem, we want to be available as string based slots
    void setVisible( bool );
    void setEnabled( bool );
#endif

  Q_SIGNALS:
    void sourceSizeAdjustmentChanged();
    void deferredUpdatesChanged();

  protected:
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    void geometryChange( const QRectF&, const QRectF& ) override;
#else
    void geometryChanged( const QRectF&, const QRectF& ) override;
#endif
    void itemChange( ItemChange, const ItemChangeData& ) override;
    void componentComplete() override;

    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override;
    void updatePolish() override;

  private:
    void adjustSourceSize( const QSizeF& );

    Q_DECLARE_PRIVATE( Image )
};
