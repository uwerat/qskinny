/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabBar.h"
#include "QskAspect.h"
#include "QskLinearBox.h"
#include "QskTabButton.h"
#include "QskTextOptions.h"

QSK_SUBCONTROL( QskTabBar, Panel )

namespace
{
    class ButtonBox : public QskLinearBox
    {
      public:
        ButtonBox( Qt::Orientation orientation, QQuickItem* parent )
            : QskLinearBox( orientation, parent )
        {
            setObjectName( QStringLiteral( "QskTabBarLayoutBox" ) );

            setSpacing( 0 );
            setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );
        }

        void restack( int currentIndex )
        {
            if ( itemCount() <= 1 )
                return;

            QQuickItem* buttonBefore = nullptr;

            for ( int i = 0; i < itemCount(); i++ )
            {
                if ( i != currentIndex )
                {
                    if ( auto button = itemAtIndex( i ) )
                    {
                        if ( buttonBefore == nullptr )
                        {
                            auto firstChild = childItems().first();
                            if ( firstChild != button )
                                button->stackBefore( firstChild );
                        }
                        else
                        {
                            button->stackAfter( buttonBefore );
                        }

                        buttonBefore = button;
                    }
                }
            }

            if ( auto button = itemAtIndex( currentIndex ) )
                button->stackAfter( buttonBefore );
        }
    };
}

class QskTabBar::PrivateData
{
  public:
    PrivateData()
        : currentIndex( -1 )
        , buttonBox( nullptr )
    {
    }

    void connectButton( QskTabButton* button, QskTabBar* tabBar, bool on )
    {
        if ( on )
        {
            connect( button, &QskTabButton::toggled,
                tabBar, &QskTabBar::adjustCurrentIndex, Qt::UniqueConnection );
        }
        else
        {
            disconnect( button, &QskTabButton::toggled,
                tabBar, &QskTabBar::adjustCurrentIndex );
        }
    }

    int currentIndex;
    QskTextOptions textOptions;
    ButtonBox* buttonBox;
};

QskTabBar::QskTabBar( QQuickItem* parent )
    : QskTabBar( Qt::Horizontal, parent )
{
}

QskTabBar::QskTabBar( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setAutoLayoutChildren( true );

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );

    m_data->buttonBox = new ButtonBox( orientation, this );

    connect( this, &QskTabBar::currentIndexChanged,
        m_data->buttonBox, &ButtonBox::restack, Qt::QueuedConnection );
}

QskTabBar::~QskTabBar()
{
}

void QskTabBar::setOrientation( Qt::Orientation orientation )
{
    if ( orientation == m_data->buttonBox->orientation() )
        return;

    setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );
    m_data->buttonBox->setOrientation( orientation );

    resetImplicitSize();

    for ( int i = 0; i < count(); i++ )
        buttonAt( i )->update();

    Q_EMIT orientationChanged();
}

Qt::Orientation QskTabBar::orientation() const
{
    return m_data->buttonBox->orientation();
}

void QskTabBar::setTextOptions( const QskTextOptions& options )
{
    if ( options != m_data->textOptions )
    {
        // we should consider text options being something for
        // QskControl - maybe added to the propagation system ???

        m_data->textOptions = options;
        Q_EMIT textOptionsChanged();

        for ( int i = 0; i < count(); i++ )
            buttonAt( i )->setTextOptions( options );
    }
}

QskTextOptions QskTabBar::textOptions() const
{
    return m_data->textOptions;
}

int QskTabBar::addTab( const QString& text )
{
    return insertTab( -1, text );
}

int QskTabBar::insertTab( int index, const QString& text )
{
    return insertTab( index, new QskTabButton( text ) );
}

int QskTabBar::addTab( QskTabButton* button )
{
    return insertTab( -1, button );
}

int QskTabBar::insertTab( int index, QskTabButton* button )
{
    auto buttonBox = m_data->buttonBox;

    if ( index < 0 || index >= buttonBox->itemCount() )
        index = buttonBox->itemCount();

    if ( isComponentComplete() )
    {
        if ( count() == 0 )
        {
            m_data->currentIndex = 0;
            button->setChecked( true );
        }
    }

    buttonBox->insertItem( index, button );
    buttonBox->restack( m_data->currentIndex );

    if ( button->textOptions() != m_data->textOptions )
        button->setTextOptions( m_data->textOptions );

    m_data->connectButton( button, this, true );

    Q_EMIT countChanged();

    return index;
}

void QskTabBar::removeTab( int index )
{
    auto item = m_data->buttonBox->itemAtIndex( index );
    if ( item == nullptr )
        return;

    delete item;

    if ( index > m_data->currentIndex )
    {
        Q_EMIT countChanged();
    }
    else if ( index < m_data->currentIndex )
    {
        m_data->currentIndex--;

        Q_EMIT countChanged();
        Q_EMIT currentIndexChanged( m_data->currentIndex );
    }
    else
    {
        QskTabButton* nextButton = nullptr;
        int nextIndex = -1;

        for ( int i = m_data->currentIndex; i >= 0; i-- )
        {
            auto button = buttonAt( i );
            if ( button && button->isEnabled() )
            {
                nextButton = button;
                nextIndex = i;

                break;
            }
        }

        if ( nextButton == nullptr )
        {
            for ( int i = m_data->currentIndex + 1; i < count(); i++ )
            {
                auto button = buttonAt( i );
                if ( button && button->isEnabled() )
                {
                    nextButton = button;
                    nextIndex = i;

                    break;
                }
            }
        }

        if ( nextButton )
        {
            m_data->connectButton( nextButton, this, false );
            nextButton->setChecked( true );
            m_data->connectButton( nextButton, this, true );
        }
        
        m_data->currentIndex = nextIndex;
        
        Q_EMIT countChanged();
        Q_EMIT currentIndexChanged( nextIndex );
    }
}

void QskTabBar::clear()
{
    if ( count() == 0 )
        return;

    const int idx = currentIndex();
    m_data->buttonBox->clear();

    Q_EMIT countChanged();

    if ( idx >= 0 )
        Q_EMIT currentIndexChanged( -1 );
}

bool QskTabBar::isTabEnabled( int index ) const
{
    const auto button = buttonAt( index );
    return button ? button->isEnabled() : false;
}

void QskTabBar::setTabEnabled( int index, bool enabled )
{
    if ( auto button = buttonAt( index ) )
    {
        // what happens, when it is the current button ???
        button->setEnabled( enabled );
    }
}

void QskTabBar::setCurrentIndex( int index )
{
    if ( index != m_data->currentIndex )
    {
        if ( isComponentComplete() )
        {
            auto button = buttonAt( index );
            if ( button && button->isEnabled() && !button->isChecked() )
                button->setChecked( true );
        }
        else
        {
            m_data->currentIndex = index;
            Q_EMIT currentIndexChanged( m_data->currentIndex );
        }
    }
}

int QskTabBar::currentIndex() const
{
    return m_data->currentIndex;
}

int QskTabBar::count() const
{
    return m_data->buttonBox->itemCount();
}

QskTabButton* QskTabBar::buttonAt( int position )
{
    return qobject_cast< QskTabButton* >(
        m_data->buttonBox->itemAtIndex( position ) );
}

const QskTabButton* QskTabBar::buttonAt( int position ) const
{
    auto that = const_cast< QskTabBar* >( this );
    return that->buttonAt( position );
}

QskTabButton* QskTabBar::currentButton()
{
    return buttonAt( currentIndex() );
}

const QskTabButton* QskTabBar::currentButton() const
{
    return buttonAt( currentIndex() );
}

QString QskTabBar::currentButtonText() const
{
    return buttonTextAt( currentIndex() );
}

QString QskTabBar::buttonTextAt( int index ) const
{
    if ( const auto button = buttonAt( index ) )
        return button->text();

    return QString();
}

int QskTabBar::indexOf( QskTabButton* button ) const
{
    return m_data->buttonBox->indexOf( button );
}

void QskTabBar::componentComplete()
{
    Inherited::componentComplete();

    if ( m_data->currentIndex < 0 && count() >= 0 )
        m_data->currentIndex = 0;

    if ( auto button = buttonAt( m_data->currentIndex ) )
    {
        if ( button->isEnabled() && !button->isChecked() )
            button->setChecked( true );
    }
}

void QskTabBar::adjustCurrentIndex()
{
    int index = -1;

    for ( int i = 0; i < count(); i++ )
    {
        if ( auto button = buttonAt( i ) )
        {
            if ( button->isChecked() )
            {
                index = i;
                break;
            }
        }
    }

    if ( index != m_data->currentIndex )
    {
        m_data->currentIndex = index;
        Q_EMIT currentIndexChanged( index );
    }
}

QskAspect::Subcontrol QskTabBar::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskTabBar::Panel;

    return Inherited::effectiveSubcontrol( subControl );
}

#include "moc_QskTabBar.cpp"
