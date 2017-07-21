/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_FOCUS_INDICATOR_H
#define QSK_FOCUS_INDICATOR_H

#include "QskControl.h"

class QSK_EXPORT QskFocusIndicator : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

public:
    QSK_SUBCONTROLS( Panel )

    QskFocusIndicator( QQuickItem* parent = nullptr );
    virtual ~QskFocusIndicator();

protected:
    virtual void windowChangeEvent( QskWindowChangeEvent* ) override;

private Q_SLOTS:
    void updateFocusFrame();
    void onFocusItemChanged();
    void onFocusItemGeometryChanged();

private:
    QRectF focusRect() const;

    void resetConnections();
    void connectWindow( const QQuickWindow*, bool );
};

#endif
