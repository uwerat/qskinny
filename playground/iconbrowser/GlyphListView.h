/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskListView.h>
#include <QskGlyphTable.h>
#include <QHash>

class GlyphListView : public QskListView
{
    Q_OBJECT
    using Inherited = QskListView;

  public:
    GlyphListView( QQuickItem* = nullptr);
    GlyphListView( const QString&, QQuickItem* = nullptr);

    void setFontPath( const QString& );
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
