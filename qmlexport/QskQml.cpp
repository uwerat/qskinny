/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQml.h"
#include "QskQml.hpp"

#include "QskLayoutQml.h"
#include "QskShortcutQml.h"
#include "QskMainQml.h"

#include <QskAspect.h>
#include <QskBoxBorderMetrics.h>
#include <QskBoxShapeMetrics.h>
#include <QskDialog.h>
#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskDialogSubWindow.h>
#include <QskDialogWindow.h>
#include <QskFocusIndicator.h>
#include <QskGradient.h>
#include <QskGradientDirection.h>
#include <QskGraphicLabel.h>
#include <QskIntervalF.h>
#include <QskLayoutMetrics.h>
#include <QskMargins.h>
#include <QskMessageWindow.h>
#include <QskPlacementPolicy.h>
#include <QskPopup.h>
#include <QskProgressBar.h>
#include <QskPushButton.h>
#include <QskScrollArea.h>
#include <QskScrollView.h>
#include <QskSelectionWindow.h>
#include <QskSeparator.h>
#include <QskSetup.h>
#include <QskShadowMetrics.h>
#include <QskSimpleListBox.h>
#include <QskSkin.h>
#include <QskSkinManager.h>
#include <QskSlider.h>
#include <QskStandardSymbol.h>
#include <QskStatusIndicator.h>
#include <QskSubWindow.h>
#include <QskSubWindowArea.h>
#include <QskTabBar.h>
#include <QskTabButton.h>
#include <QskTabView.h>
#include <QskTextLabel.h>
#include <QskTextOptions.h>
#include <QskVirtualKeyboard.h>
#include <QskWindow.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 2, 0 )
    QSK_QT_PRIVATE_BEGIN
        #include <private/qqmlmetatype_p.h>
    QSK_QT_PRIVATE_END
#endif

#if QT_VERSION < QT_VERSION_CHECK( 6, 5, 0 )

#include <qjsvalue.h>
#include <qjsvalueiterator.h>

namespace
{
    /*
        Since Qt 6.5 we have QML_STRUCTURED_VALUE and do not need to
        write our own converter.

        However: we should also be able to implement a generic converter from the
        metatype information: TODO ...

        For the moment we have these converters:
     */
    QskGradientStop toGradientStop( const QJSValue& value )
    {
        return QskGradientStop(
            value.property( QStringLiteral( "position" ) ).toNumber(),
            value.property( QStringLiteral( "color" ) ).toVariant().value< QColor >()
        );
    }

    QskLinearDirection toLinearDirection( const QJSValue& value )
    {
        return QskLinearDirection(
            value.property( QStringLiteral( "x1" ) ).toNumber(),
            value.property( QStringLiteral( "y1" ) ).toNumber(),
            value.property( QStringLiteral( "x2" ) ).toNumber(),
            value.property( QStringLiteral( "y2" ) ).toNumber() );
    }

    QskConicDirection toConicDirection( const QJSValue& value )
    {
        return QskConicDirection(
            value.property( QStringLiteral( "x" ) ).toNumber(),
            value.property( QStringLiteral( "y" ) ).toNumber(),
            value.property( QStringLiteral( "startAngle" ) ).toNumber(),
            value.property( QStringLiteral( "spanAngle" ) ).toNumber() );
    }

    QskRadialDirection toRadialDirection( const QJSValue& value )
    {
        return QskRadialDirection(
            value.property( QStringLiteral( "x" ) ).toNumber(),
            value.property( QStringLiteral( "y" ) ).toNumber(),
            value.property( QStringLiteral( "radiusX" ) ).toNumber(),
            value.property( QStringLiteral( "radiusY" ) ).toNumber() );
    }

    QskGradient toGradient( const QJSValue& value )
    {
        QskGradient gradient;

        QJSValueIterator it( value );

        while ( it.hasNext() )
        {
            it.next();

            auto v = it.value();

            if ( v.isObject() )
            {
                if ( v.isArray() )
                {
                    if ( it.name() == QStringLiteral( "stops" ) )
                    {
                        QskGradientStops stops;

                        const int n = v.property( QStringLiteral( "length" ) ).toInt();
                        for ( int i = 0; i < n; i++ )
                            stops += toGradientStop( v.property( i ) );

                        gradient.setStops( stops );
                    }
                }
                else
                {
                    if ( it.name() == QStringLiteral( "linear" ) )
                    {
                        gradient.setLinearDirection( toLinearDirection( v ) );
                    }
                    else if ( it.name() == QStringLiteral( "conic" ) )
                    {
                        gradient.setConicDirection( toConicDirection( v ) );
                    }
                    else if ( it.name() == QStringLiteral( "radial" ) )
                    {
                        gradient.setRadialDirection( toRadialDirection( v ) );
                    }
                }
            }
            else if ( v.isNumber() )
            {
                if ( it.name() == QStringLiteral( "spreadMode" ) )
                {
                    const auto s = v.toNumber();
                    if ( s >= QskGradient::PadSpread && s <= QskGradient::RepeatSpread )
                    {
                        gradient.setSpreadMode(
                            static_cast< QskGradient::SpreadMode >( s ) );
                    }
                }
            }
        }

        return gradient;
    }

    void registerJSConverters()
    {
        QMetaType::registerConverter< QJSValue, QskGradient >( toGradient );
        QMetaType::registerConverter< QJSValue, QskLinearDirection >( toLinearDirection );
        QMetaType::registerConverter< QJSValue, QskConicDirection >( toConicDirection );
        QMetaType::registerConverter< QJSValue, QskRadialDirection >( toRadialDirection );
        QMetaType::registerConverter< QJSValue, QskGradientStop >( toGradientStop );
    }
}

#endif

void QskQml::registerTypes()
{
    qmlRegisterUncreatableType< QskSetup >( QSK_MODULE_NAME, 1, 0, "Setup", QString() );
    qmlRegisterUncreatableType< QskSkin >( QSK_MODULE_NAME, 1, 0, "Skin", QString() );
    qRegisterMetaType< QskSkin* >();

    registerObject< QskMain >();
    registerObject< QskShortcutQml >( "Shortcut" );

    registerObject< QskWindow >();

    registerObject< QskDialogWindow >();
    registerObject< QskMessageWindow >();
    registerObject< QskSelectionWindow >();

    registerObject< QskGridBoxQml >( "GridBox" );
    registerObject< QskLinearBoxQml >( "LinearBox" );

    registerObject< QskControl >();
    registerObject< QskGraphicLabel >();
    registerObject< QskVirtualKeyboard >();
    registerObject< QskTextLabel >();
    registerObject< QskTabButton >();
    registerObject< QskTabBar >();
    registerObject< QskTabView >();
    registerObject< QskFocusIndicator >();
    registerObject< QskSeparator >();
    registerObject< QskProgressBar >();
    registerObject< QskPushButton >();
    registerObject< QskScrollView >();
    registerObject< QskScrollArea >();
    registerObject< QskSlider >();
    registerObject< QskSimpleListBox >();
    registerObject< QskDialogButton >();
    registerObject< QskDialogButtonBox >();
    registerObject< QskPopup >();
    registerObject< QskStatusIndicator >();
    registerObject< QskSubWindow >();
    registerObject< QskSubWindowArea >();
    registerObject< QskDialogSubWindow >();

    registerSingleton< QskDialog >( QskDialog::instance() );

#if 0
    qmlRegisterUncreatableType< QskSkin >( "Skinny.Skins",
        QSK_VERSION_MAJOR, QSK_VERSION_MINOR, "Skin", QString() );
#endif

    QSK_REGISTER_FLAGS( QskQuickItem::UpdateFlag );
    QSK_REGISTER_FLAGS( QskQuickItem::UpdateFlags );
    QSK_REGISTER_FLAGS( QskSizePolicy::Policy );

    QSK_REGISTER_FLAGS( QskDialog::Actions );

    registerGadget< QskBoxBorderMetrics >();
    registerGadget< QskBoxShapeMetrics >();
    registerGadget< QskShadowMetrics >();
    registerGadget< QskIntervalF >();
    registerGadget< QskLayoutMetrics >();
    registerGadget< QskMargins >();

    registerGadget< QskGradient >();
    registerGadget< QskGradientStop >();
    registerGadget< QskLinearDirection >();
    registerGadget< QskConicDirection >();
    registerGadget< QskRadialDirection >();

    registerGadget< QskAspect >();
    registerGadget< QskPlacementPolicy >();
    registerGadget< QskSizePolicy >();
    registerGadget< QskTextOptions >();

    registerNamespace( QskStandardSymbol::staticMetaObject );

#if QT_VERSION < QT_VERSION_CHECK( 6, 5, 0 )
    registerJSConverters();
#endif


#if QT_VERSION < QT_VERSION_CHECK( 6, 2, 0 )
    /*
        Since Qt 6.5 invokable constructors are accessible from QML, something
        what was possibe until Qt 6.2 with string converters. For Qt [6.2,6.4]
        we do not have any solution.
     */

    QQmlMetaType::registerCustomStringConverter( qMetaTypeId< QskMargins >(),
        []( const QString& s ) { return QVariant::fromValue( QskMargins( s.toDouble() ) ); } );
#endif
}
