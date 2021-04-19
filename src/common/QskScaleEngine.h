/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SCALE_ENGINE_H
#define QSK_SCALE_ENGINE_H

#include <QskGlobal.h>
#include <qmetatype.h>

class QskScaleTickmarks;
class QskIntervalF;

class QSK_EXPORT QskScaleEngine
{
    Q_GADGET

  public:
    enum Attribute
    {
        Inverted = 1 << 0,
        Floating = 1 << 1
    };

    Q_ENUM( Attribute )
    Q_DECLARE_FLAGS( Attributes, Attribute )

    QskScaleEngine();
    ~QskScaleEngine();

    void setAttribute( Attribute, bool on = true );
    bool testAttribute( Attribute ) const;

    void setAttributes( Attributes );
    Attributes attributes() const;

    QskScaleTickmarks divideScale( qreal x1, qreal x2,
        int maxMajorSteps, int maxMinorSteps, qreal stepSize = 0.0 ) const;

    void autoScale( int maxNumSteps, qreal& x1, qreal& x2, qreal& stepSize ) const;

  private:
    QskIntervalF align( const QskIntervalF&, qreal stepSize ) const;

    QVector< qreal > strip( const QVector< qreal >&, const QskIntervalF& ) const;

    QskScaleTickmarks buildTicks(
        const QskIntervalF&, qreal stepSize, int maxMinorSteps ) const;

    QVector< qreal > buildMajorTicks( const QskIntervalF&, qreal stepSize ) const;

    void buildMinorTicks( const QVector< qreal >& majorTicks,
        int maxMinorSteps, qreal stepSize, QVector< qreal >& minorTicks,
        QVector< qreal >& mediumTicks ) const;

    Attributes m_attributes;
};

#endif
