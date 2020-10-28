/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskQml.h"
#include "QskLayoutQml.h"
#include "QskShortcutQml.h"

#include <QskCorner.h>
#include <QskDialog.h>
#include <QskDialogButton.h>
#include <QskDialogButtonBox.h>
#include <QskDialogSubWindow.h>
#include <QskDialogWindow.h>
#include <QskFocusIndicator.h>
#include <QskGradient.h>
#include <QskGraphicLabel.h>
#include <QskIntervalF.h>
#include <QskLayoutHint.h>
#include <QskMessageWindow.h>
#include <QskPopup.h>
#include <QskProgressBar.h>
#include <QskPushButton.h>
#include <QskRgbValue.h>
#include <QskScrollArea.h>
#include <QskScrollView.h>
#include <QskSelectionWindow.h>
#include <QskSeparator.h>
#include <QskSetup.h>
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


#include <qjsvalueiterator.h>
#include <qmargins.h>
#include <qstringlist.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qqmlglobal_p.h>
#include <private/qqmlmetatype_p.h>
#include <private/qvariantanimation_p.h>
QSK_QT_PRIVATE_END

#define QSK_MODULE_NAME "Skinny"

#define QSK_REGISTER( className, typeName ) \
    qmlRegisterType< className >( QSK_MODULE_NAME, 1, 0, typeName );

#define QSK_REGISTER_GADGET( className, typeName ) \
    qRegisterMetaType< className >(); \
    qmlRegisterUncreatableType< className >( QSK_MODULE_NAME, 1, 0, typeName, QString() )

// Required for QFlags to be constructed from an enum value
#define QSK_REGISTER_FLAGS( Type ) \
    QMetaType::registerConverter< int, Type >([] ( int value ) \
    { return Type( value ); })

#define QSK_REGISTER_SINGLETON( className, typeName, singleton ) \
    qmlRegisterSingletonType< className >( QSK_MODULE_NAME, 1, 0, typeName, \
        [] ( QQmlEngine*, QJSEngine* ) { return dynamic_cast< QObject* >( singleton ); } )

#if 0
// Utility for exposing a namespaced enum to QML, copied from qmlRegisterUncreatableType
#define QSK_REGISTER_NS_ENUM( namespaceName, enumName, typeName ) \
{ \
    namespace T = namespaceName; \
    QML_GETTYPENAMES \
    QQmlPrivate::RegisterType type = \
    { \
        1, qRegisterNormalizedMetaType< T::enumName* >( pointerName.constData() ), \
        qRegisterNormalizedMetaType< QQmlListProperty< T::enumName > >( listName.constData() ), \
        0, nullptr, QString(), QSK_MODULE_NAME, 1, 0, typeName, &T::staticMetaObject, \
        nullptr, nullptr, -1, -1, -1, nullptr, nullptr, nullptr, 0 \
    }; \
    (void) QQmlPrivate::qmlregister(QQmlPrivate::TypeRegistration, &type); \
    (void) qRegisterMetaType< namespaceName::enumName >(); \
}
#endif

// Utility for exposing a value type which is missing Q_GADGET, like QMarginsF
template< typename Type, typename Gadget >
class ValueProvider : public QQmlValueTypeProvider
{
  public:
    const QMetaObject* getMetaObjectForMetaType( int id ) override
    {
        if ( id == qMetaTypeId< Type >() )
            return &Gadget::staticMetaObject;
        return nullptr;
    }
};

// Expose values in QskRgbValue to QML
struct QskRgbValue_Gadget
{
    enum Enum
    {
#define RGB( name, value ) name = value,
        QSK_RGB_VALUES
#undef RGB
    };

    Q_ENUM( Enum )
    Q_GADGET
};

// Out-of-band meta object provider for QMarginsF
// This is required because QMarginsF doesn't have the Q_GADGET macro, and must
// therefore be treated like similar classes QRectF/QSizeF/etc.

class QMarginsF_Gadget : public QMarginsF
{
    Q_GADGET
    Q_PROPERTY( qreal left READ left WRITE setLeft )
    Q_PROPERTY( qreal top READ top WRITE setTop )
    Q_PROPERTY( qreal right READ right WRITE setRight )
    Q_PROPERTY( qreal bottom READ bottom WRITE setBottom )
};
Q_DECLARE_METATYPE( QMarginsF )

// Use this pattern to provide valueOf() to any type, something which is needed
// in JS to convert a variant to a JS value. This would be a template, but moc
// doesn't support template classes.

class QskSetupFlagsProvider : public QskSetup::Flags
{
    Q_GADGET
  public:
    template< typename... Args >
    QskSetupFlagsProvider( Args&&... args )
        : QskSetup::Flags( std::forward< Args >( args )... )
    {
    }

    Q_INVOKABLE int valueOf() const { return int( *this ); }
};

class QskMain : public QObject
{
  public:
    Q_OBJECT

    Q_PRIVATE_PROPERTY( setup(), QString skin READ skinName
        WRITE setSkin NOTIFY skinChanged )

    Q_PROPERTY( QStringList skinList READ skinList NOTIFY skinListChanged )

    Q_PRIVATE_PROPERTY( setup(), QskSetupFlagsProvider controlFlags
        READ controlFlags WRITE setControlFlags NOTIFY controlFlagsChanged )

    Q_PROPERTY( QQmlListProperty< QObject > data READ data )
    Q_CLASSINFO( "DefaultProperty", "data" )

  public:
    QskMain( QObject* parent = nullptr )
        : QObject( parent )
    {
        // how to supress warnings about a missing skinListChanged
        // as we don't have it ??

        connect( setup(), &QskSetup::skinChanged,
            this, &QskMain::skinChanged, Qt::QueuedConnection );

        connect( setup(), &QskSetup::controlFlagsChanged,
            this, &QskMain::controlFlagsChanged, Qt::QueuedConnection );
    }

    QStringList skinList() const
    {
        auto manager = QskSkinManager::instance();
        return manager ? manager->skinNames() : QStringList();
    }

    QQmlListProperty< QObject > data()
    {
        return QQmlListProperty< QObject >(
            this, nullptr,
            []( QQmlListProperty< QObject >* property, QObject* value )
            {
                QskMain* main = static_cast< QskMain* >( property->object );
                main->m_data.append( value );
            },
            []( QQmlListProperty< QObject >* property )
            {
                const QskMain* main = static_cast< const QskMain* >( property->object );
                return main->m_data.count();
            },
            []( QQmlListProperty< QObject >* property, int index )
            {
                const QskMain* main = static_cast< const QskMain* >( property->object );
                return main->m_data.at( index );
            },
            []( QQmlListProperty< QObject >* property )
            {
                QskMain* main = static_cast< QskMain* >( property->object );
                main->m_data.clear();
            }
        );
    }

  Q_SIGNALS:
    void skinListChanged(); // never emitted
    void skinChanged();
    void inputPanelChanged();
    void controlFlagsChanged();

  private:
    static inline QskSetup* setup() { return QskSetup::instance(); }

    QObjectList m_data;
};

Q_DECLARE_METATYPE( QskSetupFlagsProvider )

void QskQml::registerTypes()
{
    qmlRegisterRevision< QQuickItem, 6 >( QSK_MODULE_NAME, 1, 0 );

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

    QSK_REGISTER_FLAGS( QskControl::Flag );
    QSK_REGISTER_FLAGS( QskControl::Flags );
    QSK_REGISTER_FLAGS( QskSetup::Flag );
    QSK_REGISTER_FLAGS( QskSetup::Flags );
    QSK_REGISTER_FLAGS( QskSizePolicy::Policy );
    QMetaType::registerConverter< int, QskSetupFlagsProvider >();

    QSK_REGISTER_FLAGS( QskDialog::Actions );

    QSK_REGISTER_GADGET( QskRgbValue_Gadget, "RgbValue" );
    QSK_REGISTER_GADGET( QskStandardSymbol, "StandardSymbol" );
    QSK_REGISTER_GADGET( QskCorner, "Corner" );
    QSK_REGISTER_GADGET( QskGradient, "Gradient" );
    QSK_REGISTER_GADGET( QskGradientStop, "GradientStop" );
    QSK_REGISTER_GADGET( QskIntervalF, "IntervalF" );
    QSK_REGISTER_GADGET( QskLayoutHint, "LayoutHint" );
    QSK_REGISTER_GADGET( QskSizePolicy, "SizePolicy" );
    QSK_REGISTER_GADGET( QskTextOptions, "TextOptions" );

    // Support (lists of) GradientStop
    QMetaType::registerConverter< QJSValue, QskGradientStop >(

        [](const QJSValue& value) -> QskGradientStop
        {
            return
            {
                value.property( QLatin1String( "position" ) ).toNumber(),
                value.property( QLatin1String( "color" ) ).toVariant().value< QColor >()
            };
        }
    );

    QMetaType::registerConverter< QJSValue, QVector< QskGradientStop > >(

        []( const QJSValue& value )
        {
            QVector< QskGradientStop > stops;
            if ( value.isArray() )
            {
                QJSValueIterator it( value );
                while ( it.next() && it.hasNext() )
                {
                    auto source = it.value();
                    auto target = QskGradientStop();

                    const int sourceTypeId = qMetaTypeId< decltype( source ) >();
                    const int targetTypeId = qMetaTypeId< decltype( target ) >();

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
                    QMetaType::convert( QMetaType( sourceTypeId ), &source,
                        QMetaType( targetTypeId ), &target );
#else
                    QMetaType::convert( &source, sourceTypeId, &target, targetTypeId );
#endif
                    stops.append( target );
                }
            }
            return stops;
        }
    );

    // Support QMarginsF in QML
    QQml_addValueTypeProvider( new ValueProvider< QMarginsF, QMarginsF_Gadget >() );

    QQmlMetaType::registerCustomStringConverter(
        qMetaTypeId< QMarginsF >(),
        []( const QString& rhs )
        {
            auto margin = rhs.toDouble();
            return QVariant::fromValue( QMarginsF( margin, margin, margin, margin ) );
        }
    );

    QMetaType::registerConverter< int, QMarginsF >(
        []( int value ) { return QMarginsF( value, value, value, value ); } );

    QMetaType::registerConverter< qreal, QMarginsF >(
        []( qreal value ) { return QMarginsF( value, value, value, value ); } );

        qRegisterAnimationInterpolator< QMarginsF >(
        []( const QMarginsF& from, const QMarginsF& to, qreal progress )
        {
            return QVariant::fromValue( QMarginsF(
                _q_interpolate( from.left(), to.left(), progress ),
                _q_interpolate( from.top(), to.top(), progress ),
                _q_interpolate( from.right(), to.right(), progress ),
                _q_interpolate( from.bottom(), to.bottom(), progress )
            ) );
        }
    );

    // Support QskSizePolicy in QML user properties
    QMetaType::registerConverter< QJSValue, QskSizePolicy >(
        []( const QJSValue& value )
        {
            return QskSizePolicy(
                static_cast< QskSizePolicy::Policy >( value.property( 0 ).toInt() ),
                static_cast< QskSizePolicy::Policy >( value.property( 1 ).toInt() ) );
        }
    );

    QMetaType::registerConverter< int, QskSizePolicy >(
        []( int value )
        {
            const auto policy = static_cast< QskSizePolicy::Policy >( value );
            return QskSizePolicy( policy, policy );
        }
    );
}

#include "QskQml.moc"
