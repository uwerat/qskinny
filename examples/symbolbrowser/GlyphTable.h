/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qhash.h>
#include <qbytearray.h>

class QRawFont;

namespace GlyphTable
{
    QHash< QByteArray, int > nameTable( const QRawFont& );
    void dump( const QHash< QByteArray, int >& );

    quint32 ucs4ToIndex( const QRawFont&, char32_t ucs4 );

    int dump( const QHash< QByteArray, int >&,
        const QRawFont&, char32_t from, char32_t to );

    void dumpSymbols( const QHash< QByteArray, int >&,
        const QRawFont& font );
};
