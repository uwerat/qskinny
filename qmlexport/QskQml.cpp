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

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
#include <qqmlengine.h>
#endif

#if QT_VERSION < QT_VERSION_CHECK( 6, 2, 0 )
QSK_QT_PRIVATE_BEGIN
#include <private/qqmlmetatype_p.h>
QSK_QT_PRIVATE_END
#endif

#include <ctype.h>

#define QSK_MODULE_NAME "Skinny"
#define QSK_VERSION_MAJOR 1
#define QSK_VERSION_MINOR 0

#if QT_VERSION < QT_VERSION_CHECK( 6, 3, 0 )
    #define QSK_STRUCT_VERSION 0
#elif QT_VERSION < QT_VERSION_CHECK( 6, 5, 0 )
    #define QSK_STRUCT_VERSION 1
#else
    #define QSK_STRUCT_VERSION 2
#endif

// Required for QFlags to be constructed from an enum value
#define QSK_REGISTER_FLAGS( Type ) \
    QMetaType::registerConverter< int, Type >( []( int value ) { return Type( value ); } )

namespace
{
    inline const char* classNameQml( const QMetaObject& metaObject )
    {
        // without the "Qsk" prefix
        return metaObject.className() + 3;
    }

    /*
        ClassInfo corresponds to the most reecent QQmlPrivate::RegisterType
        ( structVersion: 2 introduced with Qt 6.5 )
     */
    class ClassInfo
    {
      public:

        template< typename T >
        void setTypeInfo()
        {
            using namespace QQmlPrivate;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            const char* className = T::staticMetaObject.className(); \

            const int nameLen = int(strlen(className) ); \
            const int listLen = int(strlen("QQmlListProperty<") ); \

            QVarLengthArray< char,64 > listName(listLen + nameLen + 2); \
            memcpy(listName.data(), "QQmlListProperty<", size_t(listLen) ); \
            memcpy(listName.data() + listLen, className, size_t(nameLen) ); \
            listName[listLen + nameLen] = '>'; \
            listName[listLen + nameLen + 1] = '\0';

            typeId = qMetaTypeId< T* >( );
            listId = qRegisterNormalizedMetaType< QQmlListProperty< T > >( listName.constData() );
#else
            if constexpr (std::is_base_of_v< QObject, T >)
            {
                typeId = QMetaType::fromType< T* >( );
                listId = QMetaType::fromType< QQmlListProperty< T > >( );
            }
            else
            {
                typeId = QMetaType::fromType< T >( );
                listId = QMetaType::fromType< QList< T > >( );
            }

            createValueType = ValueType< T, void >::create;
#endif


            parserStatusCast = StaticCastSelector< T,QQmlParserStatus >::cast();
            valueSourceCast = StaticCastSelector< T,QQmlPropertyValueSource >::cast();
            valueInterceptorCast = StaticCastSelector< T,QQmlPropertyValueInterceptor >::cast();
#if QSK_STRUCT_VERSION >= 1
            finalizerCast = StaticCastSelector< T,QQmlFinalizerHook >::cast();
#endif
        }

      public:
        const int structVersion = QSK_STRUCT_VERSION;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        QMetaType typeId;
        QMetaType listId;
#else
        int typeId = 0;
        int listId = 0;
#endif

        int objectSize = 0;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        void ( *create )( void* ) = nullptr;
#else
        void ( *create )( void*, void* ) = nullptr;
        void* const userdata = nullptr; // unused
#endif

        const QString noCreationReason; // unused

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        /*
            This one was introdued with Qt 6.x, but never worked
            as expected. With Qt 6.5 it has been replaced by adding
            the creationMethod that is triggering to look for
            invokable constructors.
            Let's check if it makes any sense to initialize it below
	        at all. TODO ...
         */
        QVariant ( *createValueType )( const QJSValue& ) = nullptr;
#endif

        const char* const uri = QSK_MODULE_NAME;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        const QTypeRevision version =
            QTypeRevision::fromVersion( QSK_VERSION_MAJOR, QSK_VERSION_MINOR );
#else
        const int versionMajor = QSK_VERSION_MAJOR;
        const int versionMinor = QSK_VERSION_MINOR;
#endif
        const char* elementName = nullptr;
        const QMetaObject* metaObject = nullptr;

        /*
            We do not use attached properties as it always comes with
            creating extra QObjects.
         */
        QObject* (* const attachedPropertiesFunction)( QObject* ) = nullptr;
        const QMetaObject* const attachedPropertiesMetaObject = nullptr;

        int parserStatusCast = -1;
        int valueSourceCast = -1;
        int valueInterceptorCast = -1;

        /*
            We do not use extensions as it always comes with
            creating extra QObjects.
         */
        QObject* (* const extensionObjectCreate )( QObject* ) = nullptr;
        const QMetaObject* const extensionMetaObject = nullptr;

        QQmlCustomParser* const customParser = nullptr; // unused

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        const QTypeRevision revision = QTypeRevision::zero();
#else
        const int revision = 0;
#endif
        int finalizerCast = -1;

        const int creationMethod = 2; // ValueTypeCreationMethod::Structured
    };

    template< typename T >
    inline int registerType( const char* qmlName )
    {
        using namespace QQmlPrivate;

        ClassInfo type;

        type.setTypeInfo< T >();

        type.objectSize = sizeof( T );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        type.create = Constructors< T >::createInto;
#else
        type.create = createInto< T >;
#endif

        type.elementName = qmlName;
        type.metaObject = & T::staticMetaObject;

        return qmlregister( TypeRegistration, & type );
    }

    template< typename T >
    inline int registerUncreatableType( const char* qmlName )
    {
        using namespace QQmlPrivate;

        ClassInfo type;

        type.setTypeInfo< T >( );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        type.objectSize = sizeof( T );
        type.create = Constructors< T >::createInto;
#endif

        type.elementName = qmlName;
        type.metaObject = & T::staticMetaObject;

        return qmlregister( TypeRegistration, & type );
    }

    int registerUncreatableMetaObject(
        const QMetaObject& staticMetaObject, const char* qmlName )
    {
        using namespace QQmlPrivate;

        ClassInfo type;

        type.elementName = qmlName;
        type.metaObject = & staticMetaObject;

        return qmlregister( TypeRegistration, & type );
    }

    template< typename T >
    inline void registerObject( const char* qmlName = nullptr )
    {
        // the class name without the "Qsk" prefix
        if ( qmlName == nullptr )
            qmlName = classNameQml( T::staticMetaObject );

        ( void ) registerType< T >( qmlName );
    }

    template< typename T >
    inline void registerGadget()
    {
        auto className = classNameQml( T::staticMetaObject );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
        registerUncreatableType< T >( className );
#else
        /*
            According to the QML naming rules uncreatables have to
            start with a lowercase letter ( since Qt6 ), while namespaces
            and creatable items usually start with a upper letter.
            This results in an odd naming scheme for the enums defined inside of gadgets.

            To work around this we register the gadget twice  - starting with
            upper or lower letter.

            Maybe it would make sense to only pass stripped metaObjects, where all
            enums are removed from the first and everything else than the enums from
            the second. TODO ...
         */

        if ( T::staticMetaObject.enumeratorCount() > 0 )
        {
            registerUncreatableMetaObject( T::staticMetaObject, className );
        }

        QByteArray name = className;
        name.data()[0] = std::tolower( name.data()[0] );
        registerUncreatableType< T >( name.constData() );
#endif
    }

    inline int registerNamespace( const QMetaObject& metaObject )
    {
        return registerUncreatableMetaObject( metaObject, classNameQml( metaObject ) );
    }

    template< typename T >
    inline int registerSingleton( QObject* singleton )
    {
        const auto name = classNameQml( T::staticMetaObject );

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
        auto callback =
            []( QQmlEngine*, QJSEngine* )
            {
                QQmlEngine::setObjectOwnership( singleton, QQmlEngine::CppOwnership );
                return singleton;
            };

        return qmlRegisterSingletonType< T >( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, name, callback );
#else
        return qmlRegisterSingletonInstance( QSK_MODULE_NAME,
            QSK_VERSION_MAJOR, QSK_VERSION_MINOR, name, singleton );
#endif
    }
}

static inline QskGradientStop qskJSToGradientStop( const QJSValue& value )
{
    return QskGradientStop(
        value.property( QStringLiteral( "position" ) ).toNumber(),
        value.property( QStringLiteral( "color" ) ).toVariant().value< QColor >()
    );
}

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

    QMetaType::registerConverter< QJSValue, QskGradientStop >( qskJSToGradientStop );

#if QT_VERSION < QT_VERSION_CHECK( 6, 2, 0 )
    // how to do this with >= 6.2 TODO ...
    QQmlMetaType::registerCustomStringConverter( qMetaTypeId< QskMargins >(),
        []( const QString& s ) { return QVariant::fromValue( QskMargins( s.toDouble() ) ); } );
#endif
}
