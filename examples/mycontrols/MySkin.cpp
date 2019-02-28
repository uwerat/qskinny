/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MySkin.h"

#include "MyToggleButton.h"
#include "MyToggleButtonSkinlet.h"

#include <QskSkin.h>
#include <QskAnimationHint.h>
#include <QskSetup.h>

#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskBoxBorderColors.h>
#include <QskGradient.h>
#include <QskRgbValue.h>
#include <QskColorFilter.h>

#include <QskBox.h>
#include <QskBoxSkinlet.h>

#include <QskFocusIndicator.h>
#include <QskFocusIndicatorSkinlet.h>

class MySkin : public QskSkin
{
  public:

    enum GraphicRole
    {
        GraphicRoleNormal,
        GraphicRoleInverted
    };

    MySkin()
    {
        declareSkinlet< QskBox, QskBoxSkinlet >();
        declareSkinlet< QskFocusIndicator, QskFocusIndicatorSkinlet >();
        declareSkinlet< MyToggleButton, MyToggleButtonSkinlet >();

#if 1
        setGradient( QskAspect::Control, Qt::gray );
#endif
    }

    void initFocusIndicatorHints(
        qreal border, qreal radius, qreal padding, QRgb rgb )
    {
        const auto subControl = QskFocusIndicator::Panel;

        setBoxBorderMetrics( subControl, border );
        setBoxShape( subControl, radius );
        setMargins( subControl | QskAspect::Padding, padding );
        setGradient( subControl, Qt::transparent );

        setBoxBorderColors( subControl, rgb );
    }

    void initBoxHints(
        qreal border, qreal radius,
        const QskBoxBorderColors& borderColors,
        const QskGradient& fillColor )
    {
        const auto subControl = QskBox::Panel;

        setBoxBorderMetrics( subControl, border );
        setBoxShape( subControl, radius );
        setBoxBorderColors( subControl, borderColors );
        setGradient( subControl, fillColor );
        setMargins( subControl | QskAspect::Padding, 0.5 * radius );
    }

    void initToggleButtonHints(
        bool raised, qreal width, qreal height, qreal radius,
        QRgb baseColor, QRgb baseTextColor,
        QRgb foregroundColor, QRgb foregroundTextColor )
    {
        using namespace QskAspect;
        using Q = MyToggleButton;

        for( auto subControl : { Q::UncheckedLabel, Q::CheckedLabel } )
        {
            QColor color1 = foregroundTextColor;
            QColor color2 = baseTextColor;

            if( subControl == Q::UncheckedLabel )
            {
                std::swap( color1, color2 );
            }

            setColor( subControl | Q::Checked, color1 );
            setColor( subControl, color2 );

            setSkinHint( subControl | Alignment, Qt::AlignCenter );
            setAnimation( subControl | Color, animator() );
        }

        for( auto subControl : { Q::UncheckedIcon, Q::CheckedIcon } )
        {
            int role1 = MySkin::GraphicRoleNormal;
            int role2 = MySkin::GraphicRoleInverted;

            if( subControl == Q::UncheckedIcon )
            {
                std::swap( role1, role2 );
            }

            setGraphicRole( subControl, role1 );
            setGraphicRole( subControl | Q::Checked, role2 );

            setAnimation( subControl | Flag, animator() );
        }

        setGradient( Q::Panel, baseColor );

        if ( raised )
        {
            setBoxBorderMetrics( Q::Panel, 3 );

            auto light = QColor( baseColor ).light( 110 );
            auto dark = QColor( baseColor ).dark( 120 );
            setBoxBorderColors( Q::Panel,
                QskBoxBorderColors( light, light, dark, dark ) );
        }
        else
        {
            setBoxBorderMetrics( Q::Panel, 1 );
            setBoxBorderColors( Q::Panel, QColor( baseColor ).dark( 120 ) );
        }

        setGradient( Q::Cursor, foregroundColor );
        setBoxBorderColors( Q::Cursor, QColor( foregroundColor ).dark( 120 ) );
        setBoxBorderMetrics( Q::Cursor, 1 );

        for( auto subControl : { Q::Panel, Q::Cursor } )
        {
            setMetric( subControl | MinimumWidth, width );
            setMetric( subControl | MinimumHeight, height );
            //setMargins( subControl | Padding, +2 );

            setBoxShape( subControl, radius );
        }

        setMargins( Q::CheckedPanel | Padding, 10 );
        setMargins( Q::UncheckedPanel | Padding, 10 );

        for( auto state : { NoState, Q::Disabled } )
        {
            const auto aspect = Q::Cursor | state | Position;

            setMetric( aspect | Q::Checked, 0 );
            setMetric( aspect, 1 );
        }

        setAnimation( Q::Cursor | Metric, animator() );
    }

    void setGraphicFilter( int role, QRgb rgb )
    {
        QskColorFilter filter;
        filter.addColorSubstitution( QskRgbValue::Khaki, rgb );

        QskSkin::setGraphicFilter( role, filter );
    }

  private:

    virtual QskAnimationHint animator() const = 0;
};

class MySkin1 : public MySkin
{
  public:
    MySkin1()
    {
        using namespace QskRgbValue;

        setGraphicFilter( GraphicRoleNormal, QskRgbValue::Crimson );
        setGraphicFilter( GraphicRoleInverted, QskRgbValue::Gold );

        initFocusIndicatorHints( 2, 3, 6, DarkBlue );
        initBoxHints( 2, 8, DarkCyan, LightCyan );
        initToggleButtonHints( false, 150, 120, 6,
            AliceBlue, Black, CornflowerBlue, White );
    }

    QskAnimationHint animator() const override
    {
        return QskAnimationHint( 200, QEasingCurve::Linear );
    }
};

class MySkin2 : public MySkin
{
  public:
    MySkin2()
    {
        using namespace QskRgbValue;

        setGraphicFilter( GraphicRoleNormal, QskRgbValue::HotPink );
        setGraphicFilter( GraphicRoleInverted, QskRgbValue::White );

        initFocusIndicatorHints( 2, 6, 6, Crimson );
        initBoxHints( 4, 30, LightPink, MistyRose );
        initToggleButtonHints( true, 130, 100, 40,
            LightPink, Black, HotPink, White );
    }

    QskAnimationHint animator() const override
    {
        return QskAnimationHint( 100, QEasingCurve::InQuad );
    }
};

QStringList MySkinFactory::skinNames() const
{
    return { QStringLiteral( "MySkin1" ), QStringLiteral( "MySkin2" ) };
}

QskSkin* MySkinFactory::createSkin( const QString& skinName )
{
    if ( skinName == QStringLiteral( "MySkin1" ) )
        return new MySkin1();

    if ( skinName == QStringLiteral( "MySkin2" ) )
        return new MySkin2();

    return nullptr;
}

#include "moc_MySkin.cpp"
