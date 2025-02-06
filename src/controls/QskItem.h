/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ITEM_H
#define QSK_ITEM_H

#include "QskGlobal.h"
#include <qquickitem.h>

class QskItemPrivate;
class QskGeometryChangeEvent;
class QskViewportChangeEvent;
class QskWindowChangeEvent;

class QSK_EXPORT QskItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY( QRectF geometry READ geometry WRITE setGeometry )
    Q_PROPERTY( QRectF rect READ rect )

    Q_PROPERTY( bool tabFence READ isTabFence
        WRITE setTabFence NOTIFY itemFlagsChanged  )

    Q_PROPERTY( bool polishOnResize READ polishOnResize
        WRITE setPolishOnResize NOTIFY itemFlagsChanged FINAL )

    Q_PROPERTY( bool polishOnParentResize READ polishOnParentResize
        WRITE setPolishOnParentResize NOTIFY itemFlagsChanged FINAL )

#if QT_VERSION < QT_VERSION_CHECK( 6, 7, 0 )
    Q_PROPERTY( Qt::FocusPolicy focusPolicy READ focusPolicy
        WRITE setFocusPolicy NOTIFY focusPolicyChanged )
#endif

    Q_PROPERTY( bool wheelEnabled READ isWheelEnabled
        WRITE setWheelEnabled NOTIFY wheelEnabledChanged )

    Q_PROPERTY( bool visibleToParent READ isVisibleToParent )
    Q_PROPERTY( bool hasChildItems READ hasChildItems )
    Q_PROPERTY( bool initiallyPainted READ isInitiallyPainted )

    Q_PROPERTY( UpdateFlags updateFlags READ updateFlags NOTIFY updateFlagsChanged )

    using Inherited = QQuickItem;

  public:
    enum UpdateFlag : quint16
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

    ~QskItem() override;

    const char* className() const;

    bool isVisibleTo( const QQuickItem* ) const;
    bool isVisibleToParent() const;

    bool hasChildItems() const;

    QRectF rect() const;
    QSizeF implicitSize() const;

    void setGeometry( qreal x, qreal y, qreal width, qreal height );
    void setGeometry( const QPointF&, const QSizeF& );
    QRectF geometry() const;

    using QQuickItem::setPosition;
    using QQuickItem::setSize;

    void setPosition( qreal x, qreal y );
    void setSize( qreal width, qreal height );

    void setPolishOnResize( bool );
    bool polishOnResize() const;

    void setPolishOnParentResize( bool );
    bool polishOnParentResize() const;

#if QT_VERSION < QT_VERSION_CHECK( 6, 7, 0 )
    void setFocusPolicy( Qt::FocusPolicy );
    Qt::FocusPolicy focusPolicy() const;
#endif

    void setTabFence( bool );
    bool isTabFence() const;

    void setWheelEnabled( bool );
    bool isWheelEnabled() const;

    void setLayoutMirroring( bool on, bool childrenInherit = false );
    void resetLayoutMirroring();
    bool layoutMirroring() const;

    void resetUpdateFlags();
    UpdateFlags updateFlags() const;

    void setUpdateFlag( UpdateFlag, bool on = true );
    void resetUpdateFlag( UpdateFlag );
    bool testUpdateFlag( UpdateFlag ) const;

    void classBegin() override;
    void componentComplete() override;
    void releaseResources() override;

    bool isPolishScheduled() const;
    bool isUpdateNodeScheduled() const;
    bool isInitiallyPainted() const;

    bool maybeUnresized() const;

  Q_SIGNALS:
    void wheelEnabledChanged( bool );
#if QT_VERSION < QT_VERSION_CHECK( 6, 7, 0 )
    void focusPolicyChanged( Qt::FocusPolicy );
#endif

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
    QskItem( QskItemPrivate&, QQuickItem* = nullptr );

    bool event( QEvent* ) override;

    virtual void changeEvent( QEvent* );
    virtual void geometryChangeEvent( QskGeometryChangeEvent* );
    virtual void viewportChangeEvent( QskViewportChangeEvent* );
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

    void setActiveFocusOnTab( bool ) = delete; // use setFocusPolicy
    void applyUpdateFlag( UpdateFlag, bool on );

    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override final;
    virtual QSGNode* updateItemPaintNode( QSGNode* );

    void updatePolish() override final;
    virtual void updateItemPolish();

    Q_DECLARE_PRIVATE( QskItem )
};

inline bool QskItem::hasChildItems() const
{
    return !childItems().isEmpty();
}

inline void QskItem::setGeometry( const QPointF& pos, const QSizeF& size )
{
    setGeometry( pos.x(), pos.y(), size.width(), size.height() );
}

inline void QskItem::setGeometry( const QRectF& rect )
{
    setGeometry( rect.x(), rect.y(), rect.width(), rect.height() );
}

inline void QskItem::setPosition( qreal x, qreal y )
{
    QQuickItem::setPosition( QPointF( x, y ) );
}

inline void QskItem::setSize( qreal width, qreal height )
{
    QQuickItem::setSize( QSizeF( width, height ) );
}

inline QSizeF QskItem::implicitSize() const
{
    return QSizeF( implicitWidth(), implicitHeight() );
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskItem::UpdateFlags )
Q_DECLARE_METATYPE( QskItem::UpdateFlags )

#endif
