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

QSK_SUBCONTROL( QskTextInput, TextPanel )
QSK_SYSTEM_STATE( QskTextInput, Error, QskAspect::FirstSystemState << 4 )

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

        bool fixup()
        {
            return QQuickTextInputPrivate::get( this )->fixup();
        }

        Q_INVOKABLE void updateColors();
        Q_INVOKABLE void updateMetrics();
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
    /*
        QQuickTextInput is a beast of almost 5k lines of code, we don't
        want to reimplement that - at least not now.
        So QskTextInput is more or less a simple wrapper making everything
        conforming to qskinny.
     */

    m_data->wrappedInput = new QuickTextInput( this );

    setAcceptedMouseButtons( m_data->wrappedInput->acceptedMouseButtons() );
    m_data->wrappedInput->setAcceptedMouseButtons( Qt::NoButton );

    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Fixed );

    setup( m_data->wrappedInput, &QQuickTextInput::staticMetaObject );
}

QskTextInput::~QskTextInput()
{
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
    m_data->wrappedInput->setValidator( validator );
}

QString QskTextInput::inputMask() const
{
    return m_data->wrappedInput->inputMask();
}

void QskTextInput::setInputMask( const QString& mask )
{
    m_data->wrappedInput->setInputMask( mask );
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
    return m_data->wrappedInput->hasAcceptableInput();
}

bool QskTextInput::fixup()
{
    return m_data->wrappedInput->fixup();
}

bool QskTextInput::acceptInput()
{
    return hasAcceptableInput() || fixup();
}

void QskTextInput::setupFrom( const QQuickItem* item )
{
    if ( item == nullptr )
        return;

    // finding attributes from the input hints of item

    int maxCharacters = 32767;
    QskTextInput::EchoMode echoMode = QskTextInput::Normal;

    Qt::InputMethodQueries queries = Qt::ImQueryAll;
    queries &= ~Qt::ImEnabled;

    QInputMethodQueryEvent event( queries );
    QCoreApplication::sendEvent( const_cast< QQuickItem* >( item ), &event );

    if ( event.queries() & Qt::ImHints )
    {
        const auto hints = static_cast< Qt::InputMethodHints >(
            event.value( Qt::ImHints ).toInt() );

        if ( hints & Qt::ImhHiddenText )
            echoMode = QskTextInput::NoEcho;
    }

    if ( event.queries() & Qt::ImMaximumTextLength )
    {
        // needs to be handled before Qt::ImCursorPosition !

        const auto max = event.value( Qt::ImMaximumTextLength ).toInt();
        maxCharacters = qBound( 0, max, maxCharacters );
    }

    setMaxLength( maxCharacters );

    if ( event.queries() & Qt::ImSurroundingText )
    {
        const auto text = event.value( Qt::ImSurroundingText ).toString();
        setText( text );
    }

    if ( event.queries() & Qt::ImCursorPosition )
    {
        const auto pos = event.value( Qt::ImCursorPosition ).toInt();
        setCursorPosition( pos );
    }

    if ( event.queries() & Qt::ImCurrentSelection )
    {
#if 0
        const auto text = event.value( Qt::ImCurrentSelection ).toString();
        if ( !text.isEmpty() )
        {
        }
#endif
    }

    int passwordMaskDelay = -1;
    QString passwordCharacter;

    if ( echoMode == QskTextInput::NoEcho )
    {
        /*
             Qt::ImhHiddenText does not provide information
             to decide between NoEcho/Password, or provides
             more details about how to deal with hidden inputs.
             So we try to find out more from trying some properties.
         */

        QVariant value;

        value = item->property( "passwordMaskDelay" );
        if ( value.canConvert< int >() )
            passwordMaskDelay = value.toInt();

        value = item->property( "passwordCharacter" );
        if ( value.canConvert< QString >() )
            passwordCharacter = value.toString();

        value = item->property( "echoMode" );
        if ( value.canConvert< int >() )
        {
            const auto mode = value.toInt();
            if ( mode == QskTextInput::Password )
                echoMode = QskTextInput::Password;
        }
    }

    if ( passwordMaskDelay >= 0 )
        setPasswordMaskDelay( passwordMaskDelay );
    else
        resetPasswordMaskDelay();

    if ( !passwordCharacter.isEmpty() )
        setPasswordCharacter( passwordCharacter );
    else
        resetPasswordCharacter();

    setEchoMode( echoMode );
}

#include "QskTextInput.moc"
#include "moc_QskTextInput.cpp"
