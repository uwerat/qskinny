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

#include <qcolor.h>
#include <qvariant.h>

#include <unordered_map>

class QSK_EXPORT QskSkinHintTable
{
  public:
    QskSkinHintTable();
    QskSkinHintTable( const QskSkinHintTable& other );

    ~QskSkinHintTable();

    QskSkinHintTable& operator=( const QskSkinHintTable& );

    void setColor( QskAspect, Qt::GlobalColor );
    void setColor( QskAspect, QRgb );

    void setColor( QskAspect, const QColor& );
    QColor color( QskAspect ) const;

    void setMetric( QskAspect, qreal );
    qreal metric( QskAspect ) const;

    void setStrutSize( QskAspect, const QSizeF& );
    QSizeF strutSize( QskAspect ) const;

    void setMargin( QskAspect, const QskMargins& );
    QskMargins margin( QskAspect ) const;

    void setPadding( QskAspect, const QskMargins& );
    QskMargins padding( QskAspect ) const;

    void setGradient( QskAspect, const QskGradient& );
    QskGradient gradient( QskAspect ) const;

    void setBoxShape( QskAspect, const QskBoxShapeMetrics& );
    QskBoxShapeMetrics boxShape( QskAspect ) const;

    void setBoxBorder( QskAspect, const QskBoxBorderMetrics& );
    QskBoxBorderMetrics boxBorder( QskAspect ) const;

    void setBoxBorderColors( QskAspect, const QskBoxBorderColors& );
    QskBoxBorderColors boxBorderColors( QskAspect ) const;

    void setSpacing( QskAspect, qreal );
    qreal spacing( QskAspect ) const;

    void setAlignment( QskAspect, Qt::Alignment );
    Qt::Alignment alignment( QskAspect ) const;

    void setAnimation( QskAspect, QskAnimationHint );
    QskAnimationHint animation( QskAspect ) const;

    void setGraphicRole( QskAspect, int role );
    void setFontRole( QskAspect, int role );
    void setFlagHint( QskAspect, int flag );

    void setHint( QskAspect, const QVariant& );
    const QVariant& hint( QskAspect ) const;

    bool removeHint( QskAspect );
    QVariant takeHint( QskAspect );

    bool hasHint( QskAspect ) const;

    const std::unordered_map< QskAspect, QVariant >& hints() const;

    bool hasAnimators() const;
    bool hasStates() const;
    bool hasHints() const;

    void clear();

    const QVariant* resolvedHint( QskAspect,
        QskAspect* resolvedAspect = nullptr ) const;

    QskAspect resolvedAspect( QskAspect ) const;

    QskAspect resolvedAnimator(
        QskAspect, QskAnimationHint& ) const;

  private:
    static QVariant invalidHint;

    typedef std::unordered_map< QskAspect, QVariant > HintMap;
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

inline bool QskSkinHintTable::hasHint( QskAspect aspect ) const
{
    if ( m_hints != nullptr )
        return m_hints->find( aspect ) != m_hints->cend();

    return false;
}

inline const QVariant& QskSkinHintTable::hint( QskAspect aspect ) const
{
    if ( m_hints != nullptr )
    {
        auto it = m_hints->find( aspect );
        if ( it != m_hints->cend() )
            return it->second;
    }

    return invalidHint;
}

inline void QskSkinHintTable::setColor( QskAspect aspect, const QColor& color )
{
    setHint( aspect | QskAspect::Color, color );
}

inline void QskSkinHintTable::setColor( QskAspect aspect, Qt::GlobalColor color )
{
    setHint( aspect | QskAspect::Color, QColor( color ) );
}

inline void QskSkinHintTable::setColor( QskAspect aspect, QRgb rgb )
{
    setHint( aspect | QskAspect::Color, QColor::fromRgba( rgb ) );
}

inline QColor QskSkinHintTable::color( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Color ).value< QColor >();
}

inline void QskSkinHintTable::setMetric( QskAspect aspect, qreal metric )
{
    setHint( aspect | QskAspect::Metric, metric );
}

inline qreal QskSkinHintTable::metric( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Metric ).toReal();
}

inline void QskSkinHintTable::setStrutSize( QskAspect aspect, const QSizeF& size )
{
    const auto aspectStrut = aspect | QskAspect::Metric | QskAspect::StrutSize;
    setHint( aspectStrut, QVariant::fromValue( size ) );
}

inline QSizeF QskSkinHintTable::strutSize( QskAspect aspect ) const
{
    const auto aspectStrut = aspect | QskAspect::Metric | QskAspect::StrutSize;
    return hint( aspectStrut ).value< QSizeF >();
}

inline void QskSkinHintTable::setMargin(
    QskAspect aspect, const QskMargins& margins )
{
    const auto aspectMargin = aspect | QskAspect::Metric | QskAspect::Margin;
    setHint( aspectMargin, QVariant::fromValue( margins ) );
}

inline QskMargins QskSkinHintTable::margin( QskAspect aspect ) const
{
    const auto aspectMargin = aspect | QskAspect::Metric | QskAspect::Margin;
    return hint( aspectMargin ).value< QskMargins >();
}

inline void QskSkinHintTable::setPadding(
    QskAspect aspect, const QskMargins& padding )
{
    const auto aspectPadding = aspect | QskAspect::Metric | QskAspect::Padding;
    setHint( aspectPadding, QVariant::fromValue( padding ) );
}

inline QskMargins QskSkinHintTable::padding( QskAspect aspect ) const
{
    const auto aspectPadding = aspect | QskAspect::Metric | QskAspect::Padding;
    return hint( aspectPadding ).value< QskMargins >();
}

inline void QskSkinHintTable::setGradient(
    QskAspect aspect, const QskGradient& gradient )
{
    setHint( aspect | QskAspect::Color, QVariant::fromValue( gradient ) );
}

inline QskGradient QskSkinHintTable::gradient( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Color ).value< QskGradient >();
}

inline void QskSkinHintTable::setBoxShape(
    QskAspect aspect, const QskBoxShapeMetrics& shape )
{
    using A = QskAspect;
    setHint( aspect | A::Shape | A::Metric, QVariant::fromValue( shape ) );
}

inline QskBoxShapeMetrics QskSkinHintTable::boxShape( QskAspect aspect ) const
{
    using A = QskAspect;
    return hint( aspect | A::Shape | A::Metric ).value< QskBoxShapeMetrics >();
}

inline void QskSkinHintTable::setBoxBorder(
    QskAspect aspect, const QskBoxBorderMetrics& border )
{
    using A = QskAspect;
    setHint( aspect | A::Border | A::Metric, QVariant::fromValue( border ) );
}

inline QskBoxBorderMetrics QskSkinHintTable::boxBorder( QskAspect aspect ) const
{
    using A = QskAspect;
    return hint( aspect | A::Border | A::Metric ).value< QskBoxBorderMetrics >();
}

inline void QskSkinHintTable::setBoxBorderColors(
    QskAspect aspect, const QskBoxBorderColors& colors )
{
    using A = QskAspect;
    setHint( aspect | A::Border | A::Color, QVariant::fromValue( colors ) );
}

inline QskBoxBorderColors QskSkinHintTable::boxBorderColors( QskAspect aspect ) const
{
    using A = QskAspect;
    return hint( aspect | A::Border | A::Color ).value< QskBoxBorderColors >();
}

inline void QskSkinHintTable::setSpacing( QskAspect aspect, qreal spacing )
{
    setMetric( aspect | QskAspect::Spacing, spacing );
}

inline qreal QskSkinHintTable::spacing( QskAspect aspect ) const
{
    return metric( aspect | QskAspect::Spacing );
}

inline QskAnimationHint QskSkinHintTable::animation( QskAspect aspect ) const
{
    aspect.setAnimator( true );
    return hint( aspect ).value< QskAnimationHint >();
}

inline void QskSkinHintTable::setGraphicRole( QskAspect aspect, int role )
{
    setHint( aspect | QskAspect::GraphicRole, role );
}

inline void QskSkinHintTable::setFontRole( QskAspect aspect, int role )
{
    setHint( aspect | QskAspect::FontRole, role );
}

inline void QskSkinHintTable::setAlignment(
    QskAspect aspect, Qt::Alignment alignment )
{
    setFlagHint( aspect | QskAspect::Alignment, alignment );
}

inline Qt::Alignment QskSkinHintTable::alignment( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Alignment ).value< Qt::Alignment >();
}

inline void QskSkinHintTable::setFlagHint( QskAspect aspect, int flag )
{
    setHint( aspect, QVariant( flag ) );
}

inline void QskSkinHintTable::setAnimation(
    QskAspect aspect, QskAnimationHint animation )
{
    aspect.setAnimator( true );
    setHint( aspect, QVariant::fromValue( animation ) );
}

#endif
