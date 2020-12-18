/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_H
#define QSK_SKIN_HINT_TABLE_H

#include "QskAnimationHint.h"
#include "QskAspect.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradient.h"
#include "QskMargins.h"
#include "QskIntervalF.h"

#include <qcolor.h>
#include <qvariant.h>

#include <unordered_map>

class QskSkinHintTable
{
  public:
    QskSkinHintTable();
    QskSkinHintTable( const QskSkinHintTable& other );

    ~QskSkinHintTable();

    QskSkinHintTable& operator=( const QskSkinHintTable& );

    void setColor( QskAspect::Aspect, Qt::GlobalColor );
    void setColor( QskAspect::Aspect, QRgb );

    void setColor( QskAspect::Aspect, const QColor& );
    QColor color( QskAspect::Aspect ) const;

    void setMetric( QskAspect::Aspect, qreal metric );
    qreal metric( QskAspect::Aspect ) const;

    void setStrutSize( QskAspect::Aspect, const QSizeF& );
    QSizeF strutSize( QskAspect::Aspect ) const;

    void setMargin( QskAspect::Aspect, const QskMargins& );
    QskMargins margin( QskAspect::Aspect ) const;

    void setPadding( QskAspect::Aspect, const QskMargins& );
    QskMargins padding( QskAspect::Aspect ) const;

    void setGradient( QskAspect::Aspect, const QskGradient& );
    QskGradient gradient( QskAspect::Aspect ) const;

    void setBoxShape( QskAspect::Aspect, const QskBoxShapeMetrics& );
    QskBoxShapeMetrics boxShape( QskAspect::Aspect ) const;

    void setBoxBorder( QskAspect::Aspect, const QskBoxBorderMetrics& );
    QskBoxBorderMetrics boxBorder( QskAspect::Aspect ) const;

    void setBoxBorderColors( QskAspect::Aspect, const QskBoxBorderColors& );
    QskBoxBorderColors boxBorderColors( QskAspect::Aspect ) const;

    void setInterval( QskAspect::Aspect, const QskIntervalF& );
    QskIntervalF interval( QskAspect::Aspect ) const;

    void setSpacing( QskAspect::Aspect, qreal );
    qreal spacing( QskAspect::Aspect ) const;

    void setAlignment( QskAspect::Aspect, Qt::Alignment );
    Qt::Alignment alignment( QskAspect::Aspect ) const;

    void setAnimation( QskAspect::Aspect, QskAnimationHint animation );
    QskAnimationHint animation( QskAspect::Aspect ) const;

    void setGraphicRole( QskAspect::Aspect, int role );
    void setFontRole( QskAspect::Aspect, int role );
    void setFlagHint( QskAspect::Aspect, int flag );

    void setHint( QskAspect::Aspect, const QVariant& );
    const QVariant& hint( QskAspect::Aspect ) const;

    bool removeHint( QskAspect::Aspect );
    QVariant takeHint( QskAspect::Aspect );

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
    return hint( aspect | QskAspect::Color ).value< QColor >();
}

inline void QskSkinHintTable::setMetric( QskAspect::Aspect aspect, qreal metric )
{
    setHint( aspect | QskAspect::Metric, metric );
}

inline qreal QskSkinHintTable::metric( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Metric ).toReal();
}

inline void QskSkinHintTable::setStrutSize(
    QskAspect::Aspect aspect, const QSizeF& size )
{
    const auto aspectStrut = aspect | QskAspect::Metric | QskAspect::StrutSize;
    setHint( aspectStrut, QVariant::fromValue( size ) );
}

inline QSizeF QskSkinHintTable::strutSize( QskAspect::Aspect aspect ) const
{
    const auto aspectStrut = aspect | QskAspect::Metric | QskAspect::StrutSize;
    return hint( aspectStrut ).value< QSizeF >();
}

inline void QskSkinHintTable::setMargin(
    QskAspect::Aspect aspect, const QskMargins& margins )
{
    const auto aspectMargin = aspect | QskAspect::Metric | QskAspect::Margin;
    setHint( aspectMargin, QVariant::fromValue( margins ) );
}

inline QskMargins QskSkinHintTable::margin( QskAspect::Aspect aspect ) const
{
    const auto aspectMargin = aspect | QskAspect::Metric | QskAspect::Margin;
    return hint( aspectMargin ).value< QskMargins >();
}

inline void QskSkinHintTable::setPadding(
    QskAspect::Aspect aspect, const QskMargins& padding )
{
    const auto aspectPadding = aspect | QskAspect::Metric | QskAspect::Padding;
    setHint( aspectPadding, QVariant::fromValue( padding ) );
}

inline QskMargins QskSkinHintTable::padding( QskAspect::Aspect aspect ) const
{
    const auto aspectPadding = aspect | QskAspect::Metric | QskAspect::Padding;
    return hint( aspectPadding ).value< QskMargins >();
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

inline void QskSkinHintTable::setInterval(
    QskAspect::Aspect aspect, const QskIntervalF& interval )
{
    setHint( aspect | QskAspect::Metric, QVariant::fromValue( interval ) );
}

inline QskIntervalF QskSkinHintTable::interval( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Metric ).value< QskIntervalF >();
}

inline void QskSkinHintTable::setSpacing( QskAspect::Aspect aspect, qreal spacing )
{
    setMetric( aspect | QskAspect::Spacing, spacing );
}

inline qreal QskSkinHintTable::spacing( QskAspect::Aspect aspect ) const
{
    return metric( aspect | QskAspect::Spacing );
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

inline void QskSkinHintTable::setAlignment(
    QskAspect::Aspect aspect, Qt::Alignment alignment )
{
    setFlagHint( aspect | QskAspect::Alignment, alignment );
}

inline Qt::Alignment QskSkinHintTable::alignment( QskAspect::Aspect aspect ) const
{
    return hint( aspect | QskAspect::Alignment ).value< Qt::Alignment >();
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
