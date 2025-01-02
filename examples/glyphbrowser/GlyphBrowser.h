/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class QskSkin;
class GraphicProvider;
class GraphicLabel;

class GlyphBrowser : public QskControl
{
    Q_OBJECT
    using Inherited = QskControl;

  public:
    GlyphBrowser( QQuickItem* = nullptr);

  private Q_SLOTS:
    void setGraphicRoles( QskSkin* );
    void showGlyph( uint );

  protected:
    void changeEvent( QEvent* ) override;

  private:
    GraphicProvider* m_graphicProvider;
    GraphicLabel* m_graphicLabel;
};
