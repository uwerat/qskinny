/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_H
#define QSK_SKIN_HINT_TABLE_H

#include "QskGlobal.h"
#include "QskAspect.h"
#include "QskAnimationHint.h"

#include <QVariant>
#include <QColor>

#include <unordered_map>
#include <set>

class QSK_EXPORT QskSkinHintTable
{
public:
    QskSkinHintTable( bool extraAnimatorLookup );
    ~QskSkinHintTable();

    void setColor( QskAspect::Aspect, Qt::GlobalColor );
    void setColor( QskAspect::Aspect, QRgb );

    void setColor( QskAspect::Aspect, const QColor& );
    QColor color( QskAspect::Aspect aspect ) const;

    void setMetric( QskAspect::Aspect, qreal metric );
    qreal metric( QskAspect::Aspect ) const;

    void setAnimation( QskAspect::Aspect, QskAnimationHint animation );
    QskAnimationHint animation( QskAspect::Aspect ) const;

    void setGraphicRole( QskAspect::Aspect, int role );
    void setFontRole( QskAspect::Aspect, int role );
    void setFlagHint( QskAspect::Aspect, int flag );

    bool hasAnimatorLookup() const;

    void setSkinHint( QskAspect::Aspect, const QVariant& );
    const QVariant& skinHint( QskAspect::Aspect ) const;
    void removeSkinHint( QskAspect::Aspect );

    bool hasHint( QskAspect::Aspect ) const;

    const std::set< QskAspect::Aspect >& animatorAspects( QskAspect::Subcontrol ) const;
    const std::unordered_map< QskAspect::Aspect, QVariant >& hints() const;

private:
    std::unordered_map< QskAspect::Aspect, QVariant >* m_hints;
    std::unordered_map< int, std::set< QskAspect::Aspect > >* m_animatorAspects;
};

inline bool QskSkinHintTable::hasAnimatorLookup() const
{
    return m_animatorAspects != nullptr;
}

inline bool QskSkinHintTable::hasHint( QskAspect::Aspect aspect ) const
{
    if ( m_hints )
        return m_hints->find( aspect ) != m_hints->cend();

    return false;
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, const QColor& color )
{
    setSkinHint( aspect | QskAspect::Color, color );
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, Qt::GlobalColor color )
{
    setSkinHint( aspect | QskAspect::Color, QColor( color ) );
}

inline void QskSkinHintTable::setColor(
    QskAspect::Aspect aspect, QRgb rgb )
{
    setSkinHint( aspect | QskAspect::Color, QColor::fromRgba( rgb ) );
}

inline QColor QskSkinHintTable::color( QskAspect::Aspect aspect ) const
{
    return skinHint( aspect | QskAspect::Color ).value<QColor>();
}

inline void QskSkinHintTable::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    setSkinHint( aspect | QskAspect::Metric, metric );
}

inline qreal QskSkinHintTable::metric( QskAspect::Aspect aspect ) const
{
    return skinHint( aspect | QskAspect::Metric ).toReal();
}

inline void QskSkinHintTable::setAnimation(
    QskAspect::Aspect aspect, QskAnimationHint animation )
{
    aspect.setAnimator( true );
    setSkinHint( aspect, QVariant::fromValue( animation ) );
}

inline QskAnimationHint QskSkinHintTable::animation( QskAspect::Aspect aspect ) const
{
    aspect.setAnimator( true );
    return skinHint( aspect ).value< QskAnimationHint >();
}

inline void QskSkinHintTable::setGraphicRole( QskAspect::Aspect aspect, int role )
{
    setSkinHint( aspect | QskAspect::GraphicRole, role );
}

inline void QskSkinHintTable::setFontRole( QskAspect::Aspect aspect, int role )
{
    setSkinHint( aspect | QskAspect::FontRole, role );
}

inline void QskSkinHintTable::setFlagHint( QskAspect::Aspect aspect, int flag )
{
    setSkinHint( aspect, QVariant( flag ) );
}

#endif
