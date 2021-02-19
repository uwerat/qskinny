/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskTabBar.h"
#include "QskAspect.h"
#include "QskScrollBox.h"
#include "QskLinearBox.h"
#include "QskTabButton.h"
#include "QskTextOptions.h"
#include "QskAnimationHint.h"
#include "QskQuick.h"

#include <qquickwindow.h>

QSK_SUBCONTROL( QskTabBar, Panel )

static inline Qt::Orientation qskOrientation( int position )
{
    if ( ( position == Qsk::Top ) || ( position == Qsk::Bottom ) )
        return Qt::Horizontal;
    else
        return Qt::Vertical;
}

static inline void qskTransposeSizePolicy( QskControl* control )
{
    control->setSizePolicy( control->sizePolicy().transposed() );
}

namespace
{
    class ButtonBox final : public QskLinearBox
    {
        using Inherited = QskLinearBox;

      public:
        ButtonBox( Qt::Orientation orientation, QQuickItem* parent )
            : QskLinearBox( orientation, parent )
        {
            setObjectName( QStringLiteral( "QskTabBarLayoutBox" ) );
            setExtraSpacingAt( Qt::RightEdge | Qt::BottomEdge );
        }

        void restack( int currentIndex )
        {
            /*
                We can't simply reorder the buttons as the focus tab chain depends on it.
                Again we have some QML only API ( QQuickKeyNavigationAttached ),
                that is useless for C++, but nothing like QWidget::setTabOrder.
                Maybe it makes sense to implement our own navigation in
                QskControl::keyPressEvent.
             */

            for ( int i = 0; i < elementCount(); i++ )
            {
                if ( auto button = itemAtIndex( i ) )
                    button->setZ( i == currentIndex ? 0.001 : 0.0 );
            }
        }
    };

    class ScrollBox final : public QskScrollBox
    {
        using Inherited = QskScrollBox;

      public:
        ScrollBox( QQuickItem* parent )
            : QskScrollBox( parent )
        {
            setPolishOnResize( true );
            setWheelEnabled( false );
            enableAutoTranslation( true );

            setFocusPolicy( Qt::NoFocus );

            connect( this, &ScrollBox::scrollPosChanged,
                this, &QskControl::focusIndicatorRectChanged );
        }

        QRectF focusIndicatorClipRect() const override
        {
            auto r = clipRect();

            if ( window() )
            {
                if ( auto focusItem = window()->activeFocusItem() )
                {
                    const auto itemRect = mapRectFromItem(
                        focusItem, focusItem->boundingRect() );

                    if ( r.intersects( itemRect ) )
                        return QRectF();
                }
            }

            return r;
        }

        QskAnimationHint flickHint() const override
        {
            if ( auto tabBar = qobject_cast< const QskTabBar* >( parentItem() ) )
            {
                return tabBar->effectiveAnimation( QskAspect::Metric,
                    QskTabBar::Panel, QskAspect::NoState );
            }

            // should be a skin hint TODO ...
            return QskAnimationHint( 200, QEasingCurve::OutCubic );
        }

        QRectF viewContentsRect() const override
        {
            return layoutRect();
        }

        void setOrientation( Qt::Orientation orientation )
        {
            setFlickableOrientations( orientation );

            if ( orientation == Qt::Horizontal )
                setSizePolicy( QskSizePolicy::Ignored, QskSizePolicy::MinimumExpanding );
            else
                setSizePolicy( QskSizePolicy::MinimumExpanding, QskSizePolicy::Ignored );
        }

        void ensureItemVisible( const QQuickItem* item )
        {
            if ( qskIsAncestorOf( this, item ) )
            {
                const auto pos = mapFromItem( item, QPointF() );
                ensureVisible( QRectF( pos.x(), pos.y(), item->width(), item->height() ) );
            }
        }

        QRectF clipRect() const override
        {
            auto r = Inherited::clipRect();

            if ( auto control = qskControlCast( parentItem() ) )
                r += control->paddingHint( QskTabBar::Panel );

            /*
                Often the current tab button grows beyond the bounding rectangle
                of the tab bar, so that it looks like being on top of the tab page
                border. So we only want to clip in scroll direction.
                Note: std::numeric_limits< int >::max() does not work - guess
                some overflow somewhere ...
             */
            constexpr qreal expanded = 0.90 * std::numeric_limits< int >::max();

            if ( flickableOrientations() & Qt::Horizontal )
            {
                r.setTop( r.top() - 0.5 * expanded );
                r.setHeight( expanded );
            }
            else
            {
                r.setLeft( r.left() - 0.5 * expanded );
                r.setWidth( expanded );
            }

            return r;
        }

      protected:

        bool event( QEvent* event ) override
        {
            if ( event->type() == QEvent::LayoutRequest )
            {
                resetImplicitSize();
                polish();
            }

            return Inherited::event( event );
        }

        void updateLayout() override
        {
            auto box = buttonBox();

            auto boxSize = viewContentsRect().size();
            boxSize = qskConstrainedItemSize( box, boxSize );

            if ( auto box = buttonBox() )
                box->setSize( boxSize );

            enableAutoTranslation( false );

            setScrollableSize( boxSize );
            setScrollPos( scrollPos() );

            enableAutoTranslation( true );

            translateButtonBox();

            setClip( width() < boxSize.width() || height() < boxSize.height() );
        }

        QSizeF layoutSizeHint( Qt::SizeHint which, const QSizeF& constraint ) const override
        {
            auto hint = buttonBox()->sizeConstraint( which, constraint );

            if ( which == Qt::MinimumSize )
            {
                if ( sizePolicy().horizontalPolicy() & QskSizePolicy::ShrinkFlag )
                    hint.setWidth( -1 );

                if ( sizePolicy().verticalPolicy() & QskSizePolicy::ShrinkFlag )
                    hint.setHeight( -1 );
            }

            return hint;
        }

      private:

        inline QskLinearBox* buttonBox() const
        {
            return qobject_cast< QskLinearBox* >( childItems().first() );
        }

        void enableAutoTranslation( bool on )
        {
            if ( on )
            {
                connect( this, &QskScrollBox::scrollPosChanged,
                    this, &ScrollBox::translateButtonBox );
            }
            else
            {
                disconnect( this, &QskScrollBox::scrollPosChanged,
                    this, &ScrollBox::translateButtonBox );
            }
        }

        void translateButtonBox()
        {
            if ( auto box = buttonBox() )
            {
                const QPointF pos = viewContentsRect().topLeft() - scrollPos();
                box->setPosition( pos );
            }
        }
    };
}

class QskTabBar::PrivateData
{
  public:
    PrivateData( Qsk::Position position )
        : position( position )
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

    ScrollBox* scrollBox = nullptr;
    ButtonBox* buttonBox = nullptr;
    int currentIndex = -1;

    QskTextOptions textOptions;
    uint position : 2;
};

QskTabBar::QskTabBar( QQuickItem* parent )
    : QskTabBar( Qsk::Top, parent )
{
}

QskTabBar::QskTabBar( Qsk::Position position, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( position ) )
{
    setAutoLayoutChildren( true );

    const auto orientation = qskOrientation( position );

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );

    m_data->scrollBox = new ScrollBox( this );
    m_data->scrollBox->setOrientation( orientation );

    m_data->buttonBox = new ButtonBox( orientation, m_data->scrollBox );
    m_data->buttonBox->setSpacing( spacingHint( QskTabBar::Panel ) );
    m_data->buttonBox->setSizePolicy( QskSizePolicy::Maximum, QskSizePolicy::Maximum );

    connect( this, &QskTabBar::currentIndexChanged,
        m_data->buttonBox, &ButtonBox::restack, Qt::QueuedConnection );
}

QskTabBar::~QskTabBar()
{
}

void QskTabBar::setTabPosition( Qsk::Position position )
{
    if ( position == m_data->position )
        return;

    m_data->position = position;

    const auto orientation = qskOrientation( position );

    if ( orientation != m_data->buttonBox->orientation() )
    {
        qskTransposeSizePolicy( this );

        m_data->buttonBox->setOrientation( orientation );
        qskTransposeSizePolicy( m_data->buttonBox );

        m_data->scrollBox->setOrientation( orientation );
    }

    resetImplicitSize();

    for ( int i = 0; i < count(); i++ )
        buttonAt( i )->update();

    Q_EMIT tabPositionChanged( position );
}

Qsk::Position QskTabBar::tabPosition() const
{
    return static_cast< Qsk::Position >( m_data->position );
}

Qt::Orientation QskTabBar::orientation() const
{
    return qskOrientation( m_data->position );
}

void QskTabBar::setAutoScrollFocusedButton( bool on )
{
    if ( m_data->scrollBox->autoScrollFocusItem() != on )
    {
        m_data->scrollBox->setAutoScrollFocusedItem( on );
        Q_EMIT autoScrollFocusedButtonChanged( on );
    }
}

bool QskTabBar::autoScrollFocusButton() const
{
    return m_data->scrollBox->autoScrollFocusItem();
}

void QskTabBar::setAutoFitTabs( bool on )
{
    const auto orientation = qskOrientation( m_data->position );
    int policy = m_data->buttonBox->sizePolicy( orientation );

    if ( ( policy & QskSizePolicy::GrowFlag ) != on )
    {
        if ( on )
            policy |= QskSizePolicy::GrowFlag;
        else
            policy &= ~QskSizePolicy::GrowFlag;

        // we need operators for QskSizePolicy::Policy: TODO ...
        m_data->buttonBox->setSizePolicy(
            orientation, static_cast< QskSizePolicy::Policy >( policy ) );

        polish();

        Q_EMIT autoFitTabsChanged( on );
    }
}

bool QskTabBar::autoFitTabs() const
{
    const auto policy = m_data->buttonBox->sizePolicy( orientation() );
    return ( policy & QskSizePolicy::GrowFlag );
}

void QskTabBar::setTextOptions( const QskTextOptions& options )
{
    if ( options != m_data->textOptions )
    {
        // we should consider text options being something for
        // QskControl - maybe added to the propagation system ???

        m_data->textOptions = options;
        Q_EMIT textOptionsChanged( options );

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

    if ( index < 0 || index >= buttonBox->elementCount() )
        index = buttonBox->elementCount();

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

    connect( button, &QskAbstractButton::clicked,
       this, &QskTabBar::handleButtonClick );

    Q_EMIT countChanged( count() );

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
        Q_EMIT countChanged( count() );
    }
    else if ( index < m_data->currentIndex )
    {
        m_data->currentIndex--;

        Q_EMIT countChanged( count() );
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

        Q_EMIT countChanged( count() );
        Q_EMIT currentIndexChanged( nextIndex );
    }
}

void QskTabBar::clear( bool autoDelete )
{
    if ( count() == 0 )
        return;

    const int idx = currentIndex();
    m_data->buttonBox->clear( autoDelete );

    Q_EMIT countChanged( count() );

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
    return m_data->buttonBox->elementCount();
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

void QskTabBar::ensureButtonVisible( const QskTabButton* button )
{
    m_data->scrollBox->ensureItemVisible( button );
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

void QskTabBar::handleButtonClick()
{
    if ( auto button = qobject_cast< const QskTabButton* >( sender() ) )
    {
        const auto index = indexOf( button );

        if ( index >= 0 )
            Q_EMIT buttonClicked( index );
    }
}

QskAspect::Subcontrol QskTabBar::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskTabBar::Panel;

    return Inherited::effectiveSubcontrol( subControl );
}

QskAspect::Placement QskTabBar::effectivePlacement() const
{
    switch ( m_data->position )
    {
        case Qsk::Left:
            return QskAspect::Left;

        case Qsk::Right:
            return QskAspect::Right;

        case Qsk::Top:
            return QskAspect::Top;

        case Qsk::Bottom:
            return QskAspect::Bottom;
    }

    return QskAspect::NoPlacement;
}

#include "moc_QskTabBar.cpp"
