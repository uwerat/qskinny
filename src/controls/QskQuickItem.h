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

    Q_PROPERTY( bool transparentForPositioners READ isTransparentForPositioner
        WRITE setTransparentForPositioner NOTIFY itemFlagsChanged )

    Q_PROPERTY( bool tabFence READ isTabFence
        WRITE setTabFence NOTIFY itemFlagsChanged  )

    Q_PROPERTY( bool polishOnResize READ polishOnResize
        WRITE setPolishOnResize NOTIFY itemFlagsChanged FINAL )

    using Inherited = QQuickItem;

  public:
    enum Flag
    {
        DeferredUpdate          =  1 << 0,
        DeferredPolish          =  1 << 1,
        DeferredLayout          =  1 << 2,
        CleanupOnVisibility     =  1 << 3,

        PreferRasterForTextures =  1 << 4,

        DebugForceBackground    =  1 << 7,

        LastFlag = DebugForceBackground
    };

    Q_ENUM( Flag )
    Q_DECLARE_FLAGS( Flags, Flag )

    ~QskQuickItem() override;

    const char* className() const;

    bool isVisibleTo( const QQuickItem* ) const;
    bool isVisibleToParent() const;

    QRectF geometry() const;
    QRectF rect() const;
    QSizeF size() const;
    QSizeF implicitSize() const;

    void setGeometry( qreal x, qreal y, qreal width, qreal height );

    void setPolishOnResize( bool );
    bool polishOnResize() const;

    void setTransparentForPositioner( bool );
    bool isTransparentForPositioner() const;

    void setTabFence( bool );
    bool isTabFence() const;

    void setLayoutMirroring( bool on, bool recursive = false );
    void resetLayoutMirroring();
    bool layoutMirroring() const;

    void setControlFlags( Flags );
    void resetControlFlags();
    Flags controlFlags() const;

    Q_INVOKABLE void setControlFlag( Flag, bool on = true );
    Q_INVOKABLE void resetControlFlag( Flag );
    Q_INVOKABLE bool testControlFlag( Flag ) const;

    void classBegin() override;
    void componentComplete() override;
    void releaseResources() override;

    bool isPolishScheduled() const;
    bool isUpdateNodeScheduled() const;
    bool isInitiallyPainted() const;

    bool maybeUnresized() const;

  Q_SIGNALS:
    void itemFlagsChanged();
    void controlFlagsChanged();

  public Q_SLOTS:
    void setGeometry( const QRectF& );

    void show();
    void hide();
    void setVisible( bool );

    void resetImplicitSize();

  protected:
    QskQuickItem( QskQuickItemPrivate&, QQuickItem* = nullptr );

    bool event( QEvent* ) override;

    virtual void changeEvent( QEvent* );
    virtual void geometryChangeEvent( QskGeometryChangeEvent* );
    virtual void windowChangeEvent( QskWindowChangeEvent* );

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void geometryChanged( const QRectF&, const QRectF& ) override;

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

    void updateControlFlag( uint flag, bool on );
    void sendEnabledChangeEvent();

    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override final;
    virtual QSGNode* updateItemPaintNode( QSGNode* );

    void updatePolish() override final;
    virtual void updateItemPolish();

    Q_DECLARE_PRIVATE( QskQuickItem )
};

inline void QskQuickItem::setGeometry( const QRectF& rect )
{
    setGeometry( rect.x(), rect.y(), rect.width(), rect.height() );
}

inline QSizeF QskQuickItem::size() const
{
    return QSizeF( width(), height() );
}

inline QSizeF QskQuickItem::implicitSize() const
{
    return QSizeF( implicitWidth(), implicitHeight() );
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskQuickItem::Flags )
Q_DECLARE_METATYPE( QskQuickItem::Flags )

#endif
