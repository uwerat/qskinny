/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SLIDE_IN_H
#define QSK_SLIDE_IN_H

#include "QskPopup.h"
#include <qnamespace.h>

class QSK_EXPORT QskSlideIn : public QskPopup
{
    Q_OBJECT

    using Inherited = QskPopup;

  public:
    ~QskSlideIn() override;

    virtual Qt::Edge edge() const = 0;

    QRectF focusIndicatorRect() const override;
    QRectF clipRect() const override;

    void setAdjustingToParentGeometry( bool on );
    bool isAdjustingToParentGeometry() const;

    QRectF layoutRectForSize( const QSizeF& ) const override;

  protected:
    QskSlideIn( QQuickItem* = nullptr );

    bool event( QEvent* ) override;
    void itemChange( ItemChange, const ItemChangeData& ) override;

  private:
    void setIntermediate( bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
