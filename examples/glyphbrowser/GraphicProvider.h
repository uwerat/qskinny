/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskGlyphGraphicProvider.h>

class GraphicProvider : public QskGlyphGraphicProvider
{
    using Inherited = QskGlyphGraphicProvider;

  public:
    GraphicProvider( QObject* parent = nullptr );

    void updateFont();

  private:
    QString m_fontFileName;
};
