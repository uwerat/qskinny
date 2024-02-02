/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SETUP_H
#define QSK_SETUP_H

#include "QskGlobal.h"
#include "QskItem.h"

namespace QskSetup
{
    QSK_EXPORT void setUpdateFlags( QskItem::UpdateFlags );
    QSK_EXPORT void resetUpdateFlags();
    QSK_EXPORT QskItem::UpdateFlags updateFlags();

    QSK_EXPORT void setUpdateFlag( QskItem::UpdateFlag, bool on = true );
    QSK_EXPORT void resetUpdateFlag( QskItem::UpdateFlag );
    QSK_EXPORT bool testUpdateFlag( QskItem::UpdateFlag );
};

#endif
