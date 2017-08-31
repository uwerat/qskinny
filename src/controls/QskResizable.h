/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RESIZABLE_H
#define QSK_RESIZABLE_H 1

#include "QskGlobal.h"
#include "QskSizePolicy.h"
#include <QSizeF>

class QSK_EXPORT QskResizable
{
public:
    enum SizeConstraint
    {
        NoConstraint,
        HeightForWidth,
        WidthForHeight
    };

    QskResizable();
    virtual ~QskResizable();

    void setSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );
    void setSizePolicy( const QskSizePolicy& );
    void setSizePolicy( Qt::Orientation, QskSizePolicy::Policy );

    const QskSizePolicy& sizePolicy() const;
    QskSizePolicy::Policy sizePolicy( Qt::Orientation ) const;

    void setMinimumSize( const QSizeF& );
    void setMinimumSize( qreal width, qreal height );
    void setMinimumWidth( qreal width );
    void setMinimumHeight( qreal height );

    void setMaximumSize( const QSizeF& );
    void setMaximumSize( qreal width, qreal height );
    void setMaximumWidth( qreal width );
    void setMaximumHeight( qreal height );

    void setPreferredSize( const QSizeF& );
    void setPreferredSize( qreal width, qreal height );
    void setPreferredWidth( qreal width );
    void setPreferredHeight( qreal height );

    void setFixedSize( const QSizeF& );
    void setFixedSize( qreal width, qreal height );
    void setFixedWidth( qreal width );
    void setFixedHeight( qreal height );

    void setSizeHint( Qt::SizeHint, const QSizeF& );
    void setSizeHint( Qt::SizeHint, qreal width, qreal height );
    void resetSizeHint( Qt::SizeHint );

    QSizeF minimumSize() const;
    QSizeF maximumSize() const;
    QSizeF preferredSize() const;

    QSizeF sizeHint( Qt::SizeHint ) const;

    virtual qreal heightForWidth( qreal width ) const;
    virtual qreal widthForHeight( qreal height ) const;

protected:
    void initSizePolicy( QskSizePolicy::Policy, QskSizePolicy::Policy );

private:
    virtual void layoutConstraintChanged();

private:
    QskSizePolicy m_sizePolicy;
    QSizeF m_sizeHints[3];
};

#endif

