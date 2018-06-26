/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_POPUP_H
#define QSK_POPUP_H 1

#include "QskGlobal.h"
#include "QskControl.h"

class QSK_EXPORT QskPopup : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( bool modal READ isModal WRITE setModal NOTIFY modalChanged )
    Q_PROPERTY( bool overlay READ hasOverlay WRITE setOverlay NOTIFY overlayChanged )

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Overlay )

    QskPopup( QQuickItem* parent = nullptr );
    virtual ~QskPopup();

    void setModal( bool on = true );
    bool isModal() const;

    void setOverlay( bool on = true );
    bool hasOverlay() const;

    virtual QRectF overlayRect() const;
    virtual QRectF grabberRect() const;


Q_SIGNALS:
    void modalChanged( bool );
    void overlayChanged( bool );

protected:
    virtual void aboutToShow() override;

    virtual bool event( QEvent* ) override;
    virtual void focusInEvent( QFocusEvent * ) override;
    virtual void focusOutEvent( QFocusEvent * ) override;

    virtual void itemChange( QQuickItem::ItemChange,
        const QQuickItem::ItemChangeData& ) override;

    virtual QQuickItem* focusSuccessor() const;

    void grabFocus( bool );

private:
    void updateInputGrabber();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
