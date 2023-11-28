/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSkinHintTableEditor.h"
#include "QskSkinHintTable.h"

#include "QskArcMetrics.h"
#include "QskMargins.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskGraduationMetrics.h"
#include "QskShadowMetrics.h"
#include "QskStippleMetrics.h"
#include "QskGraphic.h"

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

    inline QskAspect aspectShadow( QskAspect aspect )
    {
        return aspect | QskAspect::Shadow;
    }

    inline QskAspect aspectOption( QskAspect aspect )
    {
        return aspect | QskAspect::Option;
    }

    inline QskAspect aspectSymbol( QskAspect aspect )
    {
        return aspect | QskAspect::Symbol;
    }

    inline QskAspect aspectStipple( QskAspect aspect )
    {
        return aspect | QskAspect::Style;
    }

    inline QskAspect aspectGraduation( QskAspect aspect )
    {
        return aspect | QskAspect::Graduation;
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
    setHint( aspect, flag, combination );
}

int QskSkinHintTableEditor::flag( QskAspect aspect ) const
{
    return hint< int >( aspect );
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
    QskGradient gradient( color1, color2 );
    gradient.setLinearDirection( Qt::Horizontal );

    setGradient( aspect, gradient, combination );
}

void QskSkinHintTableEditor::setVGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskStateCombination combination )
{
    QskGradient gradient( color1, color2 );
    gradient.setLinearDirection( Qt::Vertical );

    setGradient( aspect, gradient, combination );
}

void QskSkinHintTableEditor::setGradient(
    QskAspect aspect, const QColor& color1, const QColor& color2,
    QskStateCombination combination )
{
    const QskGradient gradient( color1, color2 );
    setColorHint( aspect, gradient, combination );
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

bool QskSkinHintTableEditor::removePosition(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectPosition( aspect ), combination );
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

bool QskSkinHintTableEditor::removeStrutSize(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectStrutSize( aspect ), combination );
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

bool QskSkinHintTableEditor::removeMargin(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectMargin( aspect ), combination );
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

bool QskSkinHintTableEditor::removePadding(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectPadding( aspect ), combination );
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

bool QskSkinHintTableEditor::removeSpacing(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectSpacing( aspect ), combination );
}

qreal QskSkinHintTableEditor::spacing( QskAspect aspect ) const
{
    return metricHint< qreal >( aspectSpacing( aspect ) );
}

void QskSkinHintTableEditor::setAlignment(
    QskAspect aspect, Qt::Alignment alignment, QskStateCombination combination )
{
    setFlag( aspectAlignment( aspect ), alignment, combination );
}

bool QskSkinHintTableEditor::removeAlignment(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspectAlignment( aspect ), combination );
}

Qt::Alignment QskSkinHintTableEditor::alignment( QskAspect aspect ) const
{
    return hint< Qt::Alignment >( aspectAlignment( aspect ) );
}

void QskSkinHintTableEditor::setFontRole(
    QskAspect aspect, int fontRole, QskStateCombination combination )
{
    setHint( aspectFontRole( aspect ), fontRole, combination );
}

bool QskSkinHintTableEditor::removeFontRole(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspectFontRole( aspect ), combination );
}

int QskSkinHintTableEditor::fontRole( QskAspect aspect ) const
{
    return hint< int >( aspectFontRole( aspect ) );
}

void QskSkinHintTableEditor::setGraphicRole(
    QskAspect aspect, int graphicRole, QskStateCombination combination )
{
    setHint( aspectGraphicRole( aspect ), graphicRole, combination );
}

bool QskSkinHintTableEditor::removeGraphicRole(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspectGraphicRole( aspect ), combination );
}

int QskSkinHintTableEditor::graphicRole( QskAspect aspect ) const
{
    return hint< int >( aspectGraphicRole( aspect ) );
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

bool QskSkinHintTableEditor::removeBoxShape(
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

bool QskSkinHintTableEditor::removeBoxBorderMetric(
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

void QskSkinHintTableEditor::setBoxBorderColors( QskAspect aspect,
    const QskGradient& left, const QskGradient& top, const QskGradient& right,
    const QskGradient& bottom, QskStateCombination combination )
{
    setColorHint( aspectBorder( aspect ),
        QskBoxBorderColors( left, top, right, bottom ), combination );
}

bool QskSkinHintTableEditor::removeBoxBorderColors(
    QskAspect aspect, QskStateCombination combination )
{
    return removeColorHint( aspectBorder( aspect ), combination );
}

QskBoxBorderColors QskSkinHintTableEditor::boxBorderColors( QskAspect aspect ) const
{
    return colorHint< QskBoxBorderColors >( aspectBorder( aspect ) );
}

void QskSkinHintTableEditor::setShadowMetrics( QskAspect aspect,
    qreal spreadRadius, qreal blurRadius, const QPointF& offset, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShadow( aspect ),
        QskShadowMetrics( spreadRadius, blurRadius, offset, sizeMode ) );
}

void QskSkinHintTableEditor::setShadowMetrics( QskAspect aspect,
    const QskShadowMetrics& metrics, QskStateCombination combination )
{
    setMetricHint( aspectShadow( aspect ), metrics, combination );
}

bool QskSkinHintTableEditor::removeShadowMetric(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectShadow( aspect ), combination );
}

QskShadowMetrics QskSkinHintTableEditor::shadowMetrics( QskAspect aspect ) const
{
    return metricHint< QskShadowMetrics >( aspectShadow( aspect ) );
}

void QskSkinHintTableEditor::setShadowColor( QskAspect aspect,
    const QColor& color, QskStateCombination combination )
{
    setColorHint( aspectShadow( aspect ), color, combination );
}

void QskSkinHintTableEditor::setShadowColor( QskAspect aspect,
    Qt::GlobalColor color, QskStateCombination combination )
{
    setColorHint( aspectShadow( aspect ), QColor( color ), combination );
}

void QskSkinHintTableEditor::setShadowColor( QskAspect aspect,
    QRgb rgb, QskStateCombination combination )
{
    setColorHint( aspectShadow( aspect ), QColor::fromRgba( rgb ), combination );
}

bool QskSkinHintTableEditor::removeShadowColor(
    QskAspect aspect, QskStateCombination combination )
{
    return removeColorHint( aspectShadow( aspect ), combination );
}

QColor QskSkinHintTableEditor::shadowColor( QskAspect aspect ) const
{
    return colorHint< QColor >( aspectShadow( aspect ) );
}

void QskSkinHintTableEditor::setArcMetrics( QskAspect aspect,
    qreal startAngle, qreal spanAngle, qreal thickness, Qt::SizeMode sizeMode )
{
    setMetricHint( aspectShape( aspect ),
        QskArcMetrics( startAngle, spanAngle, thickness, sizeMode ) );
}

void QskSkinHintTableEditor::setArcMetrics( QskAspect aspect,
    const QskArcMetrics& arcMetrics, QskStateCombination combination )
{
    setMetricHint( aspectShape( aspect ), arcMetrics, combination );
}

bool QskSkinHintTableEditor::removeArcMetrics(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectShape( aspect ), combination );
}

QskArcMetrics QskSkinHintTableEditor::arcMetrics( QskAspect aspect ) const
{
    return metricHint< QskArcMetrics >( aspectShape( aspect ) );
}

void QskSkinHintTableEditor::setStippleMetrics( QskAspect aspect,
    Qt::PenStyle penStyle, QskStateCombination combination )
{
    setStippleMetrics( aspect, QskStippleMetrics( penStyle ), combination );
}

void QskSkinHintTableEditor::setStippleMetrics( QskAspect aspect,
    const QVector< qreal >& dashPattern, QskStateCombination combination )
{
    setStippleMetrics( aspect, QskStippleMetrics( dashPattern ), combination );
}

void QskSkinHintTableEditor::setStippleMetrics( QskAspect aspect,
    const QskStippleMetrics& metrics, QskStateCombination combination )
{
    setMetricHint( aspectStipple( aspect ), metrics, combination );
}

bool QskSkinHintTableEditor::removeStippleMetrics(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectStipple( aspect ), combination );
}

QskStippleMetrics QskSkinHintTableEditor::stippleMetrics( QskAspect aspect ) const
{
    return metricHint< QskStippleMetrics >( aspectStipple( aspect ) );
}

void QskSkinHintTableEditor::setGraduationMetrics( QskAspect aspect,
    const QskGraduationMetrics& metrics, QskStateCombination combination )
{
    setMetricHint( aspectGraduation( aspect ), metrics, combination );
}

bool QskSkinHintTableEditor::removeGraduationMetrics(
    QskAspect aspect, QskStateCombination combination )
{
    return removeMetricHint( aspectGraduation( aspect ), combination );
}

QskGraduationMetrics QskSkinHintTableEditor::graduationMetrics( QskAspect aspect ) const
{
    return metricHint< QskGraduationMetrics >( aspectGraduation( aspect ) );
}

void QskSkinHintTableEditor::setTextOptions( QskAspect aspect,
    Qt::TextElideMode elideMode, QskTextOptions::WrapMode wrapMode,
    QskStateCombination combination )
{
    QskTextOptions options;
    options.setElideMode( elideMode );
    options.setWrapMode( wrapMode );

    setTextOptions( aspect, options, combination );
}

void QskSkinHintTableEditor::setTextOptions( QskAspect aspect,
    const QskTextOptions& textOptions, QskStateCombination combination )
{
    setHint( aspectOption( aspect ), textOptions, combination );
}

bool QskSkinHintTableEditor::removeTextOptions(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspectOption( aspect ), combination );
}

QskTextOptions QskSkinHintTableEditor::textOptions( QskAspect aspect ) const
{
    return hint< QskTextOptions >( aspectOption( aspect ) );
}

void QskSkinHintTableEditor::setSymbol( QskAspect aspect,
    const QskGraphic& symbol, QskStateCombination combination )
{
    setHint( aspectSymbol( aspect ), symbol, combination );
}

bool QskSkinHintTableEditor::removeSymbol(
    QskAspect aspect, QskStateCombination combination )
{
    return removeHint( aspectSymbol( aspect ), combination );
}

QskGraphic QskSkinHintTableEditor::symbol( QskAspect aspect ) const
{
    return hint< QskGraphic >( aspectSymbol( aspect ) );
}
