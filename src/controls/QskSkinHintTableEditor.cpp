/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSkinHintTableEditor.h"
#include "QskSkinHintTable.h"

#include "QskMargins.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGradient.h"

namespace
{
    inline QskAspect::State lowestState( QskAspect::State mask )
    {
        using StateInt = typename std::underlying_type< QskAspect::State >::type;

        const auto count = qCountTrailingZeroBits( static_cast< StateInt >( mask ) ); 
        return static_cast< QskAspect::State >( 1 << count );
    }

    inline void setHintStateMask(
        QskSkinHintTable* table, QskAspect aspect, const QVariant& hint,
        QskAspect::State state, QskAspect::State mask )
    {
        if ( mask == 0 )
        {
            if ( state != 0 )
                table->setHint( aspect | state, hint );

            return;
        }

        const auto stateBit = lowestState( mask );

        mask &= ~stateBit;

        setHintStateMask( table, aspect, hint, state, mask );
        setHintStateMask( table, aspect, hint, state | stateBit, mask );
    }

    inline bool removeHintStateMask(
        QskSkinHintTable* table, QskAspect aspect,
        QskAspect::State state, QskAspect::State mask )
    {   
        if ( mask == 0 )
        {   
            if ( state != 0 )
                return table->removeHint( aspect | state );
            
            return false;
        }
        
        const auto stateBit = lowestState( mask );

        mask &= ~stateBit;
        
        bool ret = removeHintStateMask( table, aspect, state, mask );
        ret |= removeHintStateMask( table, aspect, state | stateBit, mask );

        return ret;
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

void QskSkinHintTableEditor::setHintForAllStateCombinations(
    QskAspect aspect, const QVariant& hint, QskAspect::State stateMask )
{
    setHintStateMask( m_table, aspect, hint, QskAspect::NoState, stateMask );
}

bool QskSkinHintTableEditor::removeHintForAllStateCombinations(
    QskAspect aspect, QskAspect::State stateMask )
{
    return removeHintStateMask( m_table, aspect, QskAspect::NoState, stateMask );
}

void QskSkinHintTableEditor::setFlagHintForAllStateCombinations(
    QskAspect aspect, const QVariant& hint, QskAspect::State stateMask )
{
    setHintForAllStateCombinations( aspect | QskAspect::Flag, hint, stateMask );
}

void QskSkinHintTableEditor::setMetricHintForAllStateCombinations(
    QskAspect aspect, const QVariant& hint, QskAspect::State stateMask )
{
    setHintForAllStateCombinations( aspect | QskAspect::Metric, hint, stateMask );
}

void QskSkinHintTableEditor::setColorHintForAllStateCombinations(
    QskAspect aspect, const QVariant& hint, QskAspect::State stateMask )
{
    setHintForAllStateCombinations( aspect | QskAspect::Color, hint, stateMask );
}

void QskSkinHintTableEditor::setFlag( QskAspect aspect, int flag )
{
    setFlagHint( aspect, flag );
}

int QskSkinHintTableEditor::flag( QskAspect aspect ) const
{
    return flagHint< int >( aspect );
}

void QskSkinHintTableEditor::setMetric( QskAspect aspect, qreal metric )
{
    setMetricHint( aspect, metric );
}

qreal QskSkinHintTableEditor::metric( QskAspect aspect ) const
{
    return metricHint< qreal >( aspect );
}

void QskSkinHintTableEditor::setColor( QskAspect aspect, Qt::GlobalColor color )
{
    setColorHint( aspect, QColor( color ) );
}

void QskSkinHintTableEditor::setColor( QskAspect aspect, QRgb rgb )
{
    setColorHint( aspect, QColor::fromRgba( rgb ) );
}

void QskSkinHintTableEditor::setColor( QskAspect aspect, const QColor& color )
{
    setColorHint( aspect, color );
}

QColor QskSkinHintTableEditor::color( QskAspect aspect ) const
{
    return colorHint< QColor >( aspect );
}

void QskSkinHintTableEditor::setHGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2 )
{
    setGradient( aspect, QskGradient( QskGradient::Horizontal, color1, color2 ) );
}

void QskSkinHintTableEditor::setVGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2 )
{
    setGradient( aspect, QskGradient( QskGradient::Vertical, color1, color2 ) );
}

void QskSkinHintTableEditor::setGradient( QskAspect aspect, const QskGradient& gradient )
{
    setColorHint( aspect, gradient );
}

void QskSkinHintTableEditor::setGradientForAllStateCombinations(
    QskAspect aspect, const QskGradient& gradient, QskAspect::State stateMask )
{
    setColorHintForAllStateCombinations( aspect, QVariant::fromValue( gradient ), stateMask );
}

QskGradient QskSkinHintTableEditor::gradient( QskAspect aspect ) const
{
    return colorHint< QskGradient >( aspect );
}

void QskSkinHintTableEditor::setStrutSize( QskAspect aspect, const QSizeF& size )
{
    setMetricHint( aspect | QskAspect::StrutSize, size );
}

void QskSkinHintTableEditor::setStrutSize( QskAspect aspect, qreal width, qreal height )
{
    setMetricHint( aspectStrutSize( aspect ), QSizeF( width, height ) );
}

void QskSkinHintTableEditor::removeStrutSize( QskAspect aspect )
{
    removeMetricHint( aspectStrutSize( aspect ) );
}

QSizeF QskSkinHintTableEditor::strutSize( QskAspect aspect ) const
{
    return metricHint< QSizeF >( aspectStrutSize( aspect ) );
}

void QskSkinHintTableEditor::setMargin( QskAspect aspect, const QskMargins& margins )
{
    setMetricHint( aspectMargin( aspect ), margins );
}

void QskSkinHintTableEditor::setMargin( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom )
{
    setMetricHint( aspectMargin( aspect ), QskMargins( left, top, right, bottom ) );
}

void QskSkinHintTableEditor::removeMargin( QskAspect aspect )
{
    removeMetricHint( aspectMargin( aspect ) );
}

QskMargins QskSkinHintTableEditor::margin( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectMargin( aspect ) );
}

void QskSkinHintTableEditor::setPadding( QskAspect aspect, const QskMargins& padding )
{
    setMetricHint( aspectPadding( aspect ), padding );
}

void QskSkinHintTableEditor::setPadding( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom )
{
    setMetricHint( aspectPadding( aspect ), QskMargins( left, top, right, bottom ) );
}

void QskSkinHintTableEditor::removePadding( QskAspect aspect )
{
    removeMetricHint( aspectPadding( aspect ) );
}

QskMargins QskSkinHintTableEditor::padding( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectPadding( aspect ) );
}

void QskSkinHintTableEditor::setSpacing( QskAspect aspect, qreal spacing )
{
    setMetricHint( aspectSpacing( aspect ), spacing );
}

void QskSkinHintTableEditor::removeSpacing( QskAspect aspect )
{
    removeMetricHint( aspectSpacing( aspect ) );
}

qreal QskSkinHintTableEditor::spacing( QskAspect aspect ) const
{
    return metricHint< qreal >( aspectSpacing( aspect ) );
}

void QskSkinHintTableEditor::setAlignment( QskAspect aspect, Qt::Alignment alignment )
{
    setFlagHint( aspectAlignment( aspect ),  static_cast< int >( alignment ) );
}

void QskSkinHintTableEditor::removeAlignment( QskAspect aspect )
{
    return removeFlagHint( aspectAlignment( aspect ) );
}

Qt::Alignment QskSkinHintTableEditor::alignment( QskAspect aspect ) const
{
    return flagHint< Qt::Alignment >( aspectAlignment( aspect ) );
}

void QskSkinHintTableEditor::setFontRole( QskAspect aspect, int fontRole )
{
    setFlagHint( aspectFontRole( aspect ), fontRole );
}

void QskSkinHintTableEditor::removeFontRole( QskAspect aspect )
{
    return removeFlagHint( aspectFontRole( aspect ) );
}

int QskSkinHintTableEditor::fontRole( QskAspect aspect ) const
{
    return flagHint< int >( aspectFontRole( aspect ) );
}

void QskSkinHintTableEditor::setGraphicRole( QskAspect aspect, int graphicRole )
{
    setFlagHint( aspectGraphicRole( aspect ), graphicRole );
}

void QskSkinHintTableEditor::removeGraphicRole( QskAspect aspect )
{
    return removeFlagHint( aspectGraphicRole( aspect ) );
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

void QskSkinHintTableEditor::setBoxShape( QskAspect aspect, qreal topLeft, qreal topRight,
    qreal bottomLeft, qreal bottomRight, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShape( aspect ),
        QskBoxShapeMetrics( topLeft, topRight, bottomLeft, bottomRight, sizeMode ) );
}

void QskSkinHintTableEditor::setBoxShape(
    QskAspect aspect, const QskBoxShapeMetrics& shape )
{
    setMetricHint( aspectShape( aspect ), shape );
}

void QskSkinHintTableEditor::setBoxShapeForAllStateCombinations(
    QskAspect aspect, const QskBoxShapeMetrics& shape, QskAspect::State stateMask )
{
    setMetricHintForAllStateCombinations(
        aspectShape( aspect ), QVariant::fromValue( shape ), stateMask );
}

void QskSkinHintTableEditor::removeBoxShape( QskAspect aspect )
{
    return removeMetricHint( aspectShape( aspect ) );
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
    QskAspect aspect, const QskBoxBorderMetrics& borderMetrics )
{
    setMetricHint( aspectBorder( aspect ), borderMetrics );
}

void QskSkinHintTableEditor::setBoxBorderMetricsForAllStateCombinations(
    QskAspect aspect, const QskBoxBorderMetrics& borderMetrics, QskAspect::State stateMask )
{
    setMetricHintForAllStateCombinations( aspectBorder( aspect ),
        QVariant::fromValue( borderMetrics ), stateMask );
}

void QskSkinHintTableEditor::removeBoxBorderMetric( QskAspect aspect )
{
    return removeMetricHint( aspectBorder( aspect ) );
}

QskBoxBorderMetrics QskSkinHintTableEditor::boxBorderMetrics( QskAspect aspect ) const
{
    return metricHint< QskBoxBorderMetrics >( aspectBorder( aspect ) );
}

void QskSkinHintTableEditor::setBoxBorderColors(
    QskAspect aspect, const QskBoxBorderColors& borderColors )
{
    setColorHint( aspectBorder( aspect ), borderColors );
}

void QskSkinHintTableEditor::setBoxBorderColorsForAllStateCombinations(
    QskAspect aspect, const QskBoxBorderColors& borderColors, QskAspect::State stateMask )
{
    setColorHintForAllStateCombinations( aspectBorder( aspect ),
        QVariant::fromValue( borderColors ), stateMask );
}

void QskSkinHintTableEditor::setBoxBorderColors( QskAspect aspect,
    const QColor& left, const QColor& top, const QColor& right, const QColor& bottom )
{
    setColorHint( aspectBorder( aspect ),
        QskBoxBorderColors( left, top, right, bottom ) );
}

void QskSkinHintTableEditor::removeBoxBorderColors( QskAspect aspect )
{
    return removeColorHint( aspectBorder( aspect ) );
}

QskBoxBorderColors QskSkinHintTableEditor::boxBorderColors( QskAspect aspect ) const
{
    return colorHint< QskBoxBorderColors >( aspectBorder( aspect ) );
}
