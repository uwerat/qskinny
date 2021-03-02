/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_QUICK_ITEM_H
#define QSK_QUICK_ITEM_H

#include "QskGlobal.h"
#include <qquickitem.h>

class QskQuickItemPrivate;
class QskGeometryChangeEvent;
class QskWindowChangeEvent;

class QSK_EXPORT QskQuickItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY( QRectF geometry READ geometry WRITE setGeometry )
    Q_PROPERTY( QRectF rect READ rect )

    Q_PROPERTY( bool transparentForPositioners READ isTransparentForPositioner
        WRITE setTransparentForPositioner NOTIFY itemFlagsChanged )

    Q_PROPERTY( bool tabFence READ isTabFence
        WRITE setTabFence NOTIFY itemFlagsChanged  )

    Q_PROPERTY( bool polishOnResize READ polishOnResize
        WRITE setPolishOnResize NOTIFY itemFlagsChanged FINAL )

    Q_PROPERTY( bool visibleToParent READ isVisibleToParent )
    Q_PROPERTY( bool hasChildItems READ hasChildItems )
    Q_PROPERTY( bool initiallyPainted READ isInitiallyPainted )

    Q_PROPERTY( UpdateFlags updateFlags READ updateFlags NOTIFY updateFlagsChanged )

    using Inherited = QQuickItem;

  public:
    enum UpdateFlag
    {
        DeferredUpdate          =  1 << 0,
        DeferredPolish          =  1 << 1,
        DeferredLayout          =  1 << 2,
        CleanupOnVisibility     =  1 << 3,

        PreferRasterForTextures =  1 << 4,

        DebugForceBackground    =  1 << 7
    };

    Q_ENUM( UpdateFlag )
    Q_DECLARE_FLAGS( UpdateFlags, UpdateFlag )

    ~QskQuickItem() override;

    const char* className() const;

    bool isVisibleTo( const QQuickItem* ) const;
    bool isVisibleToParent() const;

    bool hasChildItems() const;

    QRectF rect() const;
#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )
    QSizeF size() const;
#endif
    QSizeF implicitSize() const;

    void setGeometry( qreal x, qreal y, qreal width, qreal height );
    QRectF geometry() const;

    void setPolishOnResize( bool );
    bool polishOnResize() const;

    void setTransparentForPositioner( bool );
    bool isTransparentForPositioner() const;

    void setTabFence( bool );
    bool isTabFence() const;

    void setLayoutMirroring( bool on, bool childrenInherit = false );
    void resetLayoutMirroring();
    bool layoutMirroring() const;

    void resetUpdateFlags();
    UpdateFlags updateFlags() const;

    Q_INVOKABLE void setUpdateFlag( UpdateFlag, bool on = true );
    Q_INVOKABLE void resetUpdateFlag( UpdateFlag );
    Q_INVOKABLE bool testUpdateFlag( UpdateFlag ) const;

    void classBegin() override;
    void componentComplete() override;
    void releaseResources() override;

    bool isPolishScheduled() const;
    bool isUpdateNodeScheduled() const;
    bool isInitiallyPainted() const;

    bool maybeUnresized() const;

  Q_SIGNALS:
    void itemFlagsChanged();
    void updateFlagsChanged( UpdateFlags );

  public Q_SLOTS:
    void setGeometry( const QRectF& );

    void show();
    void hide();

    void setHidden( bool );
    void setDisabled( bool );

    void resetImplicitSize();

#ifdef Q_MOC_RUN
    // methods from QQuickItem, we want to be available as string based slots
    void setVisible( bool );
    void setEnabled( bool );
#endif

  protected:
    QskQuickItem( QskQuickItemPrivate&, QQuickItem* = nullptr );

    bool event( QEvent* ) override;

    virtual void changeEvent( QEvent* );
    virtual void geometryChangeEvent( QskGeometryChangeEvent* );
    virtual void windowChangeEvent( QskWindowChangeEvent* );

    void mouseUngrabEvent() override; 
    void touchUngrabEvent() override;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    void windowDeactivateEvent() override;
#endif

    void itemChange( ItemChange, const ItemChangeData& ) override;
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    void geometryChange( const QRectF&, const QRectF& ) override;
#else
    // using geometryChange also for Qt5
    void geometryChanged( const QRectF&, const QRectF& ) override final;
    virtual void geometryChange( const QRectF&, const QRectF& );
#endif

    virtual void aboutToShow();  // called in updatePolish

  private:
    // don't use boundingRect - it seems to be deprecated
    QRectF boundingRect() const override final { return rect(); }

    /*
         childrenRect()/childrenRectChanged does not make much sense
         in a system, where the parent is responsible for laying out
         its children.
     */
    void childrenRect() = delete;

    void applyUpdateFlag( UpdateFlag, bool on );
    void sendEnabledChangeEvent();

    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override final;
    virtual QSGNode* updateItemPaintNode( QSGNode* );

    void updatePolish() override final;
    virtual void updateItemPolish();

    Q_DECLARE_PRIVATE( QskQuickItem )
};

inline bool QskQuickItem::hasChildItems() const
{
    return !childItems().isEmpty();
}

inline void QskQuickItem::setGeometry( const QRectF& rect )
{
    setGeometry( rect.x(), rect.y(), rect.width(), rect.height() );
}

#if QT_VERSION < QT_VERSION_CHECK( 5, 10, 0 )

inline QSizeF QskQuickItem::size() const
{
    return QSizeF( width(), height() );
}

#endif

inline QSizeF QskQuickItem::implicitSize() const
{
    return QSizeF( implicitWidth(), implicitHeight() );
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskQuickItem::UpdateFlags )
Q_DECLARE_METATYPE( QskQuickItem::UpdateFlags )

#endif
