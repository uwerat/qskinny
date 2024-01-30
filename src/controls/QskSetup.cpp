/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSetup.h"
#include "QskControl.h"
#include "QskControlPrivate.h"
#include "QskGraphicProviderMap.h"
#include "QskSkinManager.h"
#include "QskSkin.h"
#include "QskWindow.h"

#include <qguiapplication.h>
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

class QskSetup::PrivateData
{
  public:
    PrivateData()
        : itemUpdateFlags( qskDefaultUpdateFlags() )
    {
    }

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

void QskSetup::addGraphicProvider( const QString& providerId, QskGraphicProvider* provider )
{
    m_data->graphicProviders.insert( providerId, provider );
}

QskGraphicProvider* QskSetup::graphicProvider( const QString& providerId ) const
{
    if ( auto skin = qskSkinManager->skin() )
    {
        if ( auto provider = skin->graphicProvider( providerId ) )
            return provider;
    }

    return m_data->graphicProviders.provider( providerId );
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

#include "moc_QskSetup.cpp"
