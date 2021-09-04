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

    void setHint( QskAspect, const QVariant&,
        QskAspect::State = QskAspect::NoState );

    template< typename T > void setHint(
        QskAspect, const T&, QskAspect::State = QskAspect::NoState );

    const QVariant& hint( QskAspect ) const;
    template< typename T > T hint( QskAspect ) const;

    bool removeHint( QskAspect, QskAspect::State = QskAspect::NoState );
    QVariant takeHint( QskAspect );

    bool hasHint( QskAspect ) const;

    // animation hints

    void setAnimation( QskAspect, QskAnimationHint,
        QskAspect::State = QskAspect::NoState );

    QskAnimationHint animation( QskAspect ) const;

    // flag

    void setFlagHint( QskAspect, const QVariant&,
        QskAspect::State = QskAspect::NoState );

    template< typename T > void setFlagHint( QskAspect, const T&,
        QskAspect::State = QskAspect::NoState );

    void removeFlagHint( QskAspect, QskAspect::State = QskAspect::NoState );

    QVariant flagHint( QskAspect ) const;
    template< typename T > T flagHint( QskAspect ) const;

    // metric

    void setMetricHint( QskAspect, const QVariant&,
        QskAspect::State = QskAspect::NoState );

    template< typename T > void setMetricHint(
        QskAspect, const T&, QskAspect::State = QskAspect::NoState );

    void removeMetricHint( QskAspect, QskAspect::State = QskAspect::NoState );

    QVariant metricHint( QskAspect ) const;
    template< typename T > T metricHint( QskAspect ) const;

    // color

    void setColorHint( QskAspect, const QVariant&,
        QskAspect::State = QskAspect::NoState );

    template< typename T > void setColorHint(
        QskAspect, const T&, QskAspect::State = QskAspect::NoState );

    void removeColorHint( QskAspect, QskAspect::State = QskAspect::NoState );

    QVariant colorHint( QskAspect ) const;
    template< typename T > T colorHint( QskAspect ) const;

    // type specific

    void setFlag( QskAspect, int, QskAspect::State = QskAspect::NoState );
    int flag( QskAspect ) const;

    void setMetric( QskAspect, qreal metric, QskAspect::State = QskAspect::NoState );
    qreal metric( QskAspect ) const;

    void setColor( QskAspect, Qt::GlobalColor, QskAspect::State = QskAspect::NoState );
    void setColor( QskAspect, QRgb, QskAspect::State = QskAspect::NoState );
    void setColor( QskAspect, const QColor&, QskAspect::State = QskAspect::NoState );

    QColor color( QskAspect ) const;

    // gradients

    void setHGradient( QskAspect, const QColor&, const QColor&,
        QskAspect::State = QskAspect::NoState );

    void setVGradient( QskAspect, const QColor&, const QColor&,
        QskAspect::State = QskAspect::NoState );

    void setGradient( QskAspect, const QskGradient&,
        QskAspect::State = QskAspect::NoState );

    QskGradient gradient( QskAspect ) const;

    // strutSize

    void setStrutSize( QskAspect, const QSizeF&,
        QskAspect::State = QskAspect::NoState );

    void setStrutSize( QskAspect, qreal width, qreal height,
        QskAspect::State = QskAspect::NoState );

    void removeStrutSize( QskAspect, QskAspect::State = QskAspect::NoState );

    QSizeF strutSize( QskAspect ) const;

    // margin

    void setMargin( QskAspect, const QskMargins&,
        QskAspect::State = QskAspect::NoState );

    void setMargin( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom,
        QskAspect::State = QskAspect::NoState );

    void removeMargin( QskAspect, QskAspect::State = QskAspect::NoState );

    QskMargins margin( QskAspect ) const;

    // padding

    void setPadding( QskAspect, const QskMargins&,
        QskAspect::State = QskAspect::NoState );

    void setPadding( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom,
        QskAspect::State = QskAspect::NoState );

    void removePadding( QskAspect, QskAspect::State = QskAspect::NoState );

    QskMargins padding( QskAspect ) const;

    // spacing

    void setSpacing( QskAspect, qreal, QskAspect::State = QskAspect::NoState );
    void removeSpacing( QskAspect, QskAspect::State = QskAspect::NoState );
    qreal spacing( QskAspect ) const;

    // alignment

    void setAlignment( QskAspect, Qt::Alignment, QskAspect::State = QskAspect::NoState );
    void removeAlignment( QskAspect, QskAspect::State = QskAspect::NoState );
    Qt::Alignment alignment( QskAspect ) const;

    // fontRole

    void setFontRole( QskAspect, int, QskAspect::State = QskAspect::NoState );
    void removeFontRole( QskAspect, QskAspect::State = QskAspect::NoState );
    int fontRole( QskAspect ) const;

    // graphicRole

    void setGraphicRole( QskAspect, int, QskAspect::State = QskAspect::NoState );
    void removeGraphicRole( QskAspect, QskAspect::State = QskAspect::NoState );
    int graphicRole( QskAspect ) const;

    // boxShape

    void setBoxShape( QskAspect, qreal radius, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxShape( QskAspect, qreal topLeft, qreal topRight,
        qreal bottomLeft, qreal bottomRight, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxShape( QskAspect, const QskBoxShapeMetrics&,
        QskAspect::State = QskAspect::NoState );

    void removeBoxShape( QskAspect, QskAspect::State = QskAspect::NoState );

    QskBoxShapeMetrics boxShape( QskAspect ) const;

    // boxBorderMetrics

    void setBoxBorderMetrics( QskAspect,
        qreal borderWidth, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, qreal left, qreal top,
        qreal right, qreal bottom, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect,
        const QskBoxBorderMetrics&, QskAspect::State = QskAspect::NoState );

    void removeBoxBorderMetric( QskAspect, QskAspect::State = QskAspect::NoState );

    QskBoxBorderMetrics boxBorderMetrics( QskAspect ) const;

    // boxBorderColors

    void setBoxBorderColors( QskAspect,
        const QskBoxBorderColors&, QskAspect::State = QskAspect::NoState );

    void setBoxBorderColors( QskAspect,
        const QColor& left, const QColor& top, const QColor& right, const QColor& bottom,
        QskAspect::State = QskAspect::NoState );

    void removeBoxBorderColors( QskAspect, QskAspect::State = QskAspect::NoState );
    QskBoxBorderColors boxBorderColors( QskAspect ) const;

  private:
    QskSkinHintTable* m_table = nullptr;
};

// --- generic access ---

template< typename T >
inline void QskSkinHintTableEditor::setHint(
    QskAspect aspect, const T& hint, QskAspect::State combinationMask )
{
    setHint( aspect, QVariant::fromValue( hint ), combinationMask );
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

inline QVariant QskSkinHintTableEditor::takeHint( QskAspect aspect )
{
    return m_table->takeHint( aspect );
}

inline bool QskSkinHintTableEditor::hasHint( QskAspect aspect ) const
{
    return m_table->hasHint( aspect );
}

// --- flag ---

inline void QskSkinHintTableEditor::setFlagHint(
    QskAspect aspect, const QVariant& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Flag, hint, combinationMask );
}

template< typename T >
inline void QskSkinHintTableEditor::setFlagHint(
    QskAspect aspect, const T& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Flag, hint, combinationMask );
}

inline void QskSkinHintTableEditor::removeFlagHint(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeHint( aspect | QskAspect::Flag, combinationMask );
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

inline void QskSkinHintTableEditor::setMetricHint(
    QskAspect aspect, const QVariant& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Metric, hint, combinationMask );
}

template< typename T >
inline void QskSkinHintTableEditor::setMetricHint(
    QskAspect aspect, const T& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Metric, hint, combinationMask );
}

inline void QskSkinHintTableEditor::removeMetricHint(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeHint( aspect | QskAspect::Metric, combinationMask );
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

inline void QskSkinHintTableEditor::setColorHint(
    QskAspect aspect, const QVariant& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Color, hint, combinationMask );
}

template< typename T >
inline void QskSkinHintTableEditor::setColorHint(
    QskAspect aspect, const T& hint, QskAspect::State combinationMask )
{
    setHint( aspect | QskAspect::Color, hint, combinationMask );
}

inline void QskSkinHintTableEditor::removeColorHint(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeHint( aspect | QskAspect::Color, combinationMask );
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

inline void QskSkinHintTableEditor::setAnimation(
    QskAspect aspect, QskAnimationHint hint, QskAspect::State combinationMask )
{
    aspect.setAnimator( true );
    setHint( aspect, hint, combinationMask );
}

#endif
