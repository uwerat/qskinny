/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CONTROL_H
#define QSK_CONTROL_H 1

#include "QskAspect.h"
#include "QskGradient.h"
#include "QskSizePolicy.h"
#include "QskSkinnable.h"

#include <qlocale.h>
#include <qquickitem.h>

#include <memory>

class QskControlPrivate;

class QskGeometryChangeEvent;
class QskWindowChangeEvent;
class QskGestureEvent;

template <typename T> class QVector;

class QSK_EXPORT QskControl : public QQuickItem, public QskSkinnable
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

    Q_PROPERTY( QRectF geometry READ geometry WRITE setGeometry )

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
    ~QskControl() override;

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

    QRectF rect() const;
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

    void setSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );
    void setSizePolicy( const QskSizePolicy& );
    void setSizePolicy( Qt::Orientation, QskSizePolicy::Policy );

    const QskSizePolicy& sizePolicy() const;
    QskSizePolicy::Policy sizePolicy( Qt::Orientation ) const;

    void setMinimumSize( const QSizeF& );
    void setMinimumSize( qreal width, qreal height );
    void setMinimumWidth( qreal width );
    void setMinimumHeight( qreal height );

    void setMaximumSize( const QSizeF& );
    void setMaximumSize( qreal width, qreal height );
    void setMaximumWidth( qreal width );
    void setMaximumHeight( qreal height );

    void setPreferredSize( const QSizeF& );
    void setPreferredSize( qreal width, qreal height );
    void setPreferredWidth( qreal width );
    void setPreferredHeight( qreal height );

    void setFixedSize( const QSizeF& );
    void setFixedSize( qreal width, qreal height );
    void setFixedWidth( qreal width );
    void setFixedHeight( qreal height );

    void setExplicitSizeHint( Qt::SizeHint, const QSizeF& );
    void setExplicitSizeHint( Qt::SizeHint, qreal width, qreal height );
    void resetExplicitSizeHint( Qt::SizeHint );

    QSizeF minimumSize() const;
    QSizeF maximumSize() const;
    QSizeF preferredSize() const;

    QSizeF explicitSizeHint( Qt::SizeHint ) const;

    QSizeF sizeHint() const;
    QSizeF effectiveSizeHint( Qt::SizeHint ) const;

    virtual qreal heightForWidth( qreal width ) const;
    virtual qreal widthForHeight( qreal height ) const;

    virtual QSizeF contentsSizeHint() const;

    bool isVisibleTo( const QQuickItem* ) const;

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
    bool event( QEvent* ) override;

    virtual void changeEvent( QEvent* );
    virtual void geometryChangeEvent( QskGeometryChangeEvent* );
    virtual void windowChangeEvent( QskWindowChangeEvent* );
    virtual void gestureEvent( QskGestureEvent* );

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

    bool childMouseEventFilter( QQuickItem*, QEvent* ) override;
    virtual bool gestureFilter( QQuickItem*, QEvent* );

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void geometryChanged( const QRectF&, const QRectF& ) override;
    void windowDeactivateEvent() override;
    void classBegin() override;
    void componentComplete() override;
    void releaseResources() override;

    void initSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );

    void cleanupNodes();

    virtual void aboutToShow();  // called in updatePolish
    virtual void updateLayout(); // called in updatePolish

  private:
    // don't use boundingRect - it seems to be deprecated
    QRectF boundingRect() const override final { return rect(); }

    void setActiveFocusOnTab( bool ) = delete;                 // use setFocusPolicy
    void updateInputMethod( Qt::InputMethodQueries ) = delete; // use qskUpdateInputMethod

    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override final;
    void updatePolish() override final;

    QskControl* owningControl() const override final;

    void layoutConstraintChanged();
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
    return effectiveSizeHint( Qt::PreferredSize );
}

inline QSizeF QskControl::minimumSize() const
{
    return explicitSizeHint( Qt::MinimumSize );
}

inline QSizeF QskControl::maximumSize() const
{
    return explicitSizeHint( Qt::MaximumSize );
}

inline QSizeF QskControl::preferredSize() const
{
    return explicitSizeHint( Qt::PreferredSize );
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskControl::Flags )
Q_DECLARE_METATYPE( QskControl::Flags )

#endif
