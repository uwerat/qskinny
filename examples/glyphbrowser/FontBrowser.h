/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskListView.h>
#include <QskGlyphTable.h>
#include <QHash>

class FontBrowser : public QskListView
{
    Q_OBJECT
    using Inherited = QskListView;

  public:
    FontBrowser( QQuickItem* = nullptr);
    FontBrowser( const QString&, QQuickItem* = nullptr);

    void setFont( const QRawFont& );
    QRawFont font() const;

    int rowCount() const override;
    int columnCount() const override;

    virtual qreal columnWidth( int col ) const override;
    virtual qreal rowHeight() const override;

    QVariant valueAt( int row, int col ) const override;

  private:
    QskGlyphTable m_glyphTable;

    QHash< uint, QString > m_nameTable;
    int m_maxNameWidth = 0;
};
