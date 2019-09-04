/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CONTROL_H
#define QSK_CONTROL_H

#include "QskQuickItem.h"
#include "QskSkinnable.h"
#include "QskAspect.h"
#include "QskGradient.h"
#include "QskSizePolicy.h"

#include <qlocale.h>
#include <memory>

class QskControlPrivate;
class QskGestureEvent;

template< typename T > class QVector;

class QSK_EXPORT QskControl : public QskQuickItem, public QskSkinnable
{
    Q_OBJECT

    Q_PROPERTY( QLocale locale READ locale
        WRITE setLocale RESET resetLocale NOTIFY localeChanged FINAL )

    Q_PROPERTY( bool autoFillBackground READ autoFillBackground
        WRITE setAutoFillBackground FINAL )

    Q_PROPERTY( bool autoLayoutChildren READ autoLayoutChildren
        WRITE setAutoLayoutChildren FINAL )

    Q_PROPERTY( Qt::FocusPolicy focusPolicy READ focusPolicy
        WRITE setFocusPolicy NOTIFY focusPolicyChanged FINAL )

    Q_PROPERTY( bool wheelEnabled READ isWheelEnabled
        WRITE setWheelEnabled NOTIFY wheelEnabledChanged FINAL )

    Q_PROPERTY( QMarginsF margins READ margins
        WRITE setMargins RESET resetMargins NOTIFY marginsChanged )

    Q_PROPERTY( QskGradient background READ background
        WRITE setBackground RESET resetBackground NOTIFY backgroundChanged )

    Q_PROPERTY( QskSizePolicy sizePolicy READ sizePolicy WRITE setSizePolicy )

    Q_PROPERTY( QSizeF minimumSize READ minimumSize WRITE setMinimumSize )
    Q_PROPERTY( QSizeF maximumSize READ maximumSize WRITE setMaximumSize )
    Q_PROPERTY( QSizeF preferredSize READ preferredSize WRITE setPreferredSize )

    using Inherited = QskQuickItem;

  public:
    QSK_STATES( Disabled, Hovered, Focused )

    QskControl( QQuickItem* parent = nullptr );
    ~QskControl() override;

    void setMargins( qreal );
    void setMargins( const QMarginsF& );
    void resetMargins();
    QMarginsF margins() const;

    void setBackgroundColor( const QColor& );

    void setBackground( const QskGradient& );
    void resetBackground();
    QskGradient background() const;

    QRectF contentsRect() const;
    QRectF layoutRect() const;

    virtual QRectF layoutRectForSize( const QSizeF& ) const;
    virtual QRectF gestureRect() const;
    virtual QRectF focusIndicatorRect() const;

    QRectF subControlRect( QskAspect::Subcontrol ) const;
    QRectF subControlRect( const QSizeF&, QskAspect::Subcontrol ) const;

    void setAutoFillBackground( bool );
    bool autoFillBackground() const;

    void setAutoLayoutChildren( bool );
    bool autoLayoutChildren() const;

    void setWheelEnabled( bool );
    bool isWheelEnabled() const;

    void setFocusPolicy( Qt::FocusPolicy );
    Qt::FocusPolicy focusPolicy() const;

    void setSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );
    void setSizePolicy( QskSizePolicy );
    void setSizePolicy( Qt::Orientation, QskSizePolicy::Policy );

    QskSizePolicy sizePolicy() const;
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
    QSizeF implicitSizeHint( Qt::SizeHint, const QSizeF& constraint ) const;

    QSizeF sizeHint() const;
    QSizeF effectiveSizeHint( Qt::SizeHint,
        const QSizeF& constraint = QSizeF() ) const;

    virtual qreal heightForWidth( qreal width ) const;
    virtual qreal widthForHeight( qreal height ) const;

    virtual QSizeF contentsSizeHint() const;

    QLocale locale() const;
    void resetLocale();

    QVector< QskAspect::Subcontrol > subControls() const;

  Q_SIGNALS:
    void backgroundChanged();
    void marginsChanged();
    void focusIndicatorRectChanged();
    void localeChanged( const QLocale& );
    void focusPolicyChanged();
    void wheelEnabledChanged();

  public Q_SLOTS:
    void setLocale( const QLocale& );

  protected:
    bool event( QEvent* ) override;

    virtual void gestureEvent( QskGestureEvent* );

    void hoverEnterEvent( QHoverEvent* ) override;
    void hoverLeaveEvent( QHoverEvent* ) override;

    bool childMouseEventFilter( QQuickItem*, QEvent* ) override;
    virtual bool gestureFilter( QQuickItem*, QEvent* );

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void geometryChanged( const QRectF&, const QRectF& ) override;
    void windowDeactivateEvent() override;

    void initSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );

    // called from updatePolish
    virtual void updateResources();
    virtual void updateLayout();

  private:
    void setActiveFocusOnTab( bool ) = delete;                 // use setFocusPolicy
    void updateInputMethod( Qt::InputMethodQueries ) = delete; // use qskUpdateInputMethod

    QSGNode* updateItemPaintNode( QSGNode* ) override final;
    void updateItemPolish() override final;

    QskControl* owningControl() const override final;

  private:
    Q_DECLARE_PRIVATE( QskControl )
};

inline QSizeF QskControl::sizeHint() const
{
    return effectiveSizeHint( Qt::PreferredSize );
}

inline QSizeF QskControl::minimumSize() const
{
    return effectiveSizeHint( Qt::MinimumSize );
}

inline QSizeF QskControl::maximumSize() const
{
    return effectiveSizeHint( Qt::MaximumSize );
}

inline QSizeF QskControl::preferredSize() const
{
    return effectiveSizeHint( Qt::PreferredSize );
}

inline QskControl* qskControlCast( QObject* object )
{
    return qobject_cast< QskControl* >( object );
}

inline const QskControl* qskControlCast( const QObject* object )
{
    return qobject_cast< const QskControl* >( object );
}

#endif
