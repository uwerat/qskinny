/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

class QskGlyphTable;
class QString;

namespace Glyph
{
    void dumpAllFonts();
    void dumpFont( const QString& );

    void dump( const QskGlyphTable& );
    void dumpPrivateCodes( const QskGlyphTable& );
};
