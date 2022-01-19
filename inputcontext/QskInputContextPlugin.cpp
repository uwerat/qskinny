/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include <qpa/qplatforminputcontext.h>
#include <qpa/qplatforminputcontextplugin_p.h>

#include "QskInputContext.h"

#include <QEvent>
#include <QLocale>
#include <QRectF>

/*
    QPlatformInputContext is no stable public API.
    So we forward everything to QskInputContext
 */
class QskPlatformInputContext final : public QPlatformInputContext
{
    Q_OBJECT

    using Inherited = QPlatformInputContext;

  public:
    QskPlatformInputContext();
    virtual ~QskPlatformInputContext() = default;

    bool isValid() const override;
    bool hasCapability( Capability ) const override;

    void update( Qt::InputMethodQueries ) override;
    Q_INVOKABLE void update( const QQuickItem*, Qt::InputMethodQueries );

    void invokeAction( QInputMethod::Action, int ) override;

    QRectF keyboardRect() const override;
    bool isAnimating() const override;

    void showInputPanel() override;
    void hideInputPanel() override;
    Q_INVOKABLE void setInputPanelVisible( const QQuickItem*, bool );

    bool isInputPanelVisible() const override;

    void reset() override;
    void commit() override;

    void setFocusObject( QObject* ) override;

    QLocale locale() const override;
    Qt::LayoutDirection inputDirection() const override;

    bool filterEvent( const QEvent* ) override;

  protected:
    bool event( QEvent* ) override;

  private:
    void updateContext();
    void updateLocale();

    QLocale m_locale;
    QPointer< QskInputContext > m_context;
};

QskPlatformInputContext::QskPlatformInputContext()
{
    auto context = QskInputContext::instance();
    if ( context == nullptr )
    {
        context = new QskInputContext();
        context->setFactory( new QskInputContextFactory() );
        QskInputContext::setInstance( context );
    }

    updateContext();
    updateLocale();
}

void QskPlatformInputContext::updateContext()
{
    if ( m_context )
        m_context->disconnect( this );

    m_context = QskInputContext::instance();

    if ( m_context )
    {
        connect( m_context, &QskInputContext::activeChanged,
            this, &QPlatformInputContext::emitInputPanelVisibleChanged );

        connect( m_context, &QskInputContext::panelRectChanged,
            this, &QPlatformInputContext::emitKeyboardRectChanged );
    }
}

void QskPlatformInputContext::updateLocale()
{
    if ( m_context )
    {
        const auto oldLocale = m_locale;
        m_locale = m_context->locale();

        if ( oldLocale != m_locale )
            emitLocaleChanged();

        if ( m_locale.textDirection() != oldLocale.textDirection() )
            emitInputDirectionChanged( m_locale.textDirection() );
    }
}

bool QskPlatformInputContext::isValid() const
{
    return true;
}

bool QskPlatformInputContext::hasCapability( Capability ) const
{
    // what is QPlatformInputContext::HiddenTextCapability ???
    return true;
}

void QskPlatformInputContext::update( Qt::InputMethodQueries queries )
{
    if ( m_context )
        m_context->update( nullptr, queries );
}

void QskPlatformInputContext::update(
    const QQuickItem* item, Qt::InputMethodQueries queries )
{
    if ( m_context )
        m_context->update( item, queries );
}

void QskPlatformInputContext::invokeAction(
    QInputMethod::Action action, int cursorPosition )
{
    if ( m_context )
        m_context->invokeAction( action, cursorPosition );
}

QRectF QskPlatformInputContext::keyboardRect() const
{
    if ( m_context )
        return m_context->panelRect();

    return QRectF();
}

bool QskPlatformInputContext::isAnimating() const
{
    // who is interested in this ?
    // also: emitAnimatingChanged

    return false;
}

void QskPlatformInputContext::showInputPanel()
{
    setInputPanelVisible( nullptr, true );
}

void QskPlatformInputContext::hideInputPanel()
{
    setInputPanelVisible( nullptr, false );
}

void QskPlatformInputContext::setInputPanelVisible(
    const QQuickItem* item, bool on )
{
    if ( m_context )
        m_context->setInputPanelVisible( item, on );
}

bool QskPlatformInputContext::isInputPanelVisible() const
{
    if ( m_context )
        return m_context->isInputPanelVisible();

    return false;
}

void QskPlatformInputContext::reset()
{
    if ( m_context )
        m_context->commitPrediction( false );
}

void QskPlatformInputContext::commit()
{
    if ( m_context )
        m_context->commitPrediction( true );
}

void QskPlatformInputContext::setFocusObject( QObject* object )
{
    if ( m_context )
        m_context->setFocusObject( object );
}

QLocale QskPlatformInputContext::locale() const
{
    return m_locale;
}

Qt::LayoutDirection QskPlatformInputContext::inputDirection() const
{
    return m_locale.textDirection();
}

bool QskPlatformInputContext::filterEvent( const QEvent* )
{
    // called from QXcbKeyboard, but what about other platforms
    return false;
}

bool QskPlatformInputContext::event( QEvent* event )
{
    switch ( event->type() )
    {
        case QEvent::LocaleChange:
        {
            updateLocale();
            break;
        }
        case QEvent::PlatformPanel:
        {
            updateContext();
            break;
        }
        default:
            break;
    }

    return Inherited::event( event );
}

class QskInputContextPlugin final : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA( IID QPlatformInputContextFactoryInterface_iid FILE "metadata.json" )

  public:
    QPlatformInputContext* create(
        const QString& system, const QStringList& ) override
    {
        if ( system.compare( QStringLiteral( "skinny" ), Qt::CaseInsensitive ) == 0 )
        {
            auto context = new QskPlatformInputContext();
            return context;
        }

        return nullptr;
    }
};

#include "QskInputContextPlugin.moc"
