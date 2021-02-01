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

void QskSkinHintTableEditor::setFlag( QskAspect aspect, int flag )
{
    setFlagHint( aspect, flag );
}

int QskSkinHintTableEditor::flag( QskAspect aspect ) const
{
    return flagHint<int>( aspect );
}

void QskSkinHintTableEditor::setMetric( QskAspect aspect, qreal metric )
{
    setMetricHint( aspect, metric );
}

qreal QskSkinHintTableEditor::metric( QskAspect aspect ) const
{
    return metricHint<qreal>( aspect );
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
    return colorHint<QColor>( aspect );
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

QskGradient QskSkinHintTableEditor::gradient( QskAspect aspect ) const
{
    return colorHint<QskGradient>( aspect );
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
