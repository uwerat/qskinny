/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_EDITOR_H
#define QSK_SKIN_HINT_TABLE_EDITOR_H

#include "QskAspect.h"
#include "QskSkinHintTable.h"
#include "QskAnimationHint.h"
#include "QskStateCombination.h"
#include "QskTextOptions.h"

#include <qcolor.h>
#include <qvariant.h>

class QskArcMetrics;
class QskMargins;
class QskGradient;
class QskBoxShapeMetrics;
class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskShadowMetrics;

class QSK_EXPORT QskSkinHintTableEditor
{
  public:
    QskSkinHintTableEditor( QskSkinHintTable* = nullptr );

    void setTable( QskSkinHintTable* );
    QskSkinHintTable* table() const;

    // generic access

    void setHint( QskAspect, const QVariant&,
        QskStateCombination = QskStateCombination() );

    template< typename T > void setHint(
        QskAspect, const T&, QskStateCombination = QskStateCombination() );

    const QVariant& hint( QskAspect ) const;
    template< typename T > T hint( QskAspect ) const;

    bool removeHint( QskAspect, QskStateCombination = QskStateCombination() );
    QVariant takeHint( QskAspect );

    bool hasHint( QskAspect ) const;

    // animation hints

    void setAnimation( QskAspect, QskAnimationHint,
        QskStateCombination = QskStateCombination() );

    void setAnimation( QskAspect, uint duration, QEasingCurve::Type,
        QskStateCombination = QskStateCombination() );

    QskAnimationHint animation( QskAspect ) const;

    // flag

    void setFlagHint( QskAspect, const QVariant&,
        QskStateCombination = QskStateCombination() );

    template< typename T > void setFlagHint( QskAspect, const T&,
        QskStateCombination = QskStateCombination() );

    bool removeFlagHint( QskAspect, QskStateCombination = QskStateCombination() );

    QVariant flagHint( QskAspect ) const;
    template< typename T > T flagHint( QskAspect ) const;

    // metric

    void setMetricHint( QskAspect, const QVariant&,
        QskStateCombination = QskStateCombination() );

    template< typename T > void setMetricHint(
        QskAspect, const T&, QskStateCombination = QskStateCombination() );

    bool removeMetricHint( QskAspect, QskStateCombination = QskStateCombination() );

    QVariant metricHint( QskAspect ) const;
    template< typename T > T metricHint( QskAspect ) const;

    // color

    void setColorHint( QskAspect, const QVariant&,
        QskStateCombination = QskStateCombination() );

    template< typename T > void setColorHint(
        QskAspect, const T&, QskStateCombination = QskStateCombination() );

    bool removeColorHint( QskAspect, QskStateCombination = QskStateCombination() );

    QVariant colorHint( QskAspect ) const;
    template< typename T > T colorHint( QskAspect ) const;

    // type specific

    void setFlag( QskAspect, int, QskStateCombination = QskStateCombination() );
    int flag( QskAspect ) const;

    void setMetric( QskAspect, qreal metric, QskStateCombination = QskStateCombination() );
    qreal metric( QskAspect ) const;

    void setColor( QskAspect, Qt::GlobalColor, QskStateCombination = QskStateCombination() );
    void setColor( QskAspect, QRgb, QskStateCombination = QskStateCombination() );
    void setColor( QskAspect, const QColor&, QskStateCombination = QskStateCombination() );

    QColor color( QskAspect ) const;

    // gradients

    void setHGradient( QskAspect, const QColor&, const QColor&,
        QskStateCombination = QskStateCombination() );

    void setVGradient( QskAspect, const QColor&, const QColor&,
        QskStateCombination = QskStateCombination() );

    void setGradient( QskAspect, const QColor&, const QColor&,
        QskStateCombination = QskStateCombination() );

    void setGradient( QskAspect, const QskGradient&,
        QskStateCombination = QskStateCombination() );

    QskGradient gradient( QskAspect ) const;

    // position

    void setPosition( QskAspect, qreal,
        QskStateCombination = QskStateCombination() );

    bool removePosition( QskAspect, QskStateCombination = QskStateCombination() );

    qreal position( QskAspect ) const;

    // strutSize

    void setStrutSize( QskAspect, const QSizeF&,
        QskStateCombination = QskStateCombination() );

    void setStrutSize( QskAspect, qreal width, qreal height,
        QskStateCombination = QskStateCombination() );

    bool removeStrutSize( QskAspect, QskStateCombination = QskStateCombination() );

    QSizeF strutSize( QskAspect ) const;

    // margin

    void setMargin( QskAspect, const QskMargins&,
        QskStateCombination = QskStateCombination() );

    void setMargin( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom,
        QskStateCombination = QskStateCombination() );

    bool removeMargin( QskAspect, QskStateCombination = QskStateCombination() );

    QskMargins margin( QskAspect ) const;

    // padding

    void setPadding( QskAspect, const QskMargins&,
        QskStateCombination = QskStateCombination() );

    void setPadding( QskAspect,
        qreal left, qreal top, qreal right, qreal bottom,
        QskStateCombination = QskStateCombination() );

    bool removePadding( QskAspect, QskStateCombination = QskStateCombination() );

    QskMargins padding( QskAspect ) const;

    // spacing

    void setSpacing( QskAspect, qreal, QskStateCombination = QskStateCombination() );
    bool removeSpacing( QskAspect, QskStateCombination = QskStateCombination() );
    qreal spacing( QskAspect ) const;

    // alignment

    void setAlignment( QskAspect, Qt::Alignment, QskStateCombination = QskStateCombination() );
    bool removeAlignment( QskAspect, QskStateCombination = QskStateCombination() );
    Qt::Alignment alignment( QskAspect ) const;

    // fontRole

    void setFontRole( QskAspect, int, QskStateCombination = QskStateCombination() );
    bool removeFontRole( QskAspect, QskStateCombination = QskStateCombination() );
    int fontRole( QskAspect ) const;

    // graphicRole

    void setGraphicRole( QskAspect, int, QskStateCombination = QskStateCombination() );
    bool removeGraphicRole( QskAspect, QskStateCombination = QskStateCombination() );
    int graphicRole( QskAspect ) const;

    // boxShape

    void setBoxShape( QskAspect, qreal radius, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxShape( QskAspect, qreal topLeft, qreal topRight,
        qreal bottomLeft, qreal bottomRight, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxShape( QskAspect, const QskBoxShapeMetrics&,
        QskStateCombination = QskStateCombination() );

    bool removeBoxShape( QskAspect, QskStateCombination = QskStateCombination() );

    QskBoxShapeMetrics boxShape( QskAspect ) const;

    // boxBorderMetrics

    void setBoxBorderMetrics( QskAspect,
        qreal borderWidth, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect, qreal left, qreal top,
        qreal right, qreal bottom, Qt::SizeMode = Qt::AbsoluteSize );

    void setBoxBorderMetrics( QskAspect,
        const QskBoxBorderMetrics&, QskStateCombination = QskStateCombination() );

    bool removeBoxBorderMetric( QskAspect, QskStateCombination = QskStateCombination() );

    QskBoxBorderMetrics boxBorderMetrics( QskAspect ) const;

    // boxBorderColors

    void setBoxBorderColors( QskAspect,
        const QskBoxBorderColors&, QskStateCombination = QskStateCombination() );

    void setBoxBorderColors( QskAspect,
        const QskGradient& left, const QskGradient& top,
        const QskGradient& right, const QskGradient& bottom,
        QskStateCombination = QskStateCombination() );

    bool removeBoxBorderColors( QskAspect, QskStateCombination = QskStateCombination() );
    QskBoxBorderColors boxBorderColors( QskAspect ) const;

    // shadowMetrics

    void setShadowMetrics( QskAspect, qreal, qreal,
        const QPointF&, Qt::SizeMode = Qt::AbsoluteSize );

    void setShadowMetrics( QskAspect, const QskShadowMetrics&,
        QskStateCombination = QskStateCombination() );

    bool removeShadowMetric( QskAspect, QskStateCombination = QskStateCombination() );

    QskShadowMetrics shadowMetrics( QskAspect ) const;

    // shadowColor

    void setShadowColor( QskAspect,
        const QColor&, QskStateCombination = QskStateCombination() );

    void setShadowColor( QskAspect,
        Qt::GlobalColor, QskStateCombination = QskStateCombination() );

    void setShadowColor( QskAspect,
        QRgb, QskStateCombination = QskStateCombination() );

    bool removeShadowColor( QskAspect, QskStateCombination = QskStateCombination() );
    QColor shadowColor( QskAspect ) const;

    // arcMetrics

    void setArcMetrics( QskAspect, qreal, qreal, qreal, Qt::SizeMode = Qt::AbsoluteSize );

    void setArcMetrics( QskAspect,
        const QskArcMetrics&, QskStateCombination = QskStateCombination() );

    bool removeArcMetrics( QskAspect, QskStateCombination = QskStateCombination() );

    QskArcMetrics arcMetrics( QskAspect ) const;

    // text options flag

    void setTextOptions( QskAspect,
        Qt::TextElideMode, QskTextOptions::WrapMode,
        QskStateCombination = QskStateCombination() );

    void setTextOptions( QskAspect,
        const QskTextOptions&, QskStateCombination = QskStateCombination() );

    bool removeTextOptions( QskAspect, QskStateCombination = QskStateCombination() );
    QskTextOptions textOptions( QskAspect ) const;

  private:
    QskSkinHintTable* m_table = nullptr;
};

// --- generic access ---

template< typename T >
inline void QskSkinHintTableEditor::setHint(
    QskAspect aspect, const T& hint, QskStateCombination combination )
{
    setHint( aspect, QVariant::fromValue( hint ), combination );
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
    QskAspect aspect, const QVariant& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Flag, hint, combination );
}

template< typename T >
inline void QskSkinHintTableEditor::setFlagHint(
    QskAspect aspect, const T& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Flag, hint, combination );
}

inline bool QskSkinHintTableEditor::removeFlagHint(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspect | QskAspect::Flag, combination );
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
    QskAspect aspect, const QVariant& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Metric, hint, combination );
}

template< typename T >
inline void QskSkinHintTableEditor::setMetricHint(
    QskAspect aspect, const T& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Metric, hint, combination );
}

inline bool QskSkinHintTableEditor::removeMetricHint(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspect | QskAspect::Metric, combination );
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
    QskAspect aspect, const QVariant& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Color, hint, combination );
}

template< typename T >
inline void QskSkinHintTableEditor::setColorHint(
    QskAspect aspect, const T& hint, QskStateCombination combination )
{
    setHint( aspect | QskAspect::Color, hint, combination );
}

inline bool QskSkinHintTableEditor::removeColorHint(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspect | QskAspect::Color, combination );
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
    QskAspect aspect, QskAnimationHint hint, QskStateCombination combination )
{
    aspect.setAnimator( true );
    setHint( aspect, hint, combination );
}

inline void QskSkinHintTableEditor::setAnimation( QskAspect aspect,
    uint duration, QEasingCurve::Type curveType, QskStateCombination combination )
{
    setAnimation( aspect, QskAnimationHint( duration, curveType ), combination );
}

#endif
