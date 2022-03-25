/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButtonBox.h"
#include "QskDialogButton.h"
#include "QskLinearBox.h"
#include "QskSkin.h"

#include "QskLinearLayoutEngine.h"

#include <qevent.h>
#include <qpointer.h>
#include <qvector.h>

#include <qpa/qplatformdialoghelper.h>

#include <limits>

QSK_SUBCONTROL( QskDialogButtonBox, Panel )

static void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

namespace
{
    class LayoutEngine : public QskLinearLayoutEngine
    {
      public:
        LayoutEngine( Qt::Orientation orientation )
            : QskLinearLayoutEngine( orientation, std::numeric_limits< uint >::max() )
        {
        }

        void addStretch()
        {
            const auto index = insertSpacerAt( count(), 0 );
            setStretchFactorAt( index, 1 );
        }

        void addButtons( const QVector< QskPushButton* >& buttons, bool reverse )
        {
            if ( reverse )
            {
                for ( int i = buttons.count() - 1; i >= 0; i-- )
                    addItem( buttons[ i ] );
            }
            else
            {
                for ( int i = 0; i < buttons.count(); i++ )
                    addItem( buttons[ i ] );
            }
        }
    };
}

class QskDialogButtonBox::PrivateData
{
  public:
    PrivateData( Qt::Orientation orientation )
        : layoutEngine( orientation )
    {
    }

    static inline void connectButton( QskDialogButtonBox* box,
        QskPushButton* button, bool on )
    {
        if ( on )
        {
            connect( button, &QskPushButton::clicked,
                box, &QskDialogButtonBox::onButtonClicked,
                Qt::UniqueConnection );

            connect( button, &QskPushButton::visibleChanged,
                box, &QskDialogButtonBox::invalidateLayout,
                Qt::UniqueConnection );
        }
        else
        {
            disconnect( button, &QskPushButton::clicked,
                box, &QskDialogButtonBox::onButtonClicked );

            disconnect( button, &QskPushButton::visibleChanged,
                box, &QskDialogButtonBox::invalidateLayout );
        }
    }
    LayoutEngine layoutEngine;

    QVector< QskPushButton* > buttons[ QskDialog::NActionRoles ];
    QPointer< QskPushButton > defaultButton;

    QskDialog::Action clickedAction = QskDialog::NoAction;
    bool centeredButtons = false;
};

QskDialogButtonBox::QskDialogButtonBox( QQuickItem* parent )
    : QskDialogButtonBox( Qt::Horizontal, parent )
{
}

QskDialogButtonBox::QskDialogButtonBox( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( orientation ) )
{
    setPolishOnResize( true );

    if ( orientation == Qt::Horizontal )
        initSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );
}

QskDialogButtonBox::~QskDialogButtonBox()
{
    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        for ( auto button : qAsConst( m_data->buttons[ i ] ) )
        {
            /*
                The destructor of QQuickItem sets the parentItem of
                all children to nullptr, what leads to visibleChanged
                signals. So we better disconnect first.
             */
            PrivateData::connectButton( this, button, false );
        }
    }
}

void QskDialogButtonBox::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->layoutEngine.orientation() == orientation )
        return;

    m_data->layoutEngine.setOrientation( orientation );

    if ( orientation == Qt::Horizontal )
        setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );

    invalidateLayout();

    Q_EMIT orientationChanged();
}

Qt::Orientation QskDialogButtonBox::orientation() const
{
    return m_data->layoutEngine.orientation();
}

QskAspect::Subcontrol QskDialogButtonBox::substitutedSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskDialogButtonBox::Panel;

    return Inherited::substitutedSubcontrol( subControl );
}

QSizeF QskDialogButtonBox::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize )
        return QSizeF(); // unlimited

    if ( ( m_data->layoutEngine.count() == 0 ) && hasChildItems() )
    {
        const_cast< QskDialogButtonBox* >( this )->rearrangeButtons();
    }

    return m_data->layoutEngine.sizeHint( which, constraint );
}

void QskDialogButtonBox::invalidateLayout()
{
    m_data->layoutEngine.clear();
    resetImplicitSize();
    polish();
}

void QskDialogButtonBox::updateLayout()
{
    auto& layoutEngine = m_data->layoutEngine;

    if ( ( layoutEngine.count() == 0 ) && hasChildItems() )
    {
        rearrangeButtons();

        if ( parentItem() && ( layoutEngine.count() > 0 ) )
            qskSendEventTo( parentItem(), QEvent::LayoutRequest );
    }

    if ( !maybeUnresized() )
        layoutEngine.setGeometries( layoutRect() );
}

void QskDialogButtonBox::rearrangeButtons()
{
    // Result differs from QDialogButtonBox. Needs more
    // investigation - TODO ...

    auto& layoutEngine = m_data->layoutEngine;
    layoutEngine.clear();

    const int* currentLayout = effectiveSkin()->dialogButtonLayout( orientation() );

    if ( m_data->centeredButtons )
        layoutEngine.addStretch();

    while ( *currentLayout != QPlatformDialogHelper::EOL )
    {
        const int role = ( *currentLayout & ~QPlatformDialogHelper::Reverse );
        const bool reverse = ( *currentLayout & QPlatformDialogHelper::Reverse );

        switch ( role )
        {
            case QPlatformDialogHelper::Stretch:
            {
                if ( !m_data->centeredButtons )
                    layoutEngine.addStretch();

                break;
            }
            case QPlatformDialogHelper::AcceptRole:
            {
                const auto& buttons = m_data->buttons[ role ];

                if ( !buttons.isEmpty() )
                    layoutEngine.addItem( buttons.first() );

                break;
            }
            case QPlatformDialogHelper::AlternateRole:
            {
                const auto& buttons = m_data->buttons[ QskDialog::AcceptRole ];

                if ( buttons.size() > 1 )
                    layoutEngine.addButtons( buttons.mid( 1 ), reverse );

                break;
            }
            case QPlatformDialogHelper::DestructiveRole:
            case QPlatformDialogHelper::RejectRole:
            case QPlatformDialogHelper::ActionRole:
            case QPlatformDialogHelper::HelpRole:
            case QPlatformDialogHelper::YesRole:
            case QPlatformDialogHelper::NoRole:
            case QPlatformDialogHelper::ApplyRole:
            case QPlatformDialogHelper::ResetRole:
            {
                const auto& buttons = m_data->buttons[ role ];

                if ( !buttons.isEmpty() )
                    layoutEngine.addButtons( buttons, reverse );

                break;
            }
        }

        ++currentLayout;
    }

    if ( m_data->centeredButtons )
        layoutEngine.addStretch();

    updateTabFocusChain();
}

void QskDialogButtonBox::updateTabFocusChain()
{
    if ( childItems().count() <= 1 )
        return;

    QQuickItem* lastItem = nullptr;

    const auto& layoutEngine = m_data->layoutEngine;
    for ( int i = 0; i < layoutEngine.count(); i++ )
    {
        if ( auto item = layoutEngine.itemAt( i ) )
        {
            if ( lastItem )
                item->stackAfter( lastItem );

            lastItem = item;
        }
    }
}

void QskDialogButtonBox::setCenteredButtons( bool centered )
{
    if ( centered != m_data->centeredButtons )
    {
        m_data->centeredButtons = centered;
        invalidateLayout();

        Q_EMIT centeredButtonsChanged();
    }
}

bool QskDialogButtonBox::centeredButtons() const
{
    return m_data->centeredButtons;
}

void QskDialogButtonBox::addButton(
    QskPushButton* button, QskDialog::ActionRole role )
{
    if ( role < 0 || role >= QskDialog::NActionRoles )
        return;

    if ( button )
    {
        if ( button->parent() == nullptr )
            button->setParent( this );

        /*
            Order of the children according to the layout rules
            will be done later in updateTabOrder
         */
        button->setParentItem( this );

        PrivateData::connectButton( this, button, true );

        m_data->buttons[ role ].removeOne( button );
        m_data->buttons[ role ] += button;
        invalidateLayout();
    }
}

void QskDialogButtonBox::addAction( QskDialog::Action action )
{
    if ( auto button = createButton( action ) )
        addButton( button, QskDialog::actionRole( action ) );
}

void QskDialogButtonBox::removeButton( QskPushButton* button )
{
    if ( button == nullptr )
        return;

    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        if ( m_data->buttons[ i ].removeOne( button ) )
        {
            PrivateData::connectButton( this, button, false );

            invalidateLayout();

            return;
        }
    }
}

QskPushButton* QskDialogButtonBox::createButton(
    QskDialog::Action action ) const
{
    return new QskDialogButton( action );
}

void QskDialogButtonBox::clear()
{
    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        qDeleteAll( m_data->buttons[ i ] );
        m_data->buttons[ i ].clear();
    }

    invalidateLayout();
}

void QskDialogButtonBox::setDefaultButton( QskPushButton* button )
{
    m_data->defaultButton = button;
}

QskPushButton* QskDialogButtonBox::defaultButton() const
{
    return m_data->defaultButton;
}

void QskDialogButtonBox::setActions( QskDialog::Actions actions )
{
    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        qDeleteAll( m_data->buttons[ i ] );
        m_data->buttons[ i ].clear();
    }

    for ( int i = QskDialog::Ok; i <= QskDialog::RestoreDefaults; i <<= 1 )
    {
        const auto action = static_cast< QskDialog::Action >( i );
        if ( action & actions )
            addAction( action );
    }

    invalidateLayout();
}

QVector< QskPushButton* > QskDialogButtonBox::buttons() const
{
    QVector< QskPushButton* > buttons;

    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
        buttons += m_data->buttons[ i ];

    return buttons;
}

QVector< QskPushButton* > QskDialogButtonBox::buttons(
    QskDialog::ActionRole role ) const
{
    if ( role < 0 || role >= QskDialog::NActionRoles )
        return QVector< QskPushButton* >();

    return m_data->buttons[ role ];
}


QskDialog::ActionRole QskDialogButtonBox::actionRole(
    const QskPushButton* button ) const
{
    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        const auto& buttons = m_data->buttons[ i ];

        for ( const auto btn : buttons )
        {
            if ( button == btn )
                return static_cast< QskDialog::ActionRole >( i );
        }
    }

    return QskDialog::InvalidRole;
}

void QskDialogButtonBox::onButtonClicked()
{
    auto button = qobject_cast< QskPushButton* >( sender() );
    if ( button == nullptr )
        return;

    switch ( actionRole( button ) )
    {
        case QskDialog::AcceptRole:
        case QskDialog::YesRole:
        {
            m_data->clickedAction = action( button );

            Q_EMIT clicked( button );
            Q_EMIT accepted();
            break;
        }
        case QskDialog::RejectRole:
        case QskDialog::NoRole:
        case QskDialog::DestructiveRole:
        {
            m_data->clickedAction = action( button );

            Q_EMIT clicked( button );
            Q_EMIT rejected();
            break;
        }
        default:
        {
            m_data->clickedAction = QskDialog::NoAction;
            Q_EMIT clicked( button );

            break;
        }
    }
}

QskDialog::Actions QskDialogButtonBox::actions() const
{
    QskDialog::Actions actions;

    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        const auto& buttons = m_data->buttons[ i ];

        for ( const auto btn : buttons )
            actions |= action( btn );
    }

    return actions;
}

QskDialog::Action QskDialogButtonBox::action( const QskPushButton* button ) const
{
    if ( button )
    {
        if ( auto dialogButton = qobject_cast< const QskDialogButton* >( button ) )
            return dialogButton->action();

        const QVariant value = button->property( "standardButton" );
        if ( value.canConvert< int >() )
            return static_cast< QskDialog::Action >( value.toInt() );
    }

    return QskDialog::NoAction;
}

QskPushButton* QskDialogButtonBox::button( QskDialog::Action action ) const
{
    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        const auto& buttons = m_data->buttons[ i ];
        for ( auto btn : buttons )
        {
            if ( this->action( btn ) == action )
                return btn;
        }
    }

    return nullptr;
}

QskDialog::Action QskDialogButtonBox::clickedAction() const
{
    return m_data->clickedAction;
}

bool QskDialogButtonBox::event( QEvent* event )
{
    switch ( static_cast< int >( event->type() ) )
    {
        case QEvent::LayoutRequest:
        {
            invalidateLayout();
            break;
        }

        case QEvent::LayoutDirectionChange:
        {
            m_data->layoutEngine.setVisualDirection(
                layoutMirroring() ? Qt::RightToLeft : Qt::LeftToRight );

            break;
        }
        case QEvent::ContentsRectChange:
        {
            polish();
            break;
        }
    }

    return Inherited::event( event );
}

void QskDialogButtonBox::itemChange(
    QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value )
{
    Inherited::itemChange( change, value );

    if ( change == ItemChildRemovedChange )
    {
        if ( auto button = qobject_cast< QskPushButton* >( value.item ) )
            removeButton( button );
    }
}

bool QskDialogButtonBox::isDefaultButtonKeyEvent( const QKeyEvent* event )
{
    if ( !event->modifiers() )
        return ( event->key() == Qt::Key_Enter ) || ( event->key() == Qt::Key_Return );

    return ( event->modifiers() & Qt::KeypadModifier ) && ( event->key() == Qt::Key_Enter );
}

#include "moc_QskDialogButtonBox.cpp"
