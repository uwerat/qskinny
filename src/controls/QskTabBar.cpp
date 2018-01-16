/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabBar.h"
#include "QskLinearBox.h"
#include "QskTabButton.h"
#include "QskTextOptions.h"
#include "QskAspect.h"

QSK_SUBCONTROL( QskTabBar, Panel )

class QskTabBar::PrivateData
{
public:
    PrivateData():
        currentIndex( -1 ),
        layoutBox( nullptr )
    {
    }

    int currentIndex;
    QskTextOptions textOptions;
    QskLinearBox* layoutBox;
};

QskTabBar::QskTabBar( QQuickItem* parent ):
    QskTabBar( Qt::Horizontal, parent )
{
}

QskTabBar::QskTabBar( Qt::Orientation orientation, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
{
    m_data->layoutBox = new QskLinearBox( orientation, this );
    m_data->layoutBox->setObjectName( QStringLiteral( "QskTabBarLayoutBox" ) );
    m_data->layoutBox->setSpacing( 0 );
    m_data->layoutBox->setMargins( 0 );
    m_data->layoutBox->setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );

    setAutoLayoutChildren( true );

    QObject::connect( this, &QskTabBar::currentIndexChanged,
        this, &QskTabBar::restack, Qt::QueuedConnection );
}

QskTabBar::~QskTabBar()
{
}

void QskTabBar::setOrientation( Qt::Orientation orientation )
{
    if ( orientation == m_data->layoutBox->orientation() )
        return;

    setSizePolicy( sizePolicy( Qt::Vertical ), sizePolicy( Qt::Horizontal ) );
    m_data->layoutBox->setOrientation( orientation );

    resetImplicitSize();

    for ( int i = 0; i < count(); i++ )
        buttonAt( i )->update();

    Q_EMIT orientationChanged();
}

Qt::Orientation QskTabBar::orientation() const
{
    return m_data->layoutBox->orientation();
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
    if ( index < 0 || index >= m_data->layoutBox->itemCount() )
        index = m_data->layoutBox->itemCount();

    if ( isComponentComplete() )
    {
        if ( count() == 0 )
        {
            m_data->currentIndex = 0;
            button->setChecked( true );
        }
    }

    m_data->layoutBox->insertItem( index, button );

    restack();

    if ( button->textOptions() != m_data->textOptions )
        button->setTextOptions( m_data->textOptions );

    auto onTabSelected =
        [ this, button ] ( bool on )
        {
            if ( on )
            {
                const int pos = indexOf( button );
                if ( pos >= 0 && pos != m_data->currentIndex )
                {
                    m_data->currentIndex = pos;
                    Q_EMIT currentIndexChanged( pos );
                }
            }
        };

    connect( button, &QskTabButton::toggled, this, onTabSelected );

    Q_EMIT countChanged();

    return index;
}

void QskTabBar::removeTab( int index )
{
    QQuickItem* item = m_data->layoutBox->itemAtIndex( index );
    if ( item )
    {
        delete m_data->layoutBox->itemAtIndex( index );
        Q_EMIT countChanged();

        if ( index == m_data->currentIndex )
        {
            QskTabButton* nextButton = nullptr;

            for ( int i = m_data->currentIndex; i >= 0; i-- )
            {
                QskTabButton* btn = buttonAt( index );
                if ( btn && btn->isEnabled() )
                {
                    nextButton = btn;
                    break;
                }
            }

            if ( nextButton == nullptr )
            {
                for ( int i = m_data->currentIndex; i < count(); i++ )
                {
                    QskTabButton* btn = buttonAt( index );
                    if ( btn && btn->isEnabled() )
                    {
                        nextButton = btn;
                        break;
                    }
                }
            }

            if ( nextButton && !nextButton->isChecked() )
                nextButton->setChecked( true );
        }
    }

    restack();
}

void QskTabBar::clear()
{
    if ( count() == 0 )
        return;

    const int idx = currentIndex();
    m_data->layoutBox->clear();

    Q_EMIT countChanged();

    if ( idx >= 0 )
        Q_EMIT currentIndexChanged( -1 );
}

bool QskTabBar::isTabEnabled( int index ) const
{
    const QskTabButton* btn = buttonAt( index );
    return btn ? btn->isEnabled() : false;
}

void QskTabBar::setTabEnabled( int index, bool enabled )
{
    QskTabButton* btn = buttonAt( index );
    if ( btn )
    {
        // what happens, when it is the current button ???
        btn->setEnabled( enabled );
    }
}

void QskTabBar::setCurrentIndex( int index )
{
    if ( index != m_data->currentIndex )
    {
        if ( isComponentComplete() )
        {
            QskTabButton* btn = buttonAt( index );
            if ( btn && btn->isEnabled() && !btn->isChecked() )
                btn->setChecked( true );
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
    return m_data->layoutBox->itemCount();
}

QskTabButton* QskTabBar::buttonAt( int position )
{
    return qobject_cast< QskTabButton* >( m_data->layoutBox->itemAtIndex( position ) );
}

const QskTabButton* QskTabBar::buttonAt( int position ) const
{
    QskTabBar* that = const_cast< QskTabBar* >( this );
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
    if ( const QskTabButton* btn = buttonAt( index ) )
        return btn->text();

    return QString();
}

int QskTabBar::indexOf( QskTabButton* button ) const
{
    return m_data->layoutBox->indexOf( button );
}

void QskTabBar::componentComplete()
{
    Inherited::componentComplete();

    if ( m_data->currentIndex < 0 && count() >= 0 )
        m_data->currentIndex = 0;

    QskTabButton* btn = buttonAt( m_data->currentIndex );
    if ( btn && btn->isEnabled() && !btn->isChecked() )
        btn->setChecked( true );
}

void QskTabBar::restack()
{
    const int c = currentIndex();
    for ( int i = count() - 1; i >= 0; --i )
    {
        auto button = buttonAt( i );
        button->setZ( c == i ? count() : ( count() - i ) );
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
