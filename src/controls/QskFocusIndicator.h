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
    ~QskFocusIndicator() override;

    bool contains( const QPointF& ) const override;
    QRectF clipRect() const override;

  protected:
    void windowChangeEvent( QskWindowChangeEvent* ) override;
    virtual QRectF focusRect() const;

  private:
    void onFocusItemGeometryChanged();
    void onWindowSizeChanged( int );

    void onFocusItemChanged();
    void onFocusItemDestroyed();
    void updateFocusFrame();

    void connectWindow( const QQuickWindow*, bool on );
    QVector< QMetaObject::Connection > connectItem( const QQuickItem* );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
