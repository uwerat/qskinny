/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STACK_BOX_ANIMATOR_H
#define QSK_STACK_BOX_ANIMATOR_H

#include "QskGlobal.h"
#include "QskNamespace.h"
#include "QskAnimator.h"
#include <qobject.h>

class QskStackBox;
class QskLayoutItem;

class QSK_EXPORT QskStackBoxAnimator : public QObject, public QskAnimator
{
    Q_OBJECT

public:
    QskStackBoxAnimator( QskStackBox* parent );
    virtual ~QskStackBoxAnimator();

    void setStartIndex( int index );
    void setEndIndex( int index );

    int startIndex() const;
    int endIndex() const;

protected:
    QskStackBox* stackBox() const;
    QskLayoutItem* layoutItemAt( int index ) const;
    void resizeItemAt( int index );

private:
    int m_startIndex;
    int m_endIndex;
};

class QSK_EXPORT QskStackBoxAnimator1 : public QskStackBoxAnimator
{
    Q_OBJECT

public:
    QskStackBoxAnimator1( QskStackBox* parent );
    virtual ~QskStackBoxAnimator1();

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

protected:
    virtual void setup() override;
    virtual void advance( qreal value ) override;
    virtual void done() override;

private:
    qreal m_itemOffset[2];

    Qt::Orientation m_orientation : 2;
    Qsk::Direction m_direction : 4;
    bool m_hasClip : 1;
};

class QSK_EXPORT QskStackBoxAnimator3 : public QskStackBoxAnimator
{
    Q_OBJECT

public:
    QskStackBoxAnimator3( QskStackBox* parent );
    virtual ~QskStackBoxAnimator3();

protected:
    virtual void setup() override;
    virtual void advance( qreal value ) override;
    virtual void done() override;
};

#endif
