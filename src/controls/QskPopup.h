/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_POPUP_H
#define QSK_POPUP_H 1

#include "QskControl.h"

class QSK_EXPORT QskPopup : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( bool modal READ isModal WRITE setModal NOTIFY modalChanged )
    Q_PROPERTY( bool overlay READ hasOverlay WRITE setOverlay NOTIFY overlayChanged )
    Q_PROPERTY( uint priority READ priority WRITE setPriority NOTIFY priorityChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Overlay )

    enum PopupFlag
    {
        CloseOnHide         = 1 << 0,
        DeleteOnClose       = 1 << 1,
        CloseOnPressOutside = 1 << 2
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
    bool hasOverlay() const;

    // allows for stacking according to priorities
    void setPriority( uint );
    uint priority() const;

    virtual QRectF overlayRect() const;

    bool isOpen() const;

  public Q_SLOTS:
    void open();
    void close();

  Q_SIGNALS:
    void closed();
    void modalChanged( bool );
    void overlayChanged( bool );
    void priorityChanged( uint );

  protected:
    void aboutToShow() override;
    virtual void setFading( bool on );

    bool event( QEvent* ) override;
    void focusInEvent( QFocusEvent* ) override;
    void focusOutEvent( QFocusEvent* ) override;
    void windowChangeEvent( QskWindowChangeEvent* ) override;

    void itemChange( QQuickItem::ItemChange,
        const QQuickItem::ItemChangeData& ) override;

    virtual QQuickItem* focusSuccessor() const;

    void grabFocus( bool );

  private:
    void updateInputGrabber();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
