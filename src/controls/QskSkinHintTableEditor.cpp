/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinHintTableEditor.h"
#include "QskSkinHintTable.h"

#include "QskArcMetrics.h"
#include "QskMargins.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"

namespace
{
    inline QskAspect::State lowestState( QskAspect::States mask )
    {
        using StateInt = typename std::underlying_type< QskAspect::State >::type;

        const auto count = qCountTrailingZeroBits( static_cast< StateInt >( mask ) );
        return static_cast< QskAspect::State >( 1 << count );
    }

    inline void setHintStateMask(
        QskSkinHintTable* table, QskAspect aspect, const QVariant& hint,
        bool noState, QskAspect::States states, QskAspect::States mask )
    {
        if ( mask == 0 )
        {
            if ( noState || states != 0 )
                table->setHint( aspect | states, hint );

            return;
        }

        const auto stateLow = lowestState( mask );

        mask &= ~stateLow;

        setHintStateMask( table, aspect, hint, noState, states, mask );
        setHintStateMask( table, aspect, hint, noState, states | stateLow, mask );
    }

    inline bool removeHintStateMask(
        QskSkinHintTable* table, QskAspect aspect,
        bool noState, QskAspect::States states, QskAspect::States mask )
    {
        if ( mask == 0 )
        {
            if ( noState || states != 0 )
                return table->removeHint( aspect | states );

            return false;
        }

        const auto stateLow = lowestState( mask );

        mask &= ~stateLow;

        bool ret = removeHintStateMask( table, aspect, noState, states, mask );
        ret |= removeHintStateMask( table, aspect, noState, states | stateLow, mask );

        return ret;
    }

    inline QskAspect aspectPosition( QskAspect aspect )
    {
        return aspect | QskAspect::Position;
    }

    inline QskAspect aspectStrutSize( QskAspect aspect )
    {
        return aspect | QskAspect::StrutSize;
    }

    inline QskAspect aspectMargin( QskAspect aspect )
    {
        return aspect | QskAspect::Margin;
    }

    inline QskAspect aspectPadding( QskAspect aspect )
    {
        return aspect | QskAspect::Padding;
    }

    inline QskAspect aspectSpacing( QskAspect aspect )
    {
        return aspect | QskAspect::Spacing;
    }

    inline QskAspect aspectAlignment( QskAspect aspect )
    {
        return aspect | QskAspect::Alignment;
    }

    inline QskAspect aspectFontRole( QskAspect aspect )
    {
        return aspect | QskAspect::FontRole;
    }

    inline QskAspect aspectGraphicRole( QskAspect aspect )
    {
        return aspect | QskAspect::GraphicRole;
    }

    inline QskAspect aspectShape( QskAspect aspect )
    {
        return aspect | QskAspect::Shape;
    }

    inline QskAspect aspectBorder( QskAspect aspect )
    {
        return aspect | QskAspect::Border;
    }
}

QskSkinHintTableEditor::QskSkinHintTableEditor( QskSkinHintTable* table )
    : m_table( table )
{
}

void QskSkinHintTableEditor::setTable( QskSkinHintTable* table )
{
    m_table = table;
}

QskSkinHintTable* QskSkinHintTableEditor::table() const
{
    return m_table;
}

void QskSkinHintTableEditor::setHint( QskAspect aspect,
    const QVariant& hint, QskStateCombination combination )
{
    if ( combination.isNull() )
    {
        m_table->setHint( aspect, hint );
    }
    else
    {
        const bool noState =
            ( combination.type() == QskStateCombination::CombinationNoState );

        setHintStateMask( m_table, aspect, hint,
            noState, QskAspect::States(), combination.states() );
    }
}

bool QskSkinHintTableEditor::removeHint(
    QskAspect aspect, QskStateCombination combination )
{
    if ( combination.isNull() )
    {
        return m_table->removeHint( aspect );
    }
    else
    {
        const bool noState =
            ( combination.type() == QskStateCombination::CombinationNoState );

        return removeHintStateMask( m_table, aspect,
            noState, QskAspect::States(), combination.states() );
    }
}

void QskSkinHintTableEditor::setFlag(
    QskAspect aspect, int flag, QskStateCombination combination )
{
    setFlagHint( aspect, flag, combination );
}

int QskSkinHintTableEditor::flag( QskAspect aspect ) const
{
    return flagHint< int >( aspect );
}

void QskSkinHintTableEditor::setMetric(
    QskAspect aspect, qreal metric, QskStateCombination combination )
{
    setMetricHint( aspect, metric, combination );
}

qreal QskSkinHintTableEditor::metric( QskAspect aspect ) const
{
    return metricHint< qreal >( aspect );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, Qt::GlobalColor color, QskStateCombination combination )
{
    setColorHint( aspect, QColor( color ), combination );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, QRgb rgb, QskStateCombination combination )
{
    setColorHint( aspect, QColor::fromRgba( rgb ), combination );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, const QColor& color, QskStateCombination combination )
{
    setColorHint( aspect, color, combination );
}

QColor QskSkinHintTableEditor::color( QskAspect aspect ) const
{
    return colorHint< QColor >( aspect );
}

void QskSkinHintTableEditor::setHGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskStateCombination combination )
{
    const QskGradient gradient( QskGradient::Horizontal, color1, color2 );
    setGradient( aspect, gradient, combination );
}

void QskSkinHintTableEditor::setVGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskStateCombination combination )
{
    const QskGradient gradient( QskGradient::Vertical, color1, color2 );
    setGradient( aspect, gradient, combination );
}

void QskSkinHintTableEditor::setGradient(
    QskAspect aspect, const QskGradient& gradient,
    QskStateCombination combination )
{
    setColorHint( aspect, gradient, combination );
}

QskGradient QskSkinHintTableEditor::gradient( QskAspect aspect ) const
{
    return colorHint< QskGradient >( aspect );
}

void QskSkinHintTableEditor::setPosition(
    QskAspect aspect, qreal position, QskStateCombination combination )
{
    setMetricHint( aspectPosition( aspect ), position, combination );
}

void QskSkinHintTableEditor::removePosition(
    QskAspect aspect, QskStateCombination combination )
{
    removeMetricHint( aspectPosition( aspect ), combination );
}

qreal QskSkinHintTableEditor::position( QskAspect aspect ) const
{
    return metricHint< qreal >( aspectPosition( aspect ) );
}

void QskSkinHintTableEditor::setStrutSize(
    QskAspect aspect, const QSizeF& size, QskStateCombination combination )
{
    setMetricHint( aspectStrutSize( aspect ), size, combination );
}

void QskSkinHintTableEditor::setStrutSize(
    QskAspect aspect, qreal width, qreal height, QskStateCombination combination )
{
    setMetricHint( aspectStrutSize( aspect ),
        QSizeF( width, height ), combination );
}

void QskSkinHintTableEditor::removeStrutSize(
    QskAspect aspect, QskStateCombination combination )
{
    removeMetricHint( aspectStrutSize( aspect ), combination );
}

QSizeF QskSkinHintTableEditor::strutSize( QskAspect aspect ) const
{
    return metricHint< QSizeF >( aspectStrutSize( aspect ) );
}

void QskSkinHintTableEditor::setMargin(
    QskAspect aspect, const QskMargins& margins, QskStateCombination combination )
{
    setMetricHint( aspectMargin( aspect ), margins, combination );
}

void QskSkinHintTableEditor::setMargin( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom,
    QskStateCombination combination )
{
    const QskMargins margins( left, top, right, bottom );
    setMetricHint( aspectMargin( aspect ), margins, combination );
}

void QskSkinHintTableEditor::removeMargin(
    QskAspect aspect, QskStateCombination combination )
{
    removeMetricHint( aspectMargin( aspect ), combination );
}

QskMargins QskSkinHintTableEditor::margin( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectMargin( aspect ) );
}

void QskSkinHintTableEditor::setPadding(
    QskAspect aspect, const QskMargins& padding,
    QskStateCombination combination )
{
    setMetricHint( aspectPadding( aspect ), padding, combination );
}

void QskSkinHintTableEditor::setPadding( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom,
    QskStateCombination combination )
{
    QskMargins padding( left, top, right, bottom );
    setMetricHint( aspectPadding( aspect ), padding, combination );
}

void QskSkinHintTableEditor::removePadding(
    QskAspect aspect, QskStateCombination combination )
{
    removeMetricHint( aspectPadding( aspect ), combination );
}

QskMargins QskSkinHintTableEditor::padding( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectPadding( aspect ) );
}

void QskSkinHintTableEditor::setSpacing(
    QskAspect aspect, qreal spacing, QskStateCombination combination )
{
    setMetricHint( aspectSpacing( aspect ), spacing, combination );
}

void QskSkinHintTableEditor::removeSpacing(
    QskAspect aspect, QskStateCombination combination )
{
    removeMetricHint( aspectSpacing( aspect ), combination );
}

qreal QskSkinHintTableEditor::spacing( QskAspect aspect ) const
{
    return metricHint< qreal >( aspectSpacing( aspect ) );
}

void QskSkinHintTableEditor::setAlignment(
    QskAspect aspect, Qt::Alignment alignment, QskStateCombination combination )
{
    setFlagHint( aspectAlignment( aspect ),
        static_cast< int >( alignment ), combination );
}

void QskSkinHintTableEditor::removeAlignment(
    QskAspect aspect, QskStateCombination combination )
{
    return removeFlagHint( aspectAlignment( aspect ), combination );
}

Qt::Alignment QskSkinHintTableEditor::alignment( QskAspect aspect ) const
{
    return flagHint< Qt::Alignment >( aspectAlignment( aspect ) );
}

void QskSkinHintTableEditor::setFontRole(
    QskAspect aspect, int fontRole, QskStateCombination combination )
{
    setFlagHint( aspectFontRole( aspect ), fontRole, combination );
}

void QskSkinHintTableEditor::removeFontRole(
    QskAspect aspect, QskStateCombination combination )
{
    return removeFlagHint( aspectFontRole( aspect ), combination );
}

int QskSkinHintTableEditor::fontRole( QskAspect aspect ) const
{
    return flagHint< int >( aspectFontRole( aspect ) );
}

void QskSkinHintTableEditor::setGraphicRole(
    QskAspect aspect, int graphicRole, QskStateCombination combination )
{
    setFlagHint( aspectGraphicRole( aspect ), graphicRole, combination );
}

void QskSkinHintTableEditor::removeGraphicRole(
    QskAspect aspect, QskStateCombination combination )
{
    return removeFlagHint( aspectGraphicRole( aspect ), combination );
}

int QskSkinHintTableEditor::graphicRole( QskAspect aspect ) const
{
    return flagHint< int >( aspectGraphicRole( aspect ) );
}

void QskSkinHintTableEditor::setBoxShape(
    QskAspect aspect, qreal radius, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShape( aspect ),
        QskBoxShapeMetrics( radius, sizeMode ) );
}

void QskSkinHintTableEditor::setBoxShape( QskAspect aspect,
    qreal topLeft, qreal topRight, qreal bottomLeft, qreal bottomRight,
    Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShape( aspect ),
        QskBoxShapeMetrics( topLeft, topRight, bottomLeft, bottomRight, sizeMode ) );
}

void QskSkinHintTableEditor::setBoxShape(
    QskAspect aspect, const QskBoxShapeMetrics& shape,
    QskStateCombination combination )
{
    setMetricHint( aspectShape( aspect ), shape, combination );
}

void QskSkinHintTableEditor::removeBoxShape(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectShape( aspect ), combination );
}

QskBoxShapeMetrics QskSkinHintTableEditor::boxShape( QskAspect aspect ) const
{
    return metricHint< QskBoxShapeMetrics >( aspectShape( aspect ) );
}

void QskSkinHintTableEditor::setBoxBorderMetrics( QskAspect aspect,
    qreal borderWidth, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectBorder( aspect ),
        QskBoxBorderMetrics( borderWidth, sizeMode ) );
}

void QskSkinHintTableEditor::setBoxBorderMetrics( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectBorder( aspect ),
        QskBoxBorderMetrics( left, top, right, bottom, sizeMode ) );
}

void QskSkinHintTableEditor::setBoxBorderMetrics(
    QskAspect aspect, const QskBoxBorderMetrics& borderMetrics,
    QskStateCombination combination )
{
    setMetricHint( aspectBorder( aspect ), borderMetrics, combination );
}

void QskSkinHintTableEditor::removeBoxBorderMetric(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectBorder( aspect ), combination );
}

QskBoxBorderMetrics QskSkinHintTableEditor::boxBorderMetrics( QskAspect aspect ) const
{
    return metricHint< QskBoxBorderMetrics >( aspectBorder( aspect ) );
}

void QskSkinHintTableEditor::setBoxBorderColors(
    QskAspect aspect, const QskBoxBorderColors& borderColors,
    QskStateCombination combination )
{
    setColorHint( aspectBorder( aspect ), borderColors, combination );
}

void QskSkinHintTableEditor::setBoxBorderColors(QskAspect aspect,
    const QskGradient& left, const QskGradient& top, const QskGradient& right,
    const QskGradient& bottom, QskStateCombination combination )
{
    setColorHint( aspectBorder( aspect ),
        QskBoxBorderColors( left, top, right, bottom ),
        combination );
}

void QskSkinHintTableEditor::removeBoxBorderColors(
    QskAspect aspect, QskStateCombination combination )
{
    return removeColorHint( aspectBorder( aspect ), combination );
}

QskBoxBorderColors QskSkinHintTableEditor::boxBorderColors( QskAspect aspect ) const
{
    return colorHint< QskBoxBorderColors >( aspectBorder( aspect ) );
}

void QskSkinHintTableEditor::setArcMetrics( QskAspect aspect, qreal width,
    qreal startAngle, qreal spanAngle, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShape( aspect ),
        QskArcMetrics( width, startAngle, spanAngle, sizeMode ) );
}

void QskSkinHintTableEditor::setArcMetrics( QskAspect aspect,
    const QskArcMetrics& arcMetrics, QskStateCombination combination )
{
    setMetricHint( aspectShape( aspect ), arcMetrics, combination );
}

void QskSkinHintTableEditor::removeArcMetrics( QskAspect aspect,
    QskStateCombination combination )
{
    return removeMetricHint( aspectShape( aspect ), combination );
}

QskArcMetrics QskSkinHintTableEditor::arcMetrics( QskAspect aspect ) const
{
    return metricHint< QskArcMetrics >( aspectShape( aspect ) );
}
