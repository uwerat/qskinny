/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_POPUP_H
#define QSK_POPUP_H

#include "QskControl.h"

class QSK_EXPORT QskPopup : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( bool open READ isOpen WRITE setOpen NOTIFY openChanged )
    Q_PROPERTY( bool modal READ isModal WRITE setModal NOTIFY modalChanged )
    Q_PROPERTY( bool transitioning READ isTransitioning NOTIFY transitioningChanged )

    Q_PROPERTY( bool overlay READ hasOverlay
        WRITE setOverlay RESET resetOverlay NOTIFY overlayChanged )

    Q_PROPERTY( uint priority READ priority WRITE setPriority NOTIFY priorityChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Overlay )
    QSK_STATES( Closed )

    enum PopupFlag
    {
        DeleteOnClose       = 1 << 0,
        CloseOnPressOutside = 1 << 1
    };

    Q_ENUM( PopupFlag )
    Q_DECLARE_FLAGS( PopupFlags, PopupFlag )

    QskPopup( QQuickItem* parent = nullptr );
    ~QskPopup() override;

    void setPopupFlags( PopupFlags );
    PopupFlags popupFlags() const;

    void setPopupFlag( PopupFlag, bool on = true );
    bool testPopupFlag( PopupFlag ) const;

    void setModal( bool on = true );
    bool isModal() const;

    void setOverlay( bool on = true );
    void resetOverlay();
    bool hasOverlay() const;

    // allows for stacking orders based on priorities
    void setPriority( uint );
    uint priority() const;

    // transitions between open/closed states
    QskAspect transitionAspect() const;
    void setTransitionAspect( QskAspect );

    bool isTransitioning() const;

    bool isOpen() const;
    bool isClosed() const;

    virtual QRectF overlayRect() const;

  public Q_SLOTS:
    void open();
    void close();
    void toggle();

    void setOpen( bool );

  Q_SIGNALS:
    void opened();
    void closed();
    void openChanged( bool );
    void transitioningChanged( bool );

    void modalChanged( bool );
    void overlayChanged( bool );
    void priorityChanged( uint );

  protected:
    void aboutToShow() override;
    int execPopup();

    bool event( QEvent* ) override;
    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;
    void windowChangeEvent( QskWindowChangeEvent* ) override;

    void itemChange( QQuickItem::ItemChange,
        const QQuickItem::ItemChangeData& ) override;

    virtual QQuickItem* focusSuccessor() const;
    bool isTransitionAccepted( QskAspect ) const override;

    void grabFocus( bool );

  private:
    void show() = delete;
    void hide() = delete;
    void setVisible( bool ) = delete;

    void updateInputGrabber();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

inline bool QskPopup::isClosed() const
{
    return !isOpen();
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QskPopup::PopupFlags )

#endif
