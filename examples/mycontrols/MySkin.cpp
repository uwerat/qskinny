/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "MySkin.h"

#include "MyToggleButton.h"
#include "MyToggleButtonSkinlet.h"

#include <QskSkin.h>
#include <QskSkinHintTableEditor.h>
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
    }

  protected:
    void setGraphicFilter( int role, QRgb rgb )
    {
        QskColorFilter filter;
        filter.addColorSubstitution( QskRgb::Khaki, rgb );

        QskSkin::setGraphicFilter( role, filter );
    }
};

class MySkinEditor : public QskSkinHintTableEditor
{
  public:
    void setupFocusIndicator(
        qreal border, qreal radius, qreal padding, QRgb rgb )
    {
        const auto subControl = QskFocusIndicator::Panel;

        setBoxBorderMetrics( subControl, border );
        setBoxShape( subControl, radius );
        setPadding( subControl, padding );
        setGradient( subControl, Qt::transparent );

        setBoxBorderColors( subControl, rgb );
    }

    void setupBox(
        qreal border, qreal radius,
        const QskBoxBorderColors& borderColors,
        const QskGradient& fillColor )
    {
        const auto subControl = QskBox::Panel;

        setBoxBorderMetrics( subControl, border );
        setBoxShape( subControl, radius );
        setBoxBorderColors( subControl, borderColors );
        setGradient( subControl, fillColor );
        setPadding( subControl, 0.5 * radius );
    }

    void setupToggleButton(
        bool raised, qreal width, qreal height, qreal radius,
        QRgb baseColor, QRgb baseTextColor,
        QRgb foregroundColor, QRgb foregroundTextColor )
    {
        using A = QskAspect;
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

            setAlignment( subControl, Qt::AlignCenter );
            setAnimation( subControl | A::Color, animator() );
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

            setAnimation( subControl | A::Flag, animator() );
        }

        setGradient( Q::Panel, baseColor );

        if ( raised )
        {
            setBoxBorderMetrics( Q::Panel, 3 );

            auto light = QColor( baseColor ).lighter( 110 );
            auto dark = QColor( baseColor ).darker( 120 );
            setBoxBorderColors( Q::Panel,
                QskBoxBorderColors( light, light, dark, dark ) );
        }
        else
        {
            setBoxBorderMetrics( Q::Panel, 1 );
            setBoxBorderColors( Q::Panel, QColor( baseColor ).darker( 120 ) );
        }

        setGradient( Q::Cursor, foregroundColor );
        setBoxBorderColors( Q::Cursor, QColor( foregroundColor ).darker( 120 ) );
        setBoxBorderMetrics( Q::Cursor, 1 );

        setStrutSize( Q::Panel, width, height );

        setBoxShape( Q::Panel, radius );
        setBoxShape( Q::Cursor, radius );

        setPadding( Q::CheckedPanel, 10 );
        setPadding( Q::UncheckedPanel, 10 );

        for( auto state : { A::NoState, Q::Disabled } )
        {
            const auto aspect = Q::Cursor | state | A::Position;

            setMetric( aspect | Q::Checked, 0 );
            setMetric( aspect, 1 );
        }

        setAnimation( Q::Cursor | A::Metric, animator() );
    }

    void setAnimator( uint duration, QEasingCurve::Type type )
    {
        m_animationHint.duration = duration;
        m_animationHint.type = type;
    }

    QskAnimationHint animator() const
    {
        return m_animationHint;
    }

  private:
    QskAnimationHint m_animationHint;
};

class MySkin1 : public MySkin
{
  public:
    MySkin1()
    {
        using namespace QskRgb;

        setGraphicFilter( GraphicRoleNormal, Crimson );
        setGraphicFilter( GraphicRoleInverted, Gold );

        MySkinEditor editor;
        editor.setTable( &hintTable() );
        editor.setAnimator( 200, QEasingCurve::Linear ); 

        editor.setGradient( QskAspect::Control, Qt::gray );

        editor.setupFocusIndicator( 2, 3, 6, DarkBlue );
        editor.setupBox( 2, 8, DarkCyan, LightCyan );

        editor.setupToggleButton( false, 150, 120, 6,
            AliceBlue, Black, CornflowerBlue, White );
    }
};

class MySkin2 : public MySkin
{
  public:
    MySkin2()
    {
        using namespace QskRgb;

        setGraphicFilter( GraphicRoleNormal, HotPink );
        setGraphicFilter( GraphicRoleInverted, White );

        MySkinEditor editor;
        editor.setTable( &hintTable() );
        editor.setAnimator( 100, QEasingCurve::InQuad );

        editor.setGradient( QskAspect::Control, Qt::gray );

        editor.setupFocusIndicator( 2, 6, 6, Crimson );
        editor.setupBox( 4, 30, LightPink, MistyRose );

        editor.setupToggleButton( true, 130, 100, 40,
            LightPink, Black, HotPink, White );
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
