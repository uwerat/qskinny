/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputSubWindow.h"
#include "QskTextOptions.h"
#include "QskGraphic.h"
#include "QskPushButton.h"
#include "QskTextLabel.h"
#include "QskGraphicLabel.h"
#include "QskDialogButtonBox.h"
#include "QskLinearBox.h"
#include "QskSkin.h"

#include <QPointer>

namespace
{
    class TextLabel final : public QskTextLabel
    {
    public:
        TextLabel( QskInputSubWindow* box )
        {
            setObjectName( QStringLiteral( "QskInputSubWindowTextLabel" ) );

            QskTextOptions options;
            options.setWrapMode( QskTextOptions::WordWrap );

            setTextOptions( options );

            connect( this, SIGNAL( textChanged(QString) ),
                box, SIGNAL( infoTextChanged() ) );

            connect( this, SIGNAL( textOptionsChanged() ),
                box, SIGNAL( infoTextOptionsChanged() ) );
        }
    };

    class SymbolLabel final : public QskGraphicLabel
    {
    public:
        SymbolLabel( QskInputSubWindow* )
        {
            setObjectName( QStringLiteral( "QskInputSubWindowSymbolLabel" ) );
            updateSourceSize();
        }

    protected:
        virtual void changeEvent( QEvent* event ) override final
        {
            if ( event->type() == QEvent::FontChange )
                updateSourceSize();

            QskGraphicLabel::changeEvent( event );
        }

    private:
        void updateSourceSize()
        {
            // when there is no explicit size known,
            // we always adjust the icon according to the font

            if ( sourceSize().isEmpty() )
            {
                const QFont font = effectiveFont( QskTextLabel::Text );
                setPreferredSize( -1.0, 1.5 * QFontMetricsF( font ).height() );
            }
        }
    };
}

class QskInputSubWindow::PrivateData
{
public:
    PrivateData():
        standardButtons( QskDialog::NoButton ),
        defaultButton( QskDialog::NoButton ),
        inputControl( nullptr )
    {
    }

    QskDialog::StandardButtons standardButtons;
    QskDialog::StandardButton defaultButton;

    QskTextLabel* infoTextLabel;
    QskGraphicLabel* symbolLabel;
    QPointer< QskControl > inputControl;
    QskLinearBox* titleBox;
    QskDialogButtonBox* buttonBox;
    QskLinearBox* layoutBox;
};

QskInputSubWindow::QskInputSubWindow( QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->infoTextLabel = new TextLabel( this );
    m_data->infoTextLabel->setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Preferred );
    m_data->infoTextLabel->setAlignment( Qt::AlignHCenter | Qt::AlignTop );

    m_data->symbolLabel = new SymbolLabel( this );
    m_data->symbolLabel->setAlignment( Qt::AlignTop | Qt::AlignHCenter );
    m_data->symbolLabel->setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
    m_data->symbolLabel->hide();

    m_data->titleBox = new QskLinearBox( Qt::Horizontal );
    m_data->titleBox->setSpacing( 0 );
    m_data->titleBox->addItem( m_data->symbolLabel, Qt::AlignTop | Qt::AlignHCenter );
    m_data->titleBox->addItem( m_data->infoTextLabel, Qt::AlignTop | Qt::AlignHCenter );
    m_data->titleBox->setStretchFactor( m_data->infoTextLabel, 10 );

    m_data->buttonBox = new QskDialogButtonBox();
    m_data->buttonBox->hide();

    m_data->layoutBox = new QskLinearBox( Qt::Vertical, this );
    m_data->layoutBox->addItem( m_data->titleBox );
    m_data->layoutBox->addItem( m_data->buttonBox );

    setAutoLayoutChildren( true );

    connect( m_data->buttonBox, &QskDialogButtonBox::accepted,
        this, &QskInputSubWindow::accept );

    connect( m_data->buttonBox, &QskDialogButtonBox::rejected,
        this, &QskInputSubWindow::reject );

    updateTitleBox();
}

QskInputSubWindow::~QskInputSubWindow()
{
}

void QskInputSubWindow::setStandardButtons( QskDialog::StandardButtons buttons )
{
    if ( m_data->standardButtons == buttons )
        return;

    m_data->standardButtons = buttons;

    m_data->buttonBox->setStandardButtons( buttons );
    m_data->buttonBox->setVisible( buttons != QskDialog::NoButton );

#if 1
    const QList< QskPushButton* > b = m_data->buttonBox->buttons();
    if ( !b.isEmpty() )
        b[0]->setFocus( true );
#endif
}

QskDialog::StandardButtons QskInputSubWindow::standardButtons() const
{
    return m_data->buttonBox->standardButtons();
}

QskDialog::StandardButton QskInputSubWindow::defaultButton() const
{
    return m_data->defaultButton;
}

void QskInputSubWindow::setDefaultButton( QskDialog::StandardButton button )
{
    m_data->defaultButton = button;
}

void QskInputSubWindow::setInfoText( const QString& text )
{
    m_data->infoTextLabel->setText( text );
    updateTitleBox();
}

QString QskInputSubWindow::infoText() const
{
    return m_data->infoTextLabel->text();
}

void QskInputSubWindow::setInfoTextOptions( const QskTextOptions& options )
{
    m_data->infoTextLabel->setTextOptions( options );
}

QskTextOptions QskInputSubWindow::infoTextOptions() const
{
    return m_data->infoTextLabel->textOptions();
}

void QskInputSubWindow::setSymbolSource( const QUrl& url )
{
    m_data->symbolLabel->setSource( url );
    m_data->symbolLabel->setVisible( !url.isEmpty() );

    auto box = qobject_cast< QskLinearBox* >(
        m_data->symbolLabel->parentItem() );

    box->setSpacing( m_data->symbolLabel->isVisible() ? 15 : 0 ); // metrics !!
    updateTitleBox();
}

void QskInputSubWindow::setSymbolType( int symbolType )
{
    setSymbol( effectiveSkin()->symbol( symbolType ) );
}

QUrl QskInputSubWindow::symbolSource() const
{
    return m_data->symbolLabel->source();
}

void QskInputSubWindow::setSymbol( const QskGraphic& symbol )
{
    m_data->symbolLabel->setVisible( !symbol.isNull() );
    m_data->symbolLabel->setGraphic( symbol );
    updateTitleBox();
}

QskGraphic QskInputSubWindow::symbol() const
{
    return m_data->symbolLabel->graphic();
}

void QskInputSubWindow::setInputControl( QskControl* control )
{
    if ( control == m_data->inputControl )
        return;

    delete m_data->inputControl;
    m_data->inputControl = control;

    if ( m_data->inputControl )
        m_data->layoutBox->insertItem( 1, control );
}

QskControl* QskInputSubWindow::inputControl() const
{
    return m_data->inputControl;
}

QskDialogButtonBox* QskInputSubWindow::buttonBox()
{
    return m_data->buttonBox;
}

const QskDialogButtonBox* QskInputSubWindow::buttonBox() const
{
    return m_data->buttonBox;
}

QskDialog::StandardButton QskInputSubWindow::clickedButton() const
{
    return m_data->buttonBox->clickedButton();
}

void QskInputSubWindow::keyPressEvent( QKeyEvent* event )
{
    if ( QskDialogButtonBox::isDefaultButtonKeyEvent( event ) )
    {
        QskPushButton* button = m_data->buttonBox->button( defaultButton() );
        if ( button && button->isEnabled() )
            button->click();
    }

    Inherited::keyPressEvent( event );
}

void QskInputSubWindow::updateTitleBox()
{
    m_data->titleBox->setVisible( !( m_data->symbolLabel->isEmpty() &&
        m_data->infoTextLabel->text().isEmpty() ) );
}

#include "moc_QskInputSubWindow.cpp"
