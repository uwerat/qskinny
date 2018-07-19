/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_H
#define QSK_SKIN_HINT_TABLE_H

#include "QskGlobal.h"
#include "QskAspect.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskAnimationHint.h"

#include <qvariant.h>
#include <qcolor.h>

#include <unordered_map>

class QSK_EXPORT QskSkinHintTable
{
public:
    QskSkinHintTable();
    QskSkinHintTable( const QskSkinHintTable& other );

    ~QskSkinHintTable();

    QskSkinHintTable& operator=( const QskSkinHintTable& );

    void setColor( QskAspect::Aspect, Qt::GlobalColor );
    void setColor( QskAspect::Aspect, QRgb );

    void setColor( QskAspect::Aspect, const QColor& );
    QColor color( QskAspect::Aspect aspect ) const;

    void setMetric( QskAspect::Aspect, qreal metric );
    qreal metric( QskAspect::Aspect ) const;

    void setMargins( QskAspect::Aspect, const QskMargins& );
    QskMargins margins( QskAspect::Aspect ) const;

    void setGradient( QskAspect::Aspect, const QskGradient& );
    QskGradient gradient( QskAspect::Aspect ) const;

    void setBoxShape( QskAspect::Aspect, const QskBoxShapeMetrics& );
    QskBoxShapeMetrics boxShape( QskAspect::Aspect ) const;

    void setBoxBorder( QskAspect::Aspect, const QskBoxBorderMetrics& );
    QskBoxBorderMetrics boxBorder( QskAspect::Aspect ) const;

    void setBoxBorderColors( QskAspect::Aspect, const QskBoxBorderColors& );
    QskBoxBorderColors boxBorderColors( QskAspect::Aspect ) const;

    void setAnimation( QskAspect::Aspect, QskAnimationHint animation );
    QskAnimationHint animation( QskAspect::Aspect ) const;

    void setGraphicRole( QskAspect::Aspect, int role );
    void setFontRole( QskAspect::Aspect, int role );
    void setFlagHint( QskAspect::Aspect, int flag );

    void setHint( QskAspect::Aspect, const QVariant& );
    const QVariant& hint( QskAspect::Aspect ) const;
    void removeHint( QskAspect::Aspect );

    bool hasHint( QskAspect::Aspect ) const;

    const std::unordered_map< QskAspect::Aspect, QVariant >& hints() const;

    bool hasAnimators() const;
    bool hasStates() const;
    bool hasHints() const;

    void clear();

    const QVariant* resolvedHint( QskAspect::Aspect,
        QskAspect::Aspect* resolvedAspect = nullptr ) const;

    QskAspect::Aspect resolvedAspect( QskAspect::Aspect ) const;

    QskAspect::Aspect resolvedAnimator(
        QskAspect::Aspect, QskAnimationHint& ) const;

private:
    static QVariant invalidHint;

    typedef std::unordered_map< QskAspect::Aspect, QVariant > HintMap;
    HintMap* m_hints;

    quint16 m_animatorCount;
    bool m_hasStates : 1;
};

inline bool QskSkinHintTable::hasHints() const
{
    return m_hints != nullptr;
}

inline bool QskSkinHintTable::hasStates() const
{
    return m_hasStates;
}

inline bool QskSkinHintTable::hasAnimators() const
{
    return m_animatorCount;
}

inline bool QskSkinHintTable::hasHint( QskAspect::Aspect aspect ) const
{
    if ( m_hints != nullptr )
        return m_hints->find( aspect ) != m_hints->cend();

    return false;
}

inline const QVariant& QskSkinHintTable::hint( QskAspect::Aspect aspect ) const
{
    if ( m_hints != nullptr )
    {
        auto it = m_hints->find( aspect );
        if ( it != m_hints->cend() )
            return it->second;
    }

    return invalidHint;
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, const QColor& color )
{
    setHint( aspect | QskAspect::Color, color );
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, Qt::GlobalColor color )
{
    setHint( aspect | QskAspect::Color, QColor( color ) );
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, QRgb rgb )
{
    setHint( aspect | QskAspect::Color, QColor::fromRgba( rgb ) );
}

inline QColor QskSkinHintTable::color( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Color ).value<QColor>();
}

inline void QskSkinHintTable::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    setHint( aspect | QskAspect::Metric, metric );
}

inline qreal QskSkinHintTable::metric( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Metric ).toReal();
}

inline void QskSkinHintTable::setMargins( QskAspect::Aspect aspect, const QskMargins& margins )
{
    setHint( aspect | QskAspect::Metric, QVariant::fromValue( margins ) );
}

inline QskMargins QskSkinHintTable::margins( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Metric ).value< QskMargins >();
}

inline void QskSkinHintTable::setGradient(
    QskAspect::Aspect aspect, const QskGradient& gradient )
{
    setHint( aspect | QskAspect::Color, QVariant::fromValue( gradient ) );
}

inline QskGradient QskSkinHintTable::gradient( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Color ).value< QskGradient >();
}

inline void QskSkinHintTable::setBoxShape(
    QskAspect::Aspect aspect, const QskBoxShapeMetrics& shape )
{
    using namespace QskAspect;
    setHint( aspect | Shape | Metric, QVariant::fromValue( shape ) );
}

inline QskBoxShapeMetrics QskSkinHintTable::boxShape( QskAspect::Aspect aspect ) const
{
    using namespace QskAspect;
    return hint( aspect | Shape | Metric ).value< QskBoxShapeMetrics >();
}

inline void QskSkinHintTable::setBoxBorder(
    QskAspect::Aspect aspect, const QskBoxBorderMetrics& border )
{
    using namespace QskAspect;
    setHint( aspect | Border | Metric, QVariant::fromValue( border ) );
}
    
inline QskBoxBorderMetrics QskSkinHintTable::boxBorder( QskAspect::Aspect aspect ) const
{
    using namespace QskAspect;
    return hint( aspect | Border | Metric ).value< QskBoxBorderMetrics >();
}
    
inline void QskSkinHintTable::setBoxBorderColors(
    QskAspect::Aspect aspect, const QskBoxBorderColors& colors )
{   
    using namespace QskAspect;
    setHint( aspect | Border | Color, QVariant::fromValue( colors ) );
}   
    
inline QskBoxBorderColors QskSkinHintTable::boxBorderColors( QskAspect::Aspect aspect ) const
{
    using namespace QskAspect;
    return hint( aspect | Border | Color ).value< QskBoxBorderColors >();
}   

inline QskAnimationHint QskSkinHintTable::animation( QskAspect::Aspect aspect ) const
{
    aspect.setAnimator( true );
    return hint( aspect ).value< QskAnimationHint >();
}

inline void QskSkinHintTable::setGraphicRole( QskAspect::Aspect aspect, int role )
{
    setHint( aspect | QskAspect::GraphicRole, role );
}

inline void QskSkinHintTable::setFontRole( QskAspect::Aspect aspect, int role )
{
    setHint( aspect | QskAspect::FontRole, role );
}

inline void QskSkinHintTable::setFlagHint( QskAspect::Aspect aspect, int flag )
{
    setHint( aspect, QVariant( flag ) );
}

inline void QskSkinHintTable::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{   
    aspect.setAnimator( true );
    setHint( aspect, QVariant::fromValue( animation ) ); 
}

#endif
