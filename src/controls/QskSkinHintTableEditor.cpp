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

void QskSkinHintTableEditor::setHint( QskAspect aspect,
    const QVariant& hint, QskAspect::State combinationMask )
{
    if ( combinationMask == QskAspect::NoState )
    {
        m_table->setHint( aspect, hint );
    }
    else
    {
        setHintStateMask( m_table, aspect, hint,
            QskAspect::NoState, combinationMask );
    }
}

bool QskSkinHintTableEditor::removeHint(
    QskAspect aspect, QskAspect::State combinationMask )
{
    if ( combinationMask == QskAspect::NoState )
    {
        return m_table->removeHint( aspect );
    }
    else
    {
        return removeHintStateMask( m_table, aspect,
            QskAspect::NoState, combinationMask );
    }
}

void QskSkinHintTableEditor::setFlag(
    QskAspect aspect, int flag, QskAspect::State combinationMask )
{
    setFlagHint( aspect, flag, combinationMask );
}

int QskSkinHintTableEditor::flag( QskAspect aspect ) const
{
    return flagHint< int >( aspect );
}

void QskSkinHintTableEditor::setMetric(
    QskAspect aspect, qreal metric, QskAspect::State combinationMask )
{
    setMetricHint( aspect, metric, combinationMask );
}

qreal QskSkinHintTableEditor::metric( QskAspect aspect ) const
{
    return metricHint< qreal >( aspect );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, Qt::GlobalColor color, QskAspect::State combinationMask )
{
    setColorHint( aspect, QColor( color ), combinationMask );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, QRgb rgb, QskAspect::State combinationMask )
{
    setColorHint( aspect, QColor::fromRgba( rgb ), combinationMask );
}

void QskSkinHintTableEditor::setColor(
    QskAspect aspect, const QColor& color, QskAspect::State combinationMask )
{
    setColorHint( aspect, color, combinationMask );
}

QColor QskSkinHintTableEditor::color( QskAspect aspect ) const
{
    return colorHint< QColor >( aspect );
}

void QskSkinHintTableEditor::setHGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskAspect::State combinationMask )
{
    const QskGradient gradient( QskGradient::Horizontal, color1, color2 );
    setGradient( aspect, gradient, combinationMask );
}

void QskSkinHintTableEditor::setVGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskAspect::State combinationMask )
{
    const QskGradient gradient( QskGradient::Vertical, color1, color2 );
    setGradient( aspect, gradient, combinationMask );
}

void QskSkinHintTableEditor::setGradient(
    QskAspect aspect, const QskGradient& gradient,
    QskAspect::State combinationMask )
{
    setColorHint( aspect, gradient, combinationMask );
}

QskGradient QskSkinHintTableEditor::gradient( QskAspect aspect ) const
{
    return colorHint< QskGradient >( aspect );
}

void QskSkinHintTableEditor::setStrutSize(
    QskAspect aspect, const QSizeF& size, QskAspect::State combinationMask )
{
    setMetricHint( aspectStrutSize( aspect ), size, combinationMask );
}

void QskSkinHintTableEditor::setStrutSize(
    QskAspect aspect, qreal width, qreal height, QskAspect::State combinationMask )
{
    setMetricHint( aspectStrutSize( aspect ),
        QSizeF( width, height ), combinationMask );
}

void QskSkinHintTableEditor::removeStrutSize(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeMetricHint( aspectStrutSize( aspect ), combinationMask );
}

QSizeF QskSkinHintTableEditor::strutSize( QskAspect aspect ) const
{
    return metricHint< QSizeF >( aspectStrutSize( aspect ) );
}

void QskSkinHintTableEditor::setMargin(
    QskAspect aspect, const QskMargins& margins, QskAspect::State combinationMask )
{
    setMetricHint( aspectMargin( aspect ), margins, combinationMask );
}

void QskSkinHintTableEditor::setMargin( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom,
    QskAspect::State combinationMask )
{
    const QskMargins margins( left, top, right, bottom );
    setMetricHint( aspectMargin( aspect ), margins, combinationMask );
}

void QskSkinHintTableEditor::removeMargin(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeMetricHint( aspectMargin( aspect ), combinationMask );
}

QskMargins QskSkinHintTableEditor::margin( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectMargin( aspect ) );
}

void QskSkinHintTableEditor::setPadding(
    QskAspect aspect, const QskMargins& padding,
    QskAspect::State combinationMask )
{
    setMetricHint( aspectPadding( aspect ), padding, combinationMask );
}

void QskSkinHintTableEditor::setPadding( QskAspect aspect,
    qreal left, qreal top, qreal right, qreal bottom,
    QskAspect::State combinationMask )
{
    QskMargins padding( left, top, right, bottom );
    setMetricHint( aspectPadding( aspect ), padding, combinationMask );
}

void QskSkinHintTableEditor::removePadding(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeMetricHint( aspectPadding( aspect ), combinationMask );
}

QskMargins QskSkinHintTableEditor::padding( QskAspect aspect ) const
{
    return metricHint< QskMargins >( aspectPadding( aspect ) );
}

void QskSkinHintTableEditor::setSpacing(
    QskAspect aspect, qreal spacing, QskAspect::State combinationMask )
{
    setMetricHint( aspectSpacing( aspect ), spacing, combinationMask );
}

void QskSkinHintTableEditor::removeSpacing(
    QskAspect aspect, QskAspect::State combinationMask )
{
    removeMetricHint( aspectSpacing( aspect ), combinationMask );
}

qreal QskSkinHintTableEditor::spacing( QskAspect aspect ) const
{
    return metricHint< qreal >( aspectSpacing( aspect ) );
}

void QskSkinHintTableEditor::setAlignment(
    QskAspect aspect, Qt::Alignment alignment, QskAspect::State combinationMask )
{
    setFlagHint( aspectAlignment( aspect ),
        static_cast< int >( alignment ), combinationMask );
}

void QskSkinHintTableEditor::removeAlignment(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeFlagHint( aspectAlignment( aspect ), combinationMask );
}

Qt::Alignment QskSkinHintTableEditor::alignment( QskAspect aspect ) const
{
    return flagHint< Qt::Alignment >( aspectAlignment( aspect ) );
}

void QskSkinHintTableEditor::setFontRole(
    QskAspect aspect, int fontRole, QskAspect::State combinationMask )
{
    setFlagHint( aspectFontRole( aspect ), fontRole, combinationMask );
}

void QskSkinHintTableEditor::removeFontRole(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeFlagHint( aspectFontRole( aspect ), combinationMask );
}

int QskSkinHintTableEditor::fontRole( QskAspect aspect ) const
{
    return flagHint< int >( aspectFontRole( aspect ) );
}

void QskSkinHintTableEditor::setGraphicRole(
    QskAspect aspect, int graphicRole, QskAspect::State combinationMask )
{
    setFlagHint( aspectGraphicRole( aspect ), graphicRole, combinationMask );
}

void QskSkinHintTableEditor::removeGraphicRole(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeFlagHint( aspectGraphicRole( aspect ), combinationMask );
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
    QskAspect aspect, const QskBoxShapeMetrics& shape, QskAspect::State combinationMask )
{
    setMetricHint( aspectShape( aspect ), shape, combinationMask );
}

void QskSkinHintTableEditor::removeBoxShape(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeMetricHint( aspectShape( aspect ), combinationMask );
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
    QskAspect::State combinationMask )
{
    setMetricHint( aspectBorder( aspect ), borderMetrics, combinationMask );
}

void QskSkinHintTableEditor::removeBoxBorderMetric(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeMetricHint( aspectBorder( aspect ), combinationMask );
}

QskBoxBorderMetrics QskSkinHintTableEditor::boxBorderMetrics( QskAspect aspect ) const
{
    return metricHint< QskBoxBorderMetrics >( aspectBorder( aspect ) );
}

void QskSkinHintTableEditor::setBoxBorderColors(
    QskAspect aspect, const QskBoxBorderColors& borderColors,
    QskAspect::State combinationMask )
{
    setColorHint( aspectBorder( aspect ), borderColors, combinationMask );
}

void QskSkinHintTableEditor::setBoxBorderColors( QskAspect aspect,
    const QColor& left, const QColor& top, const QColor& right, const QColor& bottom,
    QskAspect::State combinationMask )
{
    setColorHint( aspectBorder( aspect ),
        QskBoxBorderColors( left, top, right, bottom ),
        combinationMask );
}

void QskSkinHintTableEditor::removeBoxBorderColors(
    QskAspect aspect, QskAspect::State combinationMask )
{
    return removeColorHint( aspectBorder( aspect ), combinationMask );
}

QskBoxBorderColors QskSkinHintTableEditor::boxBorderColors( QskAspect aspect ) const
{
    return colorHint< QskBoxBorderColors >( aspectBorder( aspect ) );
}
