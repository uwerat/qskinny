/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INDEXED_LAYOUT_BOX_H
#define QSK_INDEXED_LAYOUT_BOX_H

#include "QskLayoutBox.h"

class QSK_EXPORT QskIndexedLayoutBox : public QskLayoutBox
{
    Q_OBJECT

    Q_PROPERTY( bool autoAddChildren READ autoAddChildren
        WRITE setAutoAddChildren NOTIFY autoAddChildrenChanged )

    Q_PROPERTY( Qt::Alignment defaultAlignment READ defaultAlignment
        WRITE setDefaultAlignment NOTIFY defaultAlignmentChanged )

    using Inherited = QskLayoutBox;

  public:
    explicit QskIndexedLayoutBox( QQuickItem* parent = nullptr );
    ~QskIndexedLayoutBox() override;

    void setAutoAddChildren( bool );
    bool autoAddChildren() const;

    void setDefaultAlignment( Qt::Alignment );
    Qt::Alignment defaultAlignment() const;

    Q_INVOKABLE void addItem(
        QQuickItem*, Qt::Alignment alignment = Qt::Alignment() );

    Q_INVOKABLE void insertItem(
        int index, QQuickItem*, Qt::Alignment alignment = Qt::Alignment() );

    void setAlignment( int index, Qt::Alignment );
    Qt::Alignment alignment( int index ) const;

    void setAlignment( const QQuickItem*, Qt::Alignment );
    Qt::Alignment alignment( const QQuickItem* ) const;

  Q_SIGNALS:
    void autoAddChildrenChanged();
    void defaultAlignmentChanged();

  protected:
    void itemChange( ItemChange, const ItemChangeData& ) override;
    void insertLayoutItem( QskLayoutItem*, int index );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
