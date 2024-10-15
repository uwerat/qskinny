/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskInputPanelBox.h"
#include "QskInputPredictionBar.h"
#include "QskLinearBox.h"
#include "QskTextInput.h"
#include "QskTextLabel.h"
#include "QskVirtualKeyboard.h"

#include <qpointer.h>
#include <qstring.h>

namespace
{
    class TextInputProxy final : public QskTextInput
    {
      public:
        TextInputProxy( QskInputPanelBox* panelBox, QQuickItem* parentItem = nullptr )
            : QskTextInput( parentItem )
            , m_panelBox( panelBox )
        {
            setObjectName( QStringLiteral( "InputBoxProxy" ) );

            setFocusPolicy( Qt::NoFocus );
            initSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::Fixed );
        }

      protected:
        QskAspect::Subcontrol substitutedSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if ( subControl == QskTextInput::Panel )
                return m_panelBox->effectiveSubcontrol( QskInputPanelBox::ProxyPanel );

            if ( subControl == QskTextInput::InputText )
                return m_panelBox->effectiveSubcontrol( QskInputPanelBox::ProxyText );

            return subControl;
        }

        void focusInEvent( QFocusEvent* ) override
        {
        }

        void focusOutEvent( QFocusEvent* ) override
        {
        }

      private:
        QskInputPanelBox* m_panelBox;
    };
}

QSK_SUBCONTROL( QskInputPanelBox, Panel )
QSK_SUBCONTROL( QskInputPanelBox, ProxyPanel )
QSK_SUBCONTROL( QskInputPanelBox, ProxyText )

class QskInputPanelBox::PrivateData
{
  public:
    QPointer< QQuickItem > inputItem;

    QskLinearBox* layout;
    QskTextLabel* prompt;
    TextInputProxy* inputProxy;
    QskInputPredictionBar* predictionBar;
    QskVirtualKeyboard* keyboard;

    QskInputPanelBox::PanelHints panelHints = QskInputPanelBox::InputProxy;
};

QskInputPanelBox::QskInputPanelBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );

    m_data->prompt = new QskTextLabel();
    m_data->prompt->setVisible( false );

    m_data->inputProxy = new TextInputProxy( this, nullptr );
    m_data->inputProxy->setVisible(
        m_data->panelHints & QskInputPanelBox::InputProxy );

    m_data->predictionBar = new QskInputPredictionBar();
    m_data->predictionBar->setVisible(
        m_data->panelHints & QskInputPanelBox::Prediction );

    m_data->keyboard = new QskVirtualKeyboard();

    auto layout = new QskLinearBox( Qt::Vertical, this );
    layout->setDefaultAlignment( Qt::AlignLeft | Qt::AlignHCenter );
    layout->setSpacing( 0 );

    layout->addItem( m_data->prompt );
    layout->addItem( m_data->inputProxy );
    layout->addStretch( 10 );
    layout->addItem( m_data->predictionBar );
    layout->addItem( m_data->keyboard );

    m_data->layout = layout;

    connect( m_data->predictionBar, &QskInputPredictionBar::predictiveTextSelected,
        this, &QskInputPanelBox::predictiveTextSelected );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanelBox::keySelected );
}

QskInputPanelBox::~QskInputPanelBox()
{
}

void QskInputPanelBox::setPanelHint( PanelHint hint, bool on )
{
    if ( on )
        setPanelHints( m_data->panelHints | hint );
    else
        setPanelHints( m_data->panelHints & ~hint );
}

void QskInputPanelBox::setPanelHints( PanelHints hints )
{
    if ( hints == m_data->panelHints )
        return;

    m_data->panelHints = hints;

    m_data->inputProxy->setVisible( hints & QskInputPanelBox::InputProxy );
    m_data->predictionBar->setVisible( hints & QskInputPanelBox::Prediction );

    const bool showPrompt = ( hints & QskInputPanelBox::InputProxy ) &&
        !m_data->prompt->text().isEmpty();

    m_data->prompt->setVisible( showPrompt );

    Q_EMIT panelHintsChanged();
}

QskInputPanelBox::PanelHints QskInputPanelBox::panelHints() const
{
    return m_data->panelHints;
}

void QskInputPanelBox::attachInputItem( QQuickItem* item )
{
    if ( item == m_data->inputItem )
        return;

    m_data->inputItem = item;

    if ( item )
    {
        if ( m_data->panelHints & QskInputPanelBox::InputProxy )
        {
            m_data->inputProxy->setupFrom( item );
            m_data->inputProxy->setEditing( true );
        }
    }
}

QQuickItem* QskInputPanelBox::attachedInputItem() const
{
    return m_data->inputItem;
}

QQuickItem* QskInputPanelBox::inputProxy() const
{
    if ( m_data->panelHints & QskInputPanelBox::InputProxy )
        return m_data->inputProxy;

    return nullptr;
}

QskAspect::Subcontrol QskInputPanelBox::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskInputPanelBox::Panel;

#if 1
    // TODO ...
    if ( subControl == QskInputPanelBox::ProxyPanel )
        return QskTextInput::Panel;

    if ( subControl == QskInputPanelBox::ProxyText )
        return QskTextInput::InputText;
#endif

    return subControl;
}

QString QskInputPanelBox::inputPrompt() const
{
    return m_data->prompt->text();
}

void QskInputPanelBox::setInputPrompt( const QString& text )
{
    auto prompt = m_data->prompt;

    if ( text != prompt->text() )
    {
        prompt->setText( text );

        if ( m_data->panelHints & QskInputPanelBox::InputProxy )
            prompt->setVisible( !text.isEmpty() );

        Q_EMIT inputPromptChanged( text );
    }
}

void QskInputPanelBox::setPrediction( const QStringList& prediction )
{
    m_data->predictionBar->setPrediction( prediction );
}

void QskInputPanelBox::setKeyboard( QskVirtualKeyboard* keyboard )
{
    if( m_data->keyboard )
    {
        m_data->keyboard->deleteLater();
    }

    m_data->keyboard = keyboard;
    m_data->layout->addItem( m_data->keyboard );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanelBox::keySelected );
}

void QskInputPanelBox::keyPressEvent( QKeyEvent* event )
{
    int keyCode = -1;

    switch ( event->key() )
    {
        case Qt::Key_Return:
        case Qt::Key_Escape:
        {
            keyCode = event->key();
            break;
        }

        default:
        {
            const auto text = event->text();

            if ( !text.isEmpty() )
                keyCode = text[ 0 ].unicode();
            else
                keyCode = event->key();
        }
    }

    if ( m_data->keyboard->hasKey( keyCode ) )
    {
        // animating the corresponding key button ???
        Q_EMIT keySelected( keyCode );
    }
}

#include "moc_QskInputPanelBox.cpp"
