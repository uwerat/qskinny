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
        TextInputProxy( QskInputPanel* panel, QQuickItem* parentItem = nullptr ):
            QskTextInput( parentItem ),
            m_panel( panel )
        {
            setObjectName( "InputPanelInputProxy" );
            setFocusPolicy( Qt::NoFocus );
        }

        virtual QskAspect::Subcontrol effectiveSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if ( subControl == QskTextInput::Panel )
                return m_panel->effectiveSubcontrol( QskInputPanel::ProxyPanel );

            if ( subControl == QskTextInput::Text )
                return m_panel->effectiveSubcontrol( QskInputPanel::ProxyText );

            return subControl;
        }

    protected:
        virtual void focusInEvent( QFocusEvent* ) override final
        {
        }

        virtual void focusOutEvent( QFocusEvent* ) override final
        {
        }

    private:
        QskInputPanel* m_panel;
    };
}

QSK_SUBCONTROL( QskInputPanel, Panel )
QSK_SUBCONTROL( QskInputPanel, ProxyPanel )
QSK_SUBCONTROL( QskInputPanel, ProxyText )

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

    m_data->inputProxy = new TextInputProxy( this, nullptr );
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

#if 1
    // TODO ...
    if( subControl == QskInputPanel::ProxyPanel )
        return QskTextInput::Panel;

    if( subControl == QskInputPanel::ProxyText )
        return QskTextInput::Text;
#endif

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
    int keyCode = -1;

    switch( event->key() )
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
                keyCode = text[0].unicode();
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

#include "moc_QskInputPanel.cpp"
