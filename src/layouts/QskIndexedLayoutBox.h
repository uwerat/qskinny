/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_INDEXED_LAYOUT_BOX_H
#define QSK_INDEXED_LAYOUT_BOX_H

#include "QskBox.h"

class QSK_EXPORT QskIndexedLayoutBox : public QskBox
{
    Q_OBJECT

    Q_PROPERTY( bool autoAddChildren READ autoAddChildren
        WRITE setAutoAddChildren NOTIFY autoAddChildrenChanged )

    using Inherited = QskBox;

  public:
    explicit QskIndexedLayoutBox( QQuickItem* parent = nullptr );
    ~QskIndexedLayoutBox() override;

    void setAutoAddChildren( bool on = true );
    bool autoAddChildren() const;

  Q_SIGNALS:
    void autoAddChildrenChanged();

  protected:
    void itemChange( ItemChange, const ItemChangeData& ) override;

    void reparentItem( QQuickItem* );
    void unparentItem( QQuickItem* );

  private:
    virtual void autoAddItem( QQuickItem* ) = 0;
    virtual void autoRemoveItem( QQuickItem* ) = 0;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
