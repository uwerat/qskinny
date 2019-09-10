/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButtonBox.h"
#include "QskDialogButton.h"
#include "QskLinearBox.h"
#include "QskSkin.h"

#include <qevent.h>
#include <qpointer.h>
#include <qvector.h>

#include <qpa/qplatformdialoghelper.h>
#include <qpa/qplatformtheme.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskDialogButtonBox, Panel )

static void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline QskDialog::ActionRole qskActionRole( QskDialog::Action action )
{
    const auto role = QPlatformDialogHelper::buttonRole(
        static_cast< QPlatformDialogHelper::StandardButton >( action ) );

    return static_cast< QskDialog::ActionRole >( role );
}

static void qskAddToLayout( const QVector< QskPushButton* >& buttons,
    bool reverse, QskLinearBox* layoutBox )
{
    if ( reverse )
    {
        for ( int i = buttons.count() - 1; i >= 0; i-- )
            layoutBox->addItem( buttons[ i ] );
    }
    else
    {
        for ( int i = 0; i < buttons.count(); i++ )
            layoutBox->addItem( buttons[ i ] );
    }
}

class QskDialogButtonBox::PrivateData
{
  public:
    PrivateData()
        : centeredButtons( false )
        , dirtyLayout( false )
    {
    }

    QskLinearBox* layoutBox = nullptr;
    QVector< QskPushButton* > buttons[ QskDialog::NActionRoles ];
    QPointer< QskPushButton > defaultButton;

    QskDialog::Action clickedAction = QskDialog::NoAction;

    bool centeredButtons : 1;
    bool dirtyLayout : 1;
};

QskDialogButtonBox::QskDialogButtonBox( QQuickItem* parent )
    : QskDialogButtonBox( Qt::Horizontal, parent )
{
}

QskDialogButtonBox::QskDialogButtonBox( Qt::Orientation orientation, QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );
    setOrientation( orientation );
}

QskDialogButtonBox::~QskDialogButtonBox()
{
}

void QskDialogButtonBox::setOrientation( Qt::Orientation orientation )
{
    if ( m_data->layoutBox && m_data->layoutBox->orientation() == orientation )
        return;

    delete m_data->layoutBox;

    m_data->layoutBox = new QskLinearBox( orientation, this );
    m_data->layoutBox->setObjectName( QStringLiteral( "DialogButtonBoxLayout" ) );

    if ( orientation == Qt::Horizontal )
        setSizePolicy( QskSizePolicy::Preferred, QskSizePolicy::Fixed );
    else
        setSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Preferred );

    invalidateLayout();

    Q_EMIT orientationChanged();
}

Qt::Orientation QskDialogButtonBox::orientation() const
{
    return m_data->layoutBox->orientation();
}

QskAspect::Subcontrol QskDialogButtonBox::effectiveSubcontrol(
    QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskBox::Panel )
        return QskDialogButtonBox::Panel;

    return Inherited::effectiveSubcontrol( subControl );
}

QSizeF QskDialogButtonBox::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( m_data->dirtyLayout )
    {
        const_cast< QskDialogButtonBox* >( this )->rearrangeButtons();
        m_data->dirtyLayout = false;
    }

    return m_data->layoutBox->effectiveSizeHint( which, constraint );
}

void QskDialogButtonBox::invalidateLayout()
{
    m_data->dirtyLayout = true;

    resetImplicitSize();
    polish();
}

void QskDialogButtonBox::updateLayout()
{
    if ( m_data->dirtyLayout )
    {
        rearrangeButtons();
        m_data->dirtyLayout = false;

        if ( parentItem() )
            qskSendEventTo( parentItem(), QEvent::LayoutRequest );
    }

    m_data->layoutBox->setGeometry( layoutRect() );
}

void QskDialogButtonBox::rearrangeButtons()
{
    // Result differs from QDialogButtonBox. Needs more
    // investigation - TODO ...

    auto layoutBox = m_data->layoutBox;

    layoutBox->clear();

    const int* currentLayout = effectiveSkin()->dialogButtonLayout( orientation() );

    if ( m_data->centeredButtons )
        layoutBox->addStretch( 10 );

    while ( *currentLayout != QPlatformDialogHelper::EOL )
    {
        const int role = ( *currentLayout & ~QPlatformDialogHelper::Reverse );
        const bool reverse = ( *currentLayout & QPlatformDialogHelper::Reverse );

        switch ( role )
        {
            case QPlatformDialogHelper::Stretch:
            {
                if ( !m_data->centeredButtons )
                    layoutBox->addStretch( 1 );

                break;
            }
            case QPlatformDialogHelper::AcceptRole:
            {
                const auto& buttons = m_data->buttons[ role ];

                if ( !buttons.isEmpty() )
                    layoutBox->addItem( buttons.first() );

                break;
            }
            case QPlatformDialogHelper::AlternateRole:
            {
                const auto& buttons = m_data->buttons[ QskDialog::AcceptRole ];

                if ( buttons.size() > 1 )
                    qskAddToLayout( buttons.mid( 1 ), reverse, layoutBox );

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
                    qskAddToLayout( buttons, reverse, layoutBox );

                break;
            }
        }

        ++currentLayout;
    }

    if ( m_data->centeredButtons )
        layoutBox->addStretch( 1 );

    // reorganizing the tab chain ???
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
            To have a proper ownership. Inserting the buttons
            according to the layout rules will be done later
         */
        button->setParentItem( m_data->layoutBox );

        connect( button, &QskPushButton::clicked, this,
            &QskDialogButtonBox::onButtonClicked );

        m_data->buttons[ role ] += button;
        invalidateLayout();
    }
}

void QskDialogButtonBox::addAction( QskDialog::Action action )
{
    QskPushButton* button = createButton( action );
    if ( button )
        addButton( button, qskActionRole( action ) );
}

void QskDialogButtonBox::removeButton( QskPushButton* button )
{
    // ChildRemove Events !!!

    if ( button == nullptr )
        return;

    for ( int i = 0; i < QskDialog::NActionRoles; i++ )
    {
        auto& buttons = m_data->buttons[ i ];
        if ( buttons.removeOne( button ) )
        {
            disconnect( button, &QskPushButton::clicked,
                this, &QskDialogButtonBox::onButtonClicked );

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
    if ( event->type() == QEvent::LayoutRequest )
    {
        if ( !m_data->dirtyLayout )
            resetImplicitSize();
    }

    return Inherited::event( event );
}

bool QskDialogButtonBox::isDefaultButtonKeyEvent( const QKeyEvent* event )
{
    if ( event->modifiers() & Qt::KeypadModifier && event->key() == Qt::Key_Enter )
    {
        return ( event->modifiers() & Qt::KeypadModifier )
            && ( event->key() == Qt::Key_Enter );
    }
    else
    {
        return ( event->key() == Qt::Key_Enter ) ||
            ( event->key() == Qt::Key_Return );
    }
}

QString QskDialogButtonBox::buttonText( QskDialog::Action action )
{
    // should be redirected through the skin !

    const QPlatformTheme* theme = QGuiApplicationPrivate::platformTheme();
    QString text = theme->standardButtonText( action );

#if QT_VERSION < QT_VERSION_CHECK( 5, 7, 0 )
    text.remove( '&' );
#else
    text = QPlatformTheme::removeMnemonics( text );
#endif

    return text;
}

#include "moc_QskDialogButtonBox.cpp"
