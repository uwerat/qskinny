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
class QTransform;

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

    qreal transientIndex() const;

  protected:
    QskStackBox* stackBox() const;
    QQuickItem* itemAt( int index ) const;

  private:
    void advance( qreal value ) override final;
    virtual void advanceIndex( qreal value ) = 0;

    int m_startIndex;
    int m_endIndex;

    qreal m_transientIndex;
};

class QSK_EXPORT QskStackBoxAnimator1 : public QskStackBoxAnimator
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation )

  public:
    QskStackBoxAnimator1( QskStackBox* );
    ~QskStackBoxAnimator1() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

  protected:
    bool eventFilter( QObject*, QEvent* ) override;

    void setup() override;
    void advanceIndex( qreal value ) override;
    void done() override;

  private:
    qreal m_itemOffset[ 2 ];

    Qt::Orientation m_orientation : 2;
    Qsk::Direction m_direction : 4;
    bool m_isDirty : 1;
    bool m_hasClip : 1;
};

class QSK_EXPORT QskStackBoxAnimator2 : public QskStackBoxAnimator
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( bool inverted READ isInverted WRITE setInverted )

  public:
    QskStackBoxAnimator2( QskStackBox* );
    ~QskStackBoxAnimator2() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setInverted( bool );
    bool isInverted() const;

  protected:
    void setup() override;
    void advanceIndex( qreal value ) override;
    void done() override;

  private:
    Qt::Orientation m_orientation : 2;
    bool m_inverted : 1;
};

class QSK_EXPORT QskStackBoxAnimator3 : public QskStackBoxAnimator
{
    Q_OBJECT

  public:
    QskStackBoxAnimator3( QskStackBox* );
    ~QskStackBoxAnimator3() override;

  protected:
    void setup() override;
    void advanceIndex( qreal value ) override;
    void done() override;
};

class QSK_EXPORT QskStackBoxAnimator4 : public QskStackBoxAnimator
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( bool inverted READ isInverted WRITE setInverted )

  public:
    QskStackBoxAnimator4( QskStackBox* );
    ~QskStackBoxAnimator4() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setInverted( bool );
    bool isInverted() const;

  protected:
    void setup() override;
    void advanceIndex( qreal value ) override;
    void done() override;

  private:
    QTransform transformation( const QQuickItem*, bool increasing, qreal value ) const;

    Qt::Orientation m_orientation : 2;
    bool m_inverted : 1;
};

#endif
