/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_FUSION_SKIN_H
#define QSK_FUSION_SKIN_H

#include "QskFusionGlobal.h"
#include <QskSkin.h>

class QSK_FUSION_EXPORT QskFusionSkin : public QskSkin
{
    Q_OBJECT

    using Inherited = QskSkin;

  public:
    QskFusionSkin( QObject* parent = nullptr );
    ~QskFusionSkin() override;

    enum GraphicRole
    {
        GraphicNormal,
        GraphicDisabled,
        GraphicHighlighted,
        GraphicError
    };

  protected:
    void initHints() override;

  private:
    void setGraphicColor( GraphicRole, QRgb );
};

#endif
