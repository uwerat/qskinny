/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CONTROL_H
#define QSK_CONTROL_H 1

#include "QskGlobal.h"
#include "QskResizable.h"
#include "QskSkinnable.h"
#include "QskGradient.h"
#include "QskAspect.h"

#include <QQuickItem>
#include <QLocale>

#include <memory>

class QskControlPrivate;

class QskGeometryChangeEvent;
class QskWindowChangeEvent;
class QskGestureEvent;

template class QVector< QskAspect::Aspect >;

class QSK_EXPORT QskControl : public QQuickItem, public QskResizable, public QskSkinnable
{
    Q_OBJECT

    Q_PROPERTY( QLocale locale READ locale
        WRITE setLocale RESET resetLocale NOTIFY localeChanged FINAL )

    Q_PROPERTY( bool autoFillBackground READ autoFillBackground
        WRITE setAutoFillBackground NOTIFY controlFlagsChanged FINAL )

    Q_PROPERTY( bool autoLayoutChildren READ autoLayoutChildren
        WRITE setAutoLayoutChildren NOTIFY controlFlagsChanged FINAL )

    Q_PROPERTY( bool polishOnResize READ polishOnResize
        WRITE setPolishOnResize NOTIFY controlFlagsChanged FINAL )

    Q_PROPERTY( bool transparentForPositioners READ isTransparentForPositioner
        WRITE setTransparentForPositioner NOTIFY controlFlagsChanged FINAL )

    Q_PROPERTY( Qt::FocusPolicy focusPolicy READ focusPolicy
        WRITE setFocusPolicy NOTIFY focusPolicyChanged FINAL )

    Q_PROPERTY( bool wheelEnabled READ isWheelEnabled
        WRITE setWheelEnabled NOTIFY wheelEnabledChanged FINAL )

    Q_PROPERTY( bool tabFence READ isTabFence
        WRITE setTabFence NOTIFY controlFlagsChanged FINAL )

    Q_PROPERTY( QMarginsF margins READ margins
        WRITE setMargins RESET resetMargins NOTIFY marginsChanged )

    Q_PROPERTY( QskGradient background READ background
        WRITE setBackground RESET resetBackground NOTIFY backgroundChanged )

    Q_PROPERTY( QskSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy )
    Q_PROPERTY( QSizeF minimumSize READ minimumSize WRITE setMinimumSize )
    Q_PROPERTY( QSizeF maximumSize READ maximumSize WRITE setMaximumSize )
    Q_PROPERTY( QSizeF preferredSize READ preferredSize WRITE setPreferredSize )

    using Inherited = QQuickItem;

public:
    QSK_STATES( Disabled, Hovered, Focused )

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

    QskControl( QQuickItem* parent = nullptr );
    virtual ~QskControl();

    const char* className() const;

    void setMargins( qreal );
    void setMargins( const QMarginsF& );
    void resetMargins();
    QMarginsF margins() const;

    void setBackgroundColor( const QColor& );

    void setBackground( const QskGradient& );
    void resetBackground();
    QskGradient background() const;

    QRectF geometry() const;
    QRectF contentsRect() const;

    virtual QRectF layoutRect() const;
    virtual QRectF gestureRect() const;
    virtual QRectF focusIndicatorRect() const;

    void setAutoFillBackground( bool );
    bool autoFillBackground() const;

    void setPolishOnResize( bool );
    bool polishOnResize() const;

    void setAutoLayoutChildren( bool );
    bool autoLayoutChildren() const;

    void setTransparentForPositioner( bool );
    bool isTransparentForPositioner() const;

    void setWheelEnabled( bool );
    bool isWheelEnabled() const;

    void setFocusPolicy( Qt::FocusPolicy );
    Qt::FocusPolicy focusPolicy() const;

    void setTabFence( bool );
    bool isTabFence() const;

    void setControlFlags( Flags );
    void resetControlFlags();
    Flags controlFlags() const;

    Q_INVOKABLE void setControlFlag( Flag, bool on = true );
    Q_INVOKABLE void resetControlFlag( Flag );
    Q_INVOKABLE bool testControlFlag( Flag ) const;

    QSizeF sizeHint() const;
    QSizeF effectiveConstraint( Qt::SizeHint ) const;

    virtual QSizeF contentsSizeHint() const;

    QLocale locale() const;
    void resetLocale();

    void setLayoutMirroring( bool on, bool recursive = false );
    void resetLayoutMirroring();
    bool layoutMirroring() const;

    QSizeF size() const;
    QSizeF implicitSize() const;

    void setGeometry( qreal x, qreal y, qreal width, qreal height );

    bool isPolishScheduled() const;
    bool isUpdateNodeScheduled() const;
    bool isInitiallyPainted() const;

    QVector< QskAspect::Subcontrol > subControls() const;

Q_SIGNALS:
    void backgroundChanged();
    void marginsChanged();
    void focusIndicatorRectChanged();
    void localeChanged( const QLocale& );
    void controlFlagsChanged();
    void focusPolicyChanged();
    void wheelEnabledChanged();

public Q_SLOTS:
    void setGeometry( const QRectF& );
    void setLocale( const QLocale& );

    void show();
    void hide();
    void setVisible( bool );

    void resetImplicitSize();

protected:
    virtual bool event( QEvent* ) override;
    virtual void changeEvent( QEvent* );
    virtual void geometryChangeEvent( QskGeometryChangeEvent* );
    virtual void windowChangeEvent( QskWindowChangeEvent* );
    virtual void gestureEvent( QskGestureEvent* );

    virtual void hoverEnterEvent( QHoverEvent* ) override;
    virtual void hoverLeaveEvent( QHoverEvent* ) override;

    virtual bool childMouseEventFilter( QQuickItem*, QEvent* ) override;
    virtual bool gestureFilter( QQuickItem*, QEvent* );

    virtual void itemChange( ItemChange, const ItemChangeData& ) override;
    virtual void classBegin() override;
    virtual void componentComplete() override;
    virtual void releaseResources() override;

    virtual void geometryChanged( const QRectF&, const QRectF& ) override;

    void cleanupNodes();

    virtual void updateLayout(); // called in updatePolish

private:
    void setActiveFocusOnTab( bool ) = delete; // use setFocusPolicy instead

    virtual QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override final;
    virtual void updatePolish() override final;

    virtual QskControl* owningControl() const override final;
    virtual void layoutConstraintChanged() override final;

    void updateImplicitSize();

    void updateControlFlag( uint flag, bool on );

private:
    Q_DECLARE_PRIVATE( QskControl )
};

inline void QskControl::setGeometry( const QRectF& rect )
{
    setGeometry( rect.x(), rect.y(), rect.width(), rect.height() );
}

inline QSizeF QskControl::size() const
{
    return QSizeF( width(), height() );
}

inline QSizeF QskControl::implicitSize() const
{
    return QSizeF( implicitWidth(), implicitHeight() );
}

inline QSizeF QskControl::sizeHint() const
{
    return effectiveConstraint( Qt::PreferredSize );
}

QSK_EXPORT bool qskIsAncestorOf( const QQuickItem* item, const QQuickItem *child );
QSK_EXPORT bool qskIsTransparentForPositioner( const QQuickItem* );
QSK_EXPORT bool qskIsTabFence( const QQuickItem* );
QSK_EXPORT bool qskIsShortcutScope( const QQuickItem* );

QSK_EXPORT QQuickItem* qskNearestFocusScope( const QQuickItem* );
QSK_EXPORT QList<QQuickItem *> qskPaintOrderChildItems( const QQuickItem* );

QSK_EXPORT const QSGNode* qskItemNode( const QQuickItem* );
QSK_EXPORT const QSGNode* qskPaintNode( const QQuickItem* );

Q_DECLARE_OPERATORS_FOR_FLAGS( QskControl::Flags )
Q_DECLARE_METATYPE( QskControl::Flags )

#endif
