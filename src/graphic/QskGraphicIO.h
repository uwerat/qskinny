/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRAPHIC_IO_H
#define QSK_GRAPHIC_IO_H

#include "QskGlobal.h"

class QskGraphic;
class QString;
class QIODevice;
class QByteArray;

namespace QskGraphicIO
{
    QSK_EXPORT QskGraphic read( const QString& fileName );
    QSK_EXPORT QskGraphic read( const QByteArray& data );
    QSK_EXPORT QskGraphic read( QIODevice* dev );

    QSK_EXPORT bool write( const QskGraphic&, const QString& fileName );
    QSK_EXPORT bool write( const QskGraphic&, QByteArray& data );
    QSK_EXPORT bool write( const QskGraphic&, QIODevice* dev );
}

#endif
