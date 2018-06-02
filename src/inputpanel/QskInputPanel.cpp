/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputPanel.h"
#include "QskVirtualKeyboard.h"
#include "QskInputPredictionBar.h"
#include "QskTextInput.h"
#include "QskTextLabel.h"
#include "QskLinearBox.h"

#include <QString>
#include <QLocale>
#include <QPointer>
#include <QInputMethodQueryEvent>

namespace
{
    class TextInputProxy final : public QskTextInput
    {
    public:
        TextInputProxy( QQuickItem* parentItem = nullptr ):
            QskTextInput( parentItem )
        {
            setObjectName( "InputPanelInputProxy" );
            setFocusPolicy( Qt::NoFocus );
        }

    protected:
        virtual void focusInEvent( QFocusEvent* ) override final
        {
        }

        virtual void focusOutEvent( QFocusEvent* ) override final
        {
        }
    };
}

QSK_SUBCONTROL( QskInputPanel, Panel )

class QskInputPanel::PrivateData
{
public:
    QPointer< QQuickItem > inputItem;

    QskLinearBox* layout;
    QskTextLabel* prompt;
    TextInputProxy* inputProxy;
    QskInputPredictionBar* predictionBar;
    QskVirtualKeyboard* keyboard;

    int maxChars = -1;

    QskInputPanel::PanelHints panelHints = QskInputPanel::InputProxy;
};

QskInputPanel::QskInputPanel( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );
    initSizePolicy( QskSizePolicy::Expanding, QskSizePolicy::Constrained );

    m_data->prompt = new QskTextLabel();
    m_data->prompt->setVisible( false );

    m_data->inputProxy = new TextInputProxy();
    m_data->inputProxy->setVisible(
        m_data->panelHints & QskInputPanel::InputProxy );

    m_data->predictionBar = new QskInputPredictionBar();
    m_data->predictionBar->setVisible(
        m_data->panelHints & QskInputPanel::Prediction );

    m_data->keyboard = new QskVirtualKeyboard();

    auto layout = new QskLinearBox( Qt::Vertical, this );

    layout->addItem( m_data->prompt, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addItem( m_data->inputProxy, Qt::AlignLeft | Qt::AlignHCenter );
    layout->addStretch( 10 );
    layout->addItem( m_data->predictionBar );
    layout->addItem( m_data->keyboard );

    m_data->layout = layout;

    connect( m_data->predictionBar, &QskInputPredictionBar::predictiveTextSelected,
        this, &QskInputPanel::predictiveTextSelected );

    connect( m_data->keyboard, &QskVirtualKeyboard::keySelected,
        this, &QskInputPanel::keySelected );
}

QskInputPanel::~QskInputPanel()
{
}

void QskInputPanel::setPanelHint( PanelHint hint, bool on )
{
    if ( on )
        setPanelHints( m_data->panelHints | hint );
    else
        setPanelHints( m_data->panelHints & ~hint );
}

void QskInputPanel::setPanelHints( PanelHints hints )
{
    if ( hints == m_data->panelHints )
        return;

    m_data->panelHints = hints;

    m_data->inputProxy->setVisible( hints & QskInputPanel::InputProxy );
    m_data->predictionBar->setVisible( hints & QskInputPanel::Prediction );

    const bool showPrompt = ( hints & QskInputPanel::InputProxy )
        && !m_data->prompt->text().isEmpty();

    m_data->prompt->setVisible( showPrompt );

    Q_EMIT panelHintsChanged();
}

QskInputPanel::PanelHints QskInputPanel::panelHints() const
{
    return m_data->panelHints;
}

void QskInputPanel::attachInputItem( QQuickItem* item )
{
    if ( item == m_data->inputItem )
        return;

    m_data->inputItem = item;

    if ( item )
    {
        if ( m_data->panelHints & QskInputPanel::InputProxy )
        {
            m_data->inputProxy->setupFrom( item );
            m_data->inputProxy->setEditing( true );

            // hiding the cursor in item
            const QInputMethodEvent::Attribute attribute(
                QInputMethodEvent::Cursor, 0, 0, QVariant() );

            QInputMethodEvent event( QString(), { attribute } );
            QCoreApplication::sendEvent( item, &event );
        }
    }
}

QQuickItem* QskInputPanel::attachedInputItem() const
{
    return m_data->inputItem;
}

QQuickItem* QskInputPanel::inputProxy() const
{
    return m_data->inputProxy;
}

QskAspect::Subcontrol QskInputPanel::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == QskBox::Panel )
        return QskInputPanel::Panel;

    return subControl;
}

QString QskInputPanel::inputPrompt() const
{
    return m_data->prompt->text();
}

void QskInputPanel::setInputPrompt( const QString& text )
{
    auto prompt = m_data->prompt;

    if ( text != prompt->text() )
    {
        prompt->setText( text );

        if ( m_data->panelHints & QskInputPanel::InputProxy )
            prompt->setVisible( !text.isEmpty() );

        Q_EMIT inputPromptChanged( text );
    }
}

void QskInputPanel::setPrediction( const QStringList& prediction )
{
    m_data->predictionBar->setPrediction( prediction );
}

void QskInputPanel::keyPressEvent( QKeyEvent* event )
{
    // animate the corresponding key button TODO

    switch( event->key() )
    {
        case Qt::Key_Return:
        case Qt::Key_Escape:
        {
            Q_EMIT keySelected( event->key() );
            break;
        }

        case Qt::Key_Shift:
        case Qt::Key_Control:
        case Qt::Key_Meta:
        case Qt::Key_Alt:
        case Qt::Key_AltGr:
        case Qt::Key_CapsLock:
        case Qt::Key_NumLock:
        case Qt::Key_ScrollLock:
        {
            break;
        }

        default:
        {
            const auto text = event->text();

            if ( !text.isEmpty() )
                Q_EMIT keySelected( text[0].unicode() );
            else
                Q_EMIT keySelected( event->key() );
        }
    }
}

#include "moc_QskInputPanel.cpp"
