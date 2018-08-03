/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INPUT_GRABBER_H
#define QSK_INPUT_GRABBER_H 1

#include "QskControl.h"

class QskInputGrabber : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QskInputGrabber( QQuickItem* parent = nullptr );
    ~QskInputGrabber() override;

    virtual QRectF grabberRect() const;
    virtual bool isBlocking( const QPointF& ) const;

  public Q_SLOTS:
    void updateGeometry();

  protected:
    bool event( QEvent* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
