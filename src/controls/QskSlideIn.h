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

    QRectF clipRect() const override;

    void setAdjustingToParentGeometry( bool on );
    bool isAdjustingToParentGeometry() const;

  protected:
    QskSlideIn( QQuickItem* = nullptr );

    void itemChange( ItemChange, const ItemChangeData& ) override;
    void updateResources() override;
    void updateNode( QSGNode* ) override;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
