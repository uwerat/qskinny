/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STACK_BOX_ANIMATOR_H
#define QSK_STACK_BOX_ANIMATOR_H

#include "QskAnimator.h"
#include "QskNamespace.h"

#include <qobject.h>

class QskStackBox;
class QQuickItem;

class QSK_EXPORT QskStackBoxAnimator : public QObject, public QskAnimator
{
    Q_OBJECT

  public:
    QskStackBoxAnimator( QskStackBox* );
    ~QskStackBoxAnimator() override;

    void setStartIndex( int index );
    void setEndIndex( int index );

    int startIndex() const;
    int endIndex() const;

  protected:
    QskStackBox* stackBox() const;
    QQuickItem* itemAt( int index ) const;

  private:
    int m_startIndex;
    int m_endIndex;
};

class QSK_EXPORT QskStackBoxAnimator1 : public QskStackBoxAnimator
{
    Q_OBJECT

  public:
    QskStackBoxAnimator1( QskStackBox* );
    ~QskStackBoxAnimator1() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

  protected:
    bool eventFilter( QObject*, QEvent* ) override;

    void setup() override;
    void advance( qreal value ) override;
    void done() override;

  private:
    qreal m_itemOffset[ 2 ];

    Qt::Orientation m_orientation : 2;
    Qsk::Direction m_direction : 4;
    bool m_isDirty : 1;
    bool m_hasClip : 1;
};

class QSK_EXPORT QskStackBoxAnimator3 : public QskStackBoxAnimator
{
    Q_OBJECT

  public:
    QskStackBoxAnimator3( QskStackBox* );
    ~QskStackBoxAnimator3() override;

  protected:
    void setup() override;
    void advance( qreal value ) override;
    void done() override;
};

#endif
