/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_EDITOR_H
#define QSK_SKIN_HINT_TABLE_EDITOR_H

#include "QskAspect.h"
#include "QskSkinHintTable.h"
#include "QskAnimationHint.h"

#include <qcolor.h>
#include <qvariant.h>

class QskMargins;
class QskGradient;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;

class QSK_EXPORT QskSkinHintTableEditor
{
  public:
    QskSkinHintTableEditor( QskSkinHintTable* = nullptr );

    void setTable( QskSkinHintTable* );
    QskSkinHintTable* table() const;

    // generic access

    void setHint( QskAspect, const QVariant& );
    const QVariant& hint( QskAspect ) const;

    void setHintForAllStateCombinations(
        QskAspect::State, QskAspect, const QVariant& );

    template< typename T > void setHintForAllStateCombinations(
        QskAspect::State, QskAspect, const T& );

    bool removeHint( QskAspect );
    bool removeHintForAllStateCombinations( QskAspect::State, QskAspect );

    QVariant takeHint( QskAspect );

    bool hasHint( QskAspect ) const;

    template< typename T > void setHint( QskAspect, const T& );
    template< typename T > T hint( QskAspect ) const;

    // animation hints

    void setAnimation( QskAspect, QskAnimationHint );
    QskAnimationHint animation( QskAspect ) const;

    void setAnimationForAllStateCombinations(
        QskAspect::State, QskAspect, QskAnimationHint );

    // flag

    void setFlagHint( QskAspect, const QVariant& );
    template< typename T > void setFlagHint( QskAspect, const T& );

    void setFlagHintForAllStateCombinations(
        QskAspect::State, QskAspect, const QVariant& );

    template< typename T > void setFlagHintForAllStateCombinations(
        QskAspect::State, QskAspect, const T& );

    void removeFlagHint( QskAspect );

    QVariant flagHint( QskAspect ) const;
    template< typename T > T flagHint( QskAspect ) const;

    // metric

    void setMetricHint( QskAspect, const QVariant& );
    template< typename T > void setMetricHint( QskAspect, const T& );

    void setMetricHintForAllStateCombinations(
        QskAspect::State, QskAspect, const QVariant& );

    template< typename T > void setMetricHintForAllStateCombinations(
        QskAspect::State, QskAspect, const T& );

    void removeMetricHint( QskAspect );

    QVariant metricHint( QskAspect ) const;
    template< typename T > T metricHint( QskAspect ) const;

    // color

    void setColorHint( QskAspect, const QVariant& );
    template< typename T > void setColorHint( QskAspect, const T& );

    void setColorHintForAllStateCombinations(
        QskAspect::State, QskAspect, const QVariant& );

    template< typename T > void setColorHintForAllStateCombinations(
        QskAspect::State, QskAspect, const T& );

    void removeColorHint( QskAspect );

    QVariant colorHint( QskAspect ) const;
    template< typename T > T colorHint( QskAspect ) const;

    // type specific

    void setFlag( QskAspect, int );
    int flag( QskAspect ) const;

    void setFlagForAllStateCombinations( QskAspect::State, QskAspect, int );

    void setMetric( QskAspect, qreal metric );
    qreal metric( QskAspect ) const;

    void setMetricForAllStateCombinations( QskAspect::State, QskAspect, qreal );

    void setColor( QskAspect, Qt::GlobalColor );
    void setColor( QskAspect, QRgb );
    void setColor( QskAspect, const QColor& );

    void setColorForAllStateCombinations( QskAspect::State, QskAspect, Qt::GlobalColor );
    void setColorForAllStateCombinations( QskAspect::State, QskAspect, QRgb );
    void setColorForAllStateCombinations( QskAspect::State, QskAspect, const QColor& );

    QColor color( QskAspect ) const;

    // gradients

    void setHGradient( QskAspect, const QColor&, const QColor& );
    void setVGradient( QskAspect, const QColor&, const QColor& );
    void setGradient( QskAspect, const QskGradient& );

    void setGradientForAllStateCombinations(
        QskAspect::State, QskAspect, const QskGradient& );

    QskGradient gradient( QskAspect ) const;

    // strutSize

    void setStrutSize( QskAspect, const QSizeF& );
    void setStrutSize( QskAspect, qreal width, qreal height );

    void removeStrutSize( QskAspect );

    void setStrutSizeForAllStateCombinations(
        QskAspect::State, QskAspect, const QSizeF& );

    QSizeF strutSize( QskAspect ) const;

    // margin

    void setMargin( QskAspect, const QskMargins& );
    void setMargin( QskAspect, qreal left, qreal top, qreal right, qreal bottom );

    void setMarginForAllStateCombinations(
        QskAspect::State, QskAspect, const QskMargins& );

    void removeMargin( QskAspect );

    QskMargins margin( QskAspect ) const;

    // padding

    void setPadding( QskAspect, const QskMargins& );
    void setPadding( QskAspect, qreal left, qreal top, qreal right, qreal bottom );

    void setPaddingForAllStateCombinations(
        QskAspect::State, QskAspect, const QskMargins& );

    void removePadding( QskAspect );

    QskMargins padding( QskAspect ) const;

    // spacing

    void setSpacing( QskAspect, qreal );
    void removeSpacing( QskAspect );
    qreal spacing( QskAspect ) const;

    // alignment

    void setAlignment( QskAspect, Qt::Alignment );
    void removeAlignment( QskAspect );
    Qt::Alignment alignment( QskAspect ) const;

    // fontRole

    void setFontRole( QskAspect, int );
    void removeFontRole( QskAspect );
    int fontRole( QskAspect ) const;

    // graphicRole

    void setGraphicRole( QskAspect, int );
    void removeGraphicRole( QskAspect );
    int graphicRole( QskAspect ) const;

    // boxShape

    void setBoxShape( QskAspect, qreal radius, Qt::SizeMode = Qt::AbsoluteSize );
    void setBoxShape( QskAspect, qreal topLeft, qreal topRight,
        qreal bottomLeft, qreal bottomRight, Qt::SizeMode = Qt::AbsoluteSize );
    void setBoxShape( QskAspect, const QskBoxShapeMetrics& );

    void setBoxShapeForAllStateCombinations(
        QskAspect::State, QskAspect, const QskBoxShapeMetrics& );

    void removeBoxShape( QskAspect );

    QskBoxShapeMetrics boxShape( QskAspect ) const;

    // boxBorderMetrics

    void setBoxBorderMetrics( QskAspect,
        qreal borderWidth, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, qreal left, qreal top,
        qreal right, qreal bottom, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, const QskBoxBorderMetrics& );
    void setBoxBorderMetricsForAllStateCombinations(
        QskAspect::State, QskAspect, const QskBoxBorderMetrics& );

    void removeBoxBorderMetric( QskAspect );

    QskBoxBorderMetrics boxBorderMetrics( QskAspect ) const;

    // boxBorderColors

    void setBoxBorderColors( QskAspect, const QskBoxBorderColors& );
    void setBoxBorderColorsForAllStateCombinations(
        QskAspect::State, QskAspect, const QskBoxBorderColors& );

    void setBoxBorderColors( QskAspect, const QColor& left, const QColor& top,
        const QColor& right, const QColor& bottom );

    void removeBoxBorderColors( QskAspect );
    QskBoxBorderColors boxBorderColors( QskAspect ) const;

  private:
    QskSkinHintTable* m_table = nullptr;
};

// --- generic access ---

inline void QskSkinHintTableEditor::setHint( QskAspect aspect, const QVariant& hint )
{
    m_table->setHint( aspect, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setHint( QskAspect aspect, const T& hint )
{
    setHint( aspect, QVariant::fromValue( hint ) );
}

template< typename T >
inline T QskSkinHintTableEditor::hint( QskAspect aspect ) const
{
    return hint( aspect ).value< T >();
}

inline const QVariant& QskSkinHintTableEditor::hint( QskAspect aspect ) const
{
    return m_table->hint( aspect );
}

inline bool QskSkinHintTableEditor::removeHint( QskAspect aspect )
{
    return m_table->removeHint( aspect );
}

inline QVariant QskSkinHintTableEditor::takeHint( QskAspect aspect )
{
    return m_table->takeHint( aspect );
}

inline bool QskSkinHintTableEditor::hasHint( QskAspect aspect ) const
{
    return m_table->hasHint( aspect );
}

template< typename T >
void QskSkinHintTableEditor::setHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const T& hint )
{
    setHintForAllStateCombinations( stateMask, aspect, QVariant::fromValue( hint ) );
}

// --- flag ---

inline void QskSkinHintTableEditor::setFlagHint( QskAspect aspect, const QVariant& hint )
{
    setHint( aspect | QskAspect::Flag, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setFlagHint( QskAspect aspect, const T& hint )
{
    setHint( aspect | QskAspect::Flag, hint );
}

inline void QskSkinHintTableEditor::setFlagHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const QVariant& hint )
{
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Flag, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setFlagHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const T& hint )
{
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Flag, hint );
}

inline void QskSkinHintTableEditor::removeFlagHint( QskAspect aspect )
{
    removeHint( aspect | QskAspect::Flag );
}

inline QVariant QskSkinHintTableEditor::flagHint( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Flag );
}

template< typename T >
inline T QskSkinHintTableEditor::flagHint( QskAspect aspect ) const
{
    return hint< T >( aspect | QskAspect::Flag );
}

// --- metric ---

inline void QskSkinHintTableEditor::setMetricHint( QskAspect aspect, const QVariant& hint )
{
    setHint( aspect | QskAspect::Metric, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setMetricHint( QskAspect aspect, const T& hint )
{
    setHint( aspect | QskAspect::Metric, hint );
}

inline void QskSkinHintTableEditor::setMetricHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const QVariant& hint )
{
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Metric, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setMetricHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const T& hint )
{
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Metric, hint );
}

inline void QskSkinHintTableEditor::removeMetricHint( QskAspect aspect )
{
    removeHint( aspect | QskAspect::Metric );
}

inline QVariant QskSkinHintTableEditor::metricHint( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Metric );
}

template< typename T >
inline T QskSkinHintTableEditor::metricHint( QskAspect aspect ) const
{
    return hint< T >( aspect | QskAspect::Metric );
}

// --- color ---

inline void QskSkinHintTableEditor::setColorHint( QskAspect aspect, const QVariant& hint )
{
    setHint( aspect | QskAspect::Color, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setColorHint( QskAspect aspect, const T& hint )
{
    setHint( aspect | QskAspect::Color, hint );
}

inline void QskSkinHintTableEditor::setColorHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const QVariant& hint )
{   
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Color, hint );
}

template< typename T >
inline void QskSkinHintTableEditor::setColorHintForAllStateCombinations(
    QskAspect::State stateMask, QskAspect aspect, const T& hint )
{
    setHintForAllStateCombinations( stateMask, aspect | QskAspect::Color, hint );
}

inline void QskSkinHintTableEditor::removeColorHint( QskAspect aspect )
{
    removeHint( aspect | QskAspect::Color );
}

inline QVariant QskSkinHintTableEditor::colorHint( QskAspect aspect ) const
{
    return hint( aspect | QskAspect::Color );
}

template< typename T >
inline T QskSkinHintTableEditor::colorHint( QskAspect aspect ) const
{
    return hint< T >( aspect | QskAspect::Color );
}

// --- animation ---

inline QskAnimationHint QskSkinHintTableEditor::animation( QskAspect aspect ) const
{
    return m_table->animation( aspect );
}

inline void QskSkinHintTableEditor::setAnimation( QskAspect aspect, QskAnimationHint hint )
{
    m_table->setAnimation( aspect, hint );
}

#endif
