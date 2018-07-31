/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STACK_BOX_H
#define QSK_STACK_BOX_H

#include "QskGlobal.h"
#include "QskIndexedLayoutBox.h"

class QskStackBoxAnimator;

class QSK_EXPORT QskStackBox : public QskIndexedLayoutBox
{
    Q_OBJECT

    Q_PROPERTY( int currentIndex READ currentIndex
        WRITE setCurrentIndex NOTIFY currentIndexChanged )

    Q_PROPERTY( QQuickItem* currentItem READ currentItem
        WRITE setCurrentItem NOTIFY currentItemChanged )

    using Inherited = QskIndexedLayoutBox;

public:
    explicit QskStackBox( QQuickItem* parent = nullptr );
    QskStackBox( bool autoAddChildren, QQuickItem* parent = nullptr );

    ~QskStackBox() override;

    QQuickItem* currentItem() const;
    int currentIndex() const;

    QSizeF contentsSizeHint() const override;

    qreal heightForWidth( qreal width ) const override;
    qreal widthForHeight( qreal height ) const override;

    void setAnimator( QskStackBoxAnimator* );
    const QskStackBoxAnimator* animator() const;
    QskStackBoxAnimator* animator();

public Q_SLOTS:
    void setCurrentIndex( int index );
    void setCurrentItem( const QQuickItem* );

Q_SIGNALS:
    void currentIndexChanged( int index );
    void currentItemChanged( QQuickItem* );

protected:
    QskStackBoxAnimator* effectiveAnimator();

private:
    friend class QskStackBoxAnimator;

    void layoutItemInserted( QskLayoutItem*, int index ) override;
    void layoutItemRemoved( QskLayoutItem*, int index ) override;

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
