/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_INPUT_GRABBER_H
#define QSK_INPUT_GRABBER_H

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
