/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQml.h"
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

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
#include <qqmlengine.h>
#endif

QSK_QT_PRIVATE_BEGIN
#include <private/qqmlmetatype_p.h>
QSK_QT_PRIVATE_END

#define QSK_MODULE_NAME "Skinny"
#define QSK_VERSION_MAJOR 1
#define QSK_VERSION_MINOR 0

#define QSK_REGISTER( className, typeName ) \
    registerType< className >( typeName );

#define QSK_REGISTER_GADGET( className, typeName ) \
    registerGadget< className >( typeName )

#define QSK_REGISTER_NAMESPACE( className, typeName ) \
    registerStaticMetaObject( className::staticMetaObject, typeName )

#define QSK_REGISTER_SINGLETON( className, typeName, singleton ) \
    registerSingleton< className >( typeName, singleton );

// Required for QFlags to be constructed from an enum value
#define QSK_REGISTER_FLAGS( Type ) \
    QMetaType::registerConverter< int, Type >([] ( int value ) { return Type( value ); })

namespace
{
    template < typename T >
    inline int registerType( const char *qmlName )
    {
        return qmlRegisterType< T >( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, qmlName );
    }

    template < typename T >
    inline int registerGadget( const char *qmlName )
    {
        return qmlRegisterUncreatableType< T >( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, qmlName, QString() );
    }

    inline int registerStaticMetaObject( const QMetaObject &metaObject, const char *qmlName )
    {
        return qmlRegisterUncreatableMetaObject( metaObject, QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, qmlName, QString() );
    }

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
    template < typename T >
    inline int registerSingleton( const char *typeName, QObject* singleton )
    {
        auto callback =
            [] ( QQmlEngine*, QJSEngine* )
            {
                QQmlEngine::setObjectOwnership( singleton, QQmlEngine::CppOwnership );
                return singleton;
            };

        return qmlRegisterSingletonType< T >( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, typeName, callback );
    }
#else
    template < typename T >
    inline int registerSingleton( const char *typeName, QObject* singleton )
    {
        return qmlRegisterSingletonInstance( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, typeName, singleton );
    }
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )

#include <qloggingcategory.h>

namespace
{
    class WarningBlocker
    {
      public:
        WarningBlocker()
        {
            m_oldFilter = QLoggingCategory::installFilter( &WarningBlocker::filter );
        }

        ~WarningBlocker()
        {
            QLoggingCategory::installFilter( m_oldFilter );
        }

      private:

        static void filter( QLoggingCategory* category )
        {
            if ( qstrcmp( category->categoryName(), "qt.qml.typeregistration" ) == 0 )
            {
                category->setEnabled( QtWarningMsg, false);
                return;
            }

            m_oldFilter(category);
        }

        static QLoggingCategory::CategoryFilter m_oldFilter;
    };

    QLoggingCategory::CategoryFilter WarningBlocker::m_oldFilter;
}

#endif

static inline QskGradientStop qskJSToGradientStop( const QJSValue& value )
{
    return QskGradientStop(
        value.property( QStringLiteral( "position" ) ).toNumber(),
        value.property( QStringLiteral( "color" ) ).toVariant().value< QColor >()
    );
}

void QskQml::registerTypes()
{
#if 0
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    qmlRegisterRevision< QQuickItem, 6 >( QSK_MODULE_NAME, 1, 0 );
#endif
#endif

    qmlRegisterUncreatableType< QskSetup >( QSK_MODULE_NAME, 1, 0, "Setup", QString() );
    qmlRegisterUncreatableType< QskSkin >( QSK_MODULE_NAME, 1, 0, "Skin", QString() );
    qRegisterMetaType< QskSkin* >();

    QSK_REGISTER( QskMain, "Main" );
    QSK_REGISTER( QskShortcutQml, "Shortcut" );

    QSK_REGISTER( QskWindow, "Window" );

    QSK_REGISTER( QskDialogWindow, "DialogWindow" );
    QSK_REGISTER( QskMessageWindow, "MessageWindow" );
    QSK_REGISTER( QskSelectionWindow, "SelectionWindow" );

    QSK_REGISTER( QskGridBoxQml, "GridBox" );
    QSK_REGISTER( QskLinearBoxQml, "LinearBox" );

    QSK_REGISTER( QskControl, "Control" );
    QSK_REGISTER( QskGraphicLabel, "GraphicLabel" );
    QSK_REGISTER( QskVirtualKeyboard, "VirtualKeyboard" );
    QSK_REGISTER( QskTextLabel, "TextLabel" );
    QSK_REGISTER( QskTabButton, "TabButton" );
    QSK_REGISTER( QskTabBar, "TabBar" );
    QSK_REGISTER( QskTabView, "TabView" );
    QSK_REGISTER( QskFocusIndicator, "FocusIndicator" );
    QSK_REGISTER( QskSeparator, "Separator" );
    QSK_REGISTER( QskProgressBar, "ProgressBar" );
    QSK_REGISTER( QskPushButton, "PushButton" );
    QSK_REGISTER( QskScrollView, "ScrollView" );
    QSK_REGISTER( QskScrollArea, "ScrollArea" );
    QSK_REGISTER( QskSlider, "Slider" );
    QSK_REGISTER( QskSimpleListBox, "SimpleListBox" );
    QSK_REGISTER( QskDialogButton, "DialogButton" );
    QSK_REGISTER( QskDialogButtonBox, "DialogButtonBox" );
    QSK_REGISTER( QskPopup, "Popup" );
    QSK_REGISTER( QskStatusIndicator, "StatusIndicator" );
    QSK_REGISTER( QskSubWindow, "SubWindow" );
    QSK_REGISTER( QskSubWindowArea, "SubWindowArea" );
    QSK_REGISTER( QskDialogSubWindow, "DialogSubWindow" );

    QSK_REGISTER_SINGLETON( QskDialog, "Dialog", QskDialog::instance() );

    qmlRegisterUncreatableType< QskSkin >( "Skinny.Skins", 1, 0, "Skin", QString() );

    QSK_REGISTER_FLAGS( QskQuickItem::UpdateFlag );
    QSK_REGISTER_FLAGS( QskQuickItem::UpdateFlags );
    QSK_REGISTER_FLAGS( QskSizePolicy::Policy );

    QSK_REGISTER_FLAGS( QskDialog::Actions );

    {
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        /*
            The QML engine warns about registering uncreatables with names starting with
            a capital letter. But as those classes usually appear only as scope for
            local enums in QML, we do want to have capitals. f.e.:

                - "policy.horizonalPolicy : SizePolicy::Minimum".

            Maybe we need to introduce some dummy gadgets exposing the enums
            in capital letters by using QML_FOREIGN_NAMESPACE, while the
            original gadget is exposed in lower letters. TODO ...
         */
        WarningBlocker warningBlocker;
#endif

        QSK_REGISTER_GADGET( QskBoxBorderMetrics, "BorderMetrics" );
        QSK_REGISTER_GADGET( QskBoxShapeMetrics, "Shape" );
        QSK_REGISTER_GADGET( QskShadowMetrics, "ShadowMetrics" );
        QSK_REGISTER_GADGET( QskGradient, "Gradient" );
        QSK_REGISTER_GADGET( QskGradientStop, "GradientStop" );
        QSK_REGISTER_GADGET( QskPlacementPolicy, "PlacementPolicy" );
        QSK_REGISTER_GADGET( QskIntervalF, "IntervalF" );
        QSK_REGISTER_GADGET( QskLayoutMetrics, "LayoutMetrics" );
        QSK_REGISTER_GADGET( QskSizePolicy, "SizePolicy" );
        QSK_REGISTER_GADGET( QskTextOptions, "TextOptions" );
        QSK_REGISTER_GADGET( QskMargins, "Margins" );
        QSK_REGISTER_GADGET( QskAspect, "Aspect" );
    }

    QSK_REGISTER_NAMESPACE( QskStandardSymbol, "StandardSymbol" );

    QMetaType::registerConverter< QJSValue, QskGradientStop >( qskJSToGradientStop );

#if QT_VERSION < QT_VERSION_CHECK( 6, 2, 0 )
    // how to do this with >= 6.2 TODO ...
    QQmlMetaType::registerCustomStringConverter( qMetaTypeId< QskMargins >(),
        []( const QString& s ) { return QVariant::fromValue( QskMargins( s.toDouble() ) ); } );
#endif
}
