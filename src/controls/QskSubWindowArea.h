/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SUB_WINDOW_AREA_H
#define QSK_SUB_WINDOW_AREA_H

#include "QskControl.h"

class QskSubWindow;

class QSK_EXPORT QskSubWindowArea : public QskControl
{
    Q_OBJECT

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel )

    QskSubWindowArea( QQuickItem* parent = nullptr );
    ~QskSubWindowArea() override;

  protected:
    void geometryChangeEvent( QskGeometryChangeEvent* ) override;

    bool eventFilter( QObject*, QEvent* ) override;
    virtual bool mouseEventFilter( QskSubWindow*, const QMouseEvent* );

    void itemChange( ItemChange, const ItemChangeData& ) override;

  private:
    virtual void setDragging( QskSubWindow*, bool );
    virtual void setActive( QskSubWindow*, bool );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
