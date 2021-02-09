/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSetup.h"
#include "QskControl.h"
#include "QskControlPrivate.h"
#include "QskGraphicProviderMap.h"
#include "QskObjectTree.h"
#include "QskSkin.h"
#include "QskSkinManager.h"
#include "QskWindow.h"

#include <qguiapplication.h>
#include <qpointer.h>
#include <qstylehints.h>

QskSetup* QskSetup::s_instance = nullptr;

static inline bool qskHasEnvironment( const char* env )
{
    bool ok;

    const int value = qEnvironmentVariableIntValue( env, &ok );
    if ( ok )
        return value != 0;

    // All other strings are true, apart from "false"
    auto result = qgetenv( env );
    return !result.isEmpty() && result != "false";
}

static inline const QskQuickItem::UpdateFlags qskEnvironmentUpdateFlags()
{
    QskQuickItem::UpdateFlags flags;

    if ( qskHasEnvironment( "QSK_PREFER_RASTER" ) )
        flags |= QskQuickItem::PreferRasterForTextures;

    if ( qskHasEnvironment( "QSK_FORCE_BACKGROUND" ) )
        flags |= QskQuickItem::DebugForceBackground;

    return flags;
}

static inline QskQuickItem::UpdateFlags qskDefaultUpdateFlags()
{
    static QskQuickItem::UpdateFlags flags;

    if ( flags == 0 )
    {
        flags |= QskQuickItem::DeferredUpdate;
        flags |= QskQuickItem::DeferredPolish;
        flags |= QskQuickItem::DeferredLayout;
        flags |= QskQuickItem::CleanupOnVisibility;
        flags |= qskEnvironmentUpdateFlags();
    }

    return flags;
}

static void qskApplicationHook()
{
    QskSetup::setup();
    qAddPostRoutine( QskSetup::cleanup );
}

static void qskApplicationFilter()
{
    QCoreApplication::instance()->installEventFilter( QskSetup::instance() );
}

Q_CONSTRUCTOR_FUNCTION( qskApplicationHook )
Q_COREAPP_STARTUP_FUNCTION( qskApplicationFilter )

extern bool qskInheritLocale( QskWindow*, const QLocale& );

namespace
{
    class VisitorLocale final : public QskObjectTree::ResolveVisitor< QLocale >
    {
      public:
        VisitorLocale()
            : ResolveVisitor< QLocale >( "locale" )
        {
        }

      private:
        bool setImplicitValue( QskControl* control,
            const QLocale& locale ) override
        {
            return QskControlPrivate::inheritLocale( control, locale );
        }

        bool setImplicitValue( QskWindow* window,
            const QLocale& locale ) override
        {
            return qskInheritLocale( window, locale );
        }

        QLocale value( const QskControl* control ) const override
        {
            return control->locale();
        }

        QLocale value( const QskWindow* window ) const override
        {
            return window->locale();
        }
    };
}

class QskSetup::PrivateData
{
  public:
    PrivateData()
        : itemUpdateFlags( qskDefaultUpdateFlags() )
    {
    }

    QString skinName;
    QPointer< QskSkin > skin;

    QskGraphicProviderMap graphicProviders;
    QskQuickItem::UpdateFlags itemUpdateFlags;
};

QskSetup::QskSetup()
    : m_data( new PrivateData() )
{
}

QskSetup::~QskSetup()
{
    s_instance = nullptr; // we might be destroyed from Qml !
}

void QskSetup::setup()
{
    if ( s_instance == nullptr )
        s_instance = new QskSetup();
}

void QskSetup::cleanup()
{
    delete s_instance;
    s_instance = nullptr;
}

void QskSetup::setItemUpdateFlags( QskQuickItem::UpdateFlags flags )
{
    if ( m_data->itemUpdateFlags != flags )
    {
        m_data->itemUpdateFlags = flags;
        Q_EMIT itemUpdateFlagsChanged();
    }
}

QskQuickItem::UpdateFlags QskSetup::itemUpdateFlags() const
{
    return m_data->itemUpdateFlags;
}

void QskSetup::resetItemUpdateFlags()
{
    setItemUpdateFlags( qskDefaultUpdateFlags() );
}

void QskSetup::setItemUpdateFlag( QskQuickItem::UpdateFlag flag, bool on )
{
    if ( m_data->itemUpdateFlags.testFlag( flag ) != on )
    {
        if ( on )
            m_data->itemUpdateFlags |= flag;
        else
            m_data->itemUpdateFlags &= ~flag;

        Q_EMIT itemUpdateFlagsChanged();
    }
}

void QskSetup::resetItemUpdateFlag( QskQuickItem::UpdateFlag flag )
{
    setItemUpdateFlag( flag, flag & qskDefaultUpdateFlags() );
}

bool QskSetup::testItemUpdateFlag( QskQuickItem::UpdateFlag flag )
{
    return m_data->itemUpdateFlags.testFlag( flag );
}

QskSkin* QskSetup::setSkin( const QString& skinName )
{
    if ( m_data->skin && ( skinName == m_data->skinName ) )
        return m_data->skin;

    QskSkin* skin = QskSkinManager::instance()->createSkin( skinName );
    if ( skin == nullptr )
        return nullptr;

    if ( skin->parent() == nullptr )
        skin->setParent( this );

    const QskSkin* oldSkin = m_data->skin;

    m_data->skin = skin;
    m_data->skinName = skinName;

    if ( oldSkin )
    {
        Q_EMIT skinChanged( skin );

        if ( oldSkin->parent() == this )
            delete oldSkin;
    }

    return m_data->skin;
}

QString QskSetup::skinName() const
{
    return m_data->skinName;
}

QskSkin* QskSetup::skin()
{
    if ( m_data->skin == nullptr )
    {
        m_data->skin = QskSkinManager::instance()->createSkin( QString() );
        Q_ASSERT( m_data->skin );

        m_data->skin->setParent( this );
        m_data->skinName = m_data->skin->objectName();
    }

    return m_data->skin;
}

void QskSetup::addGraphicProvider( const QString& providerId, QskGraphicProvider* provider )
{
    m_data->graphicProviders.insert( providerId, provider );
}

QskGraphicProvider* QskSetup::graphicProvider( const QString& providerId ) const
{
    if ( m_data->skin )
    {
        QskGraphicProvider* provider = m_data->skin->graphicProvider( providerId );
        if ( provider )
            return provider;
    }

    return m_data->graphicProviders.provider( providerId );
}

QLocale QskSetup::inheritedLocale( const QObject* object )
{
    VisitorLocale visitor;
    visitor.setResolveValue( QLocale() );

    QskObjectTree::traverseUp( const_cast< QObject* >( object ), visitor );
    return visitor.resolveValue();
}

void QskSetup::inheritLocale( QObject* object, const QLocale& locale )
{
    VisitorLocale visitor;
    visitor.setResolveValue( locale );

    QskObjectTree::traverseDown( object, visitor );
}

bool QskSetup::eventFilter( QObject* object, QEvent* event )
{
    if ( auto control = qskControlCast( object ) )
    {
        /*
            Qt::FocusPolicy has always been there with widgets, got lost with
            Qt/Quick and has been reintroduced with Qt/Quick Controls 2 ( QC2 ).
            Unfortunately this was done once more by adding code on top instead
            of fixing the foundation.

            But we also don't want to have how it is done in QC2 by adding
            the focus management in the event handler of the base class.
            This implementation reverts the expected default behaviour of when
            events are accepted/ignored + is an error prone nightmare, when it
            comes to overloading event handlers missing to call the base class.

            That's why we prefer to do the focus management outside of the
            event handlers.
         */
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            {
                if ( ( control->focusPolicy() & Qt::ClickFocus ) == Qt::ClickFocus )
                {
                    const bool focusOnRelease =
                        QGuiApplication::styleHints()->setFocusOnTouchRelease();

                    if ( focusOnRelease )
                    {
                        if ( event->type() == QEvent::MouseButtonRelease )
                            control->forceActiveFocus( Qt::MouseFocusReason );
                    }
                    else
                    {
                        if ( event->type() == QEvent::MouseButtonPress )
                            control->forceActiveFocus( Qt::MouseFocusReason );
                    }
                }
                break;
            }
            case QEvent::Wheel:
            {
                if ( !control->isWheelEnabled() )
                {
                    /*
                        We block further processing of the event. This is in line
                        with not receiving any mouse event that have not been
                        explicitly enabled with setAcceptedMouseButtons().

                     */
                    event->ignore();
                    return true;
                }

                if ( ( control->focusPolicy() & Qt::WheelFocus ) == Qt::WheelFocus )
                    control->forceActiveFocus( Qt::MouseFocusReason );

                break;
            }
            default:
                break;
        }
    }

    return false;
}

QskSetup* QskSetup::qmlAttachedProperties( QObject* )
{
    return QskSetup::instance();
}

Q_DECLARE_METATYPE( QskSkin* )
#include "moc_QskSetup.cpp"
