/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTextInput.h"
#include "QskQuick.h"
#include "QskInternalMacros.h"

QSK_QT_PRIVATE_BEGIN
#include <private/qquicktextinput_p.h>
#include <private/qquicktextinput_p_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskTextInput, Text )
QSK_SUBCONTROL( QskTextInput, TextPanel )
QSK_SYSTEM_STATE( QskTextInput, Error, QskAspect::FirstSystemState << 4 )

/*
    Other properties offered from QQuickTextInput:

    - accepted();

      What is this one good for ?

    - textEdited();

      TODO ...

    - passwordCharacterChanged();
    - passwordMaskDelayChanged(int delay);

      Maybe we will have a QskPasswordField class, where we export
      these properties.
 */
namespace
{
    class QuickTextInput final : public QQuickTextInput
    {
        Q_OBJECT

        using Inherited = QQuickTextInput;

      public:
        QuickTextInput( QskTextInput* );

        inline void setAlignment( Qt::Alignment alignment )
        {
            setHAlign( ( HAlignment ) ( int( alignment ) & 0x0f ) );
            setVAlign( ( VAlignment ) ( int( alignment ) & 0xf0 ) );
        }

        Q_INVOKABLE bool fixup();
        Q_INVOKABLE void updateColors();
        Q_INVOKABLE void updateMetrics();
        Q_INVOKABLE void setEditing( bool );
        Q_INVOKABLE void handleEvent( QEvent* );

      protected:

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        void geometryChange(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChange( newGeometry, oldGeometry );
            updateClip();
        }
#else
        void geometryChanged(
            const QRectF& newGeometry, const QRectF& oldGeometry ) override
        {
            Inherited::geometryChanged( newGeometry, oldGeometry );
            updateClip();
        }
#endif

        void updateClip()
        {
            setClip( ( contentWidth() > width() ) ||
                ( contentHeight() > height() ) );
        }

        QSGNode* updatePaintNode(
            QSGNode* oldNode, UpdatePaintNodeData* data ) override
        {
            updateColors();
            return Inherited::updatePaintNode( oldNode, data );
        }
    };

    QuickTextInput::QuickTextInput( QskTextInput* textInput )
        : QQuickTextInput( textInput )
    {
        classBegin();

        setActiveFocusOnTab( false );
        setFlag( ItemAcceptsInputMethod, false );
        setFocusOnPress( false );
        setSelectByMouse( true );

        componentComplete();

        connect( this, &QQuickTextInput::contentSizeChanged,
            this, &QuickTextInput::updateClip );
    }

    void QuickTextInput::setEditing( bool on )
    {
        Q_ASSERT( focusOnPress() == false );

        QFocusEvent event( on ? QEvent::FocusIn : QEvent::FocusOut );
        QQuickTextInputPrivate::get( this )->handleFocusEvent( &event );
    }

    bool QuickTextInput::fixup()
    {
        return QQuickTextInputPrivate::get( this )->fixup();
    }

    void QuickTextInput::updateMetrics()
    {
        auto textInput = static_cast< const QskTextInput* >( parentItem() );

        setAlignment( textInput->alignment() );
        setFont( textInput->font() );
    }

    void QuickTextInput::updateColors()
    {
        using Q = QskTextInput;

        auto input = static_cast< const QskTextInput* >( parentItem() );

        setColor( input->color( Q::Text ) );

        const auto state = QskTextInput::Selected;

        setSelectionColor( input->color( Q::TextPanel | state ) );
        setSelectedTextColor( input->color( Q::Text | state ) );
    }

    void QuickTextInput::handleEvent( QEvent* ev )
    {
        event( ev );
    }
}

class QskTextInput::PrivateData
{
  public:
    QuickTextInput* wrappedInput;
};

QskTextInput::QskTextInput( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    m_data->wrappedInput = new QuickTextInput( this );
    auto wrappedInput = m_data->wrappedInput;

    setAcceptedMouseButtons( wrappedInput->acceptedMouseButtons() );
    wrappedInput->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Minimum );

    setup( wrappedInput );

    connect( wrappedInput, &QQuickTextInput::maximumLengthChanged,
        this, &QskTextInput::maximumLengthChanged );

    connect( wrappedInput, &QQuickTextInput::displayTextChanged,
        this, &QskTextInput::displayTextChanged );

    connect( wrappedInput, &QQuickTextInput::inputMaskChanged,
        this, &QskTextInput::inputMaskChanged );

    connect( wrappedInput, &QQuickTextInput::acceptableInputChanged,
        this, [this]() { Q_EMIT acceptableInputChanged( hasAcceptableInput() ); } );
}

QskTextInput::~QskTextInput()
{
}

QskAspect::Subcontrol QskTextInput::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == Inherited::Text )
        return Text;

    if ( subControl == Inherited::TextPanel )
        return TextPanel;

    return Inherited::substitutedSubcontrol( subControl );
}

void QskTextInput::ensureVisible( int position )
{
    m_data->wrappedInput->ensureVisible( position );
}

int QskTextInput::maxLength() const
{
    return m_data->wrappedInput->maxLength();
}

void QskTextInput::setMaxLength( int length )
{
    m_data->wrappedInput->setMaxLength( length );
}

QValidator* QskTextInput::validator() const
{
    return m_data->wrappedInput->validator();
}

void QskTextInput::setValidator( QValidator* validator )
{
    if ( validator != m_data->wrappedInput->validator() )
    {
        m_data->wrappedInput->setValidator( validator );
        Q_EMIT validatorChanged( validator );
    }
}

QString QskTextInput::inputMask() const
{
    return m_data->wrappedInput->inputMask();
}

void QskTextInput::setInputMask( const QString& mask )
{
    m_data->wrappedInput->setInputMask( mask );
}

bool QskTextInput::autoScroll() const
{
    return m_data->wrappedInput->autoScroll();
}

void QskTextInput::setAutoScroll( bool on )
{
    if ( m_data->wrappedInput->autoScroll() != on )
    {
        m_data->wrappedInput->setAutoScroll( on );
        Q_EMIT autoScrollChanged( on );
    }
}

QskTextInput::EchoMode QskTextInput::echoMode() const
{
    const auto mode = m_data->wrappedInput->echoMode();
    return static_cast< QskTextInput::EchoMode >( mode );
}

void QskTextInput::setEchoMode( EchoMode mode )
{
    if ( mode != echoMode() )
    {
        m_data->wrappedInput->setEchoMode(
            static_cast< QQuickTextInput::EchoMode >( mode ) );

        qskUpdateInputMethod( this, Qt::ImHints );

        Q_EMIT echoModeChanged( mode );
    }
}

QString QskTextInput::passwordCharacter() const
{
    return m_data->wrappedInput->passwordCharacter();
}

void QskTextInput::setPasswordCharacter( const QString& text )
{
    m_data->wrappedInput->setPasswordCharacter( text );
}

void QskTextInput::resetPasswordCharacter()
{
    m_data->wrappedInput->setPasswordCharacter(
        QGuiApplication::styleHints()->passwordMaskCharacter() );
}

int QskTextInput::passwordMaskDelay() const
{
    return m_data->wrappedInput->passwordMaskDelay();
}

void QskTextInput::setPasswordMaskDelay( int ms )
{
    m_data->wrappedInput->setPasswordMaskDelay( ms );
}

void QskTextInput::resetPasswordMaskDelay()
{
    m_data->wrappedInput->resetPasswordMaskDelay();
}

QString QskTextInput::displayText() const
{
    return m_data->wrappedInput->displayText();
}

bool QskTextInput::hasAcceptableInput() const
{
    /*
        We might want to make visual adjustments while having
        an "invalid" text. Don't we need a QSkinny state
        for this: TODO ...
     */
    return m_data->wrappedInput->hasAcceptableInput();
}

bool QskTextInput::fixup()
{
    return m_data->wrappedInput->fixup();
}

#include "QskTextInput.moc"
#include "moc_QskTextInput.cpp"
