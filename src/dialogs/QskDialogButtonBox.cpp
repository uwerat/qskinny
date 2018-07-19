/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogButtonBox.h"
#include "QskDialogButton.h"
#include "QskLinearBox.h"
#include "QskSkin.h"

#include <qevent.h>
#include <qpa/qplatformtheme.h>
#include <qpa/qplatformdialoghelper.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

QSK_SUBCONTROL( QskDialogButtonBox, Panel )

static void qskSendEventTo( QObject* object, QEvent::Type type )
{
    QEvent event( type );
    QCoreApplication::sendEvent( object, &event );
}

static inline QskDialog::ButtonRole qskButtonRole(
    QskDialog::StandardButton standardButton )
{
    QPlatformDialogHelper::StandardButton sb =
        static_cast< QPlatformDialogHelper::StandardButton >( standardButton );

    QPlatformDialogHelper::ButtonRole role = QPlatformDialogHelper::buttonRole( sb );

    return static_cast< QskDialog::ButtonRole >( role );
}

static void qskAddToLayout( const QList< QskPushButton* >& buttonList,
    bool reverse, QskLinearBox* layoutBox )
{
    if ( reverse )
    {
        for ( int i = buttonList.count() - 1; i >= 0; i-- )
            layoutBox->addItem( buttonList[i] );
    }
    else
    {
        for ( int i = 0; i < buttonList.count(); i++ )
            layoutBox->addItem( buttonList[i] );
    }
}

class QskDialogButtonBox::PrivateData
{
public:
    PrivateData():
        layoutBox( nullptr ),
        centeredButtons( false ),
        dirtyLayout( false ),
        clickedButton( QskDialog::NoButton )
    {
    }

    QskLinearBox* layoutBox;

    QList< QskPushButton* > buttonLists[QskDialog::NButtonRoles];

    bool centeredButtons : 1;
    bool dirtyLayout : 1;

    QskDialog::StandardButton clickedButton;
};

QskDialogButtonBox::QskDialogButtonBox( QQuickItem* parent ):
    QskDialogButtonBox( Qt::Horizontal, parent )
{
}

QskDialogButtonBox::QskDialogButtonBox( Qt::Orientation orientation, QQuickItem* parent ):
    Inherited( parent ),
    m_data( new PrivateData() )
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

QSizeF QskDialogButtonBox::contentsSizeHint() const
{
    if ( m_data->dirtyLayout )
    {
        const_cast< QskDialogButtonBox* >( this )->rearrangeButtons();
        m_data->dirtyLayout = false;
    }

    return outerBoxSize( Panel, m_data->layoutBox->contentsSizeHint() );
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
    // Result differs from the widget counter parts. Needs more
    // investigation - TODO ...

    auto layoutBox = m_data->layoutBox;

    const bool isActive = layoutBox->isActive();
    layoutBox->setActive( false );

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
                const QList< QskPushButton* >& buttons = m_data->buttonLists[role];

                if ( !buttons.isEmpty() )
                    layoutBox->addItem( buttons.first() );

                break;
            }
            case QPlatformDialogHelper::AlternateRole:
            {
                const QList< QskPushButton* >& buttons =
                    m_data->buttonLists[QskDialog::AcceptRole];

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
                const QList< QskPushButton* > buttons = m_data->buttonLists[role];

                if ( !buttons.isEmpty() )
                    qskAddToLayout( buttons, reverse, layoutBox );

                break;
            }
        }

        ++currentLayout;
    }

    if ( m_data->centeredButtons )
        layoutBox->addStretch( 1 );

    layoutBox->setActive( isActive );

    // organizing the tab chain ???
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

void QskDialogButtonBox::addButton( QskPushButton* button, QskDialog::ButtonRole role )
{
    if ( role < 0 || role >= QskDialog::NButtonRoles )
        return;

    if ( button )
    {
        if ( button->parent() == nullptr )
            button->setParent( this );

        connect( button, &QskPushButton::clicked, this,
            &QskDialogButtonBox::onButtonClicked );

        m_data->buttonLists[role] += button;
        invalidateLayout();
    }
}

void QskDialogButtonBox::addButton( QskDialog::StandardButton standardButton )
{
    QskPushButton* button = createButton( standardButton );
    if ( button )
        addButton( button, qskButtonRole( standardButton ) );
}

void QskDialogButtonBox::removeButton( QskPushButton* button )
{
    // ChildRemove Events !!!

    if ( button == nullptr )
        return;

    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        QList< QskPushButton* >& buttons = m_data->buttonLists[i];
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
    QskDialog::StandardButton standardButton ) const
{
    return new QskDialogButton( standardButton );
}

void QskDialogButtonBox::clear()
{
    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        for ( const QQuickItem* button : qskAsConst( m_data->buttonLists[i] ) )
            delete button;
    }

    invalidateLayout();
}

void QskDialogButtonBox::setStandardButtons(
    QskDialog::StandardButtons standardButton )
{
    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        for ( const QQuickItem* button : qskAsConst( m_data->buttonLists[i] ) )
            delete button;
    }

    for ( int i = QskDialog::Ok; i <= QskDialog::RestoreDefaults; i <<= 1 )
    {
        const QskDialog::StandardButton id = static_cast< QskDialog::StandardButton >( i );
        if ( id & standardButton )
            addButton( id );
    }

    invalidateLayout();
}

QList< QskPushButton* > QskDialogButtonBox::buttons() const
{
    QList< QskPushButton* > buttons;

    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
        buttons += m_data->buttonLists[i];

    return buttons;
}

QskDialog::ButtonRole QskDialogButtonBox::buttonRole( const QskPushButton* button ) const
{
    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        const QList< QskPushButton* >& buttons = m_data->buttonLists[i];

        for ( const QskPushButton* btn : buttons )
        {
            if ( button == btn )
                return static_cast< QskDialog::ButtonRole >( i );
        }
    }

    return QskDialog::InvalidRole;
}

QskDialog::StandardButton QskDialogButtonBox::defaultButtonCandidate() const
{
    // not the fastest code ever, but usually we always
    // have a AcceptRole or YesRole button

    static const QVector< QskDialog::ButtonRole > fallBackRoles =
    {
        QskDialog::AcceptRole, QskDialog::YesRole,
        QskDialog::RejectRole, QskDialog::NoRole, QskDialog::DestructiveRole,
        QskDialog::ActionRole, QskDialog::ResetRole,
        QskDialog::ApplyRole, QskDialog::HelpRole
    };

    for ( auto role : fallBackRoles )
    {
        QskDialog::StandardButton defaultButton =
            standardButton( buttonFromRole( role ) );

        if ( defaultButton != QskDialog::NoButton )
            return defaultButton;
    }

    return QskDialog::NoButton;
}

void QskDialogButtonBox::onButtonClicked()
{
    QskPushButton* button = qobject_cast< QskPushButton* >( sender() );
    if ( button == nullptr )
        return;

    switch( buttonRole( button ) )
    {
        case QskDialog::AcceptRole:
        case QskDialog::YesRole:
        {
            m_data->clickedButton = standardButton( button );

            Q_EMIT clicked( button );
            Q_EMIT accepted();
            break;
        }
        case QskDialog::RejectRole:
        case QskDialog::NoRole:
        case QskDialog::DestructiveRole:
        {
            m_data->clickedButton = standardButton( button );

            Q_EMIT clicked( button );
            Q_EMIT rejected();
            break;
        }
        default:
        {
            m_data->clickedButton = QskDialog::NoButton;
            Q_EMIT clicked( button );

            break;
        }
    }
}

QskDialog::StandardButtons QskDialogButtonBox::standardButtons() const
{
    QskDialog::StandardButtons standardButtons;

    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        const QList< QskPushButton* >& buttons = m_data->buttonLists[i];

        for ( const QskPushButton* btn : buttons )
            standardButtons |= this->standardButton( btn );
    }

    return standardButtons;
}

QskDialog::StandardButton QskDialogButtonBox::standardButton( const QskPushButton* button ) const
{
    if ( button )
    {
        if ( auto dialogButton = qobject_cast< const QskDialogButton* >( button ) )
            return dialogButton->standardButton();

        const QVariant value = button->property( "standardButton" );
        if ( value.canConvert< int >() )
            return static_cast< QskDialog::StandardButton >( value.toInt() );
    }

    return QskDialog::NoButton;
}

QskPushButton* QskDialogButtonBox::button(
    QskDialog::StandardButton standardButton ) const
{
    for ( int i = 0; i < QskDialog::NButtonRoles; i++ )
    {
        const QList< QskPushButton* >& buttons = m_data->buttonLists[i];
        for ( QskPushButton* btn : buttons )
        {
            if ( this->standardButton( btn ) == standardButton )
                return btn;
        }
    }

    return nullptr;
}

QskPushButton* QskDialogButtonBox::buttonFromRole( QskDialog::ButtonRole role ) const
{
    if ( role < 0 || role >= QskDialog::NButtonRoles )
        return nullptr;

    const auto& buttonList = m_data->buttonLists[role];
    return buttonList.isEmpty() ? nullptr : buttonList.first();
}

QskDialog::StandardButton QskDialogButtonBox::clickedButton() const
{
    return m_data->clickedButton;
}

bool QskDialogButtonBox::event( QEvent* event )
{
    if ( event->type() == QEvent::LayoutRequest )
        resetImplicitSize();

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

QString QskDialogButtonBox::buttonText( QskDialog::StandardButton standardButton )
{
    const QPlatformTheme* theme = QGuiApplicationPrivate::platformTheme();
    QString text = theme->standardButtonText( standardButton );

#if QT_VERSION < QT_VERSION_CHECK( 5, 7, 0 )
    text.remove( '&' );
#else
    text = QPlatformTheme::removeMnemonics( text );
#endif

    return text;
}

#include "moc_QskDialogButtonBox.cpp"
