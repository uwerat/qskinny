/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskWindowOrSubWindow.h"

#include "QskDialogButtonBox.h"
#include "QskFocusIndicator.h"

// copied from QskDialog.cpp:
static QskDialog::Action qskActionCandidate( const QskDialogButtonBox* buttonBox )
{
    // not the fastest code ever, but usually we always
    // have a AcceptRole or YesRole button

    const QskDialog::ActionRole candidates[] =
    {
        QskDialog::AcceptRole, QskDialog::YesRole,
        QskDialog::RejectRole, QskDialog::NoRole, QskDialog::DestructiveRole,
        QskDialog::UserRole, QskDialog::ResetRole,
        QskDialog::ApplyRole, QskDialog::HelpRole
    };

    for ( auto role : candidates )
    {
        const auto& buttons = buttonBox->buttons( role );
        if ( !buttons.isEmpty() )
            return buttonBox->action( buttons.first() );
    }

    return QskDialog::NoAction;
}

static QskDialog::DialogCode qskExec( QskDialogWindow* dialogWindow )
{
#if 1
    auto focusIndicator = new QskFocusIndicator();
    focusIndicator->setObjectName( QStringLiteral( "DialogFocusIndicator" ) );
    dialogWindow->addItem( focusIndicator );
#endif

    return dialogWindow->exec();
}


template< typename W >
QskWindowOrSubWindow< W >::QskWindowOrSubWindow( QObject* parent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction )
    : W( parent, title, actions, defaultAction )
{
}


QskWindowOrSubWindow< QskDialogWindow >::QskWindowOrSubWindow( QObject* parent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction )
    : QskDialogWindow( static_cast< QWindow* >( parent ) )
{
    auto* transientParent = static_cast< QWindow* >( parent );
    setTransientParent( transientParent );

    setTitle( title );
    setDialogActions( actions );

    if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
        defaultAction = qskActionCandidate( buttonBox() );

    setDefaultDialogAction( defaultAction );

    setModality( transientParent ? Qt::WindowModal : Qt::ApplicationModal );

    const QSize size = sizeConstraint();

    if ( this->parent() )
    {
        QRect r( QPoint(), size );
        r.moveCenter( QRect( QPoint(), this->parent()->size() ).center() );

        setGeometry( r );
    }

    auto adjustSize = [this]()
    {
        const QSize size = sizeConstraint();

        if ( size.isValid() )
        {
            setFlags( flags() | Qt::MSWindowsFixedSizeDialogHint );
            setFixedSize( size );
        }
    };

    connect( contentItem(), &QQuickItem::widthChanged, this, adjustSize );
    connect( contentItem(), &QQuickItem::heightChanged, this, adjustSize );
}

QskDialog::DialogCode QskWindowOrSubWindow< QskDialogWindow >::exec()
{
    return qskExec( this );
}

void QskWindowOrSubWindow< QskDialogWindow >::setContentItem( QQuickItem* item )
{
    QskDialogWindow::setDialogContentItem( item );
}

QskWindowOrSubWindow< QskDialogSubWindow >::QskWindowOrSubWindow( QObject* parent, const QString& title,
    QskDialog::Actions actions, QskDialog::Action defaultAction )
    : QskDialogSubWindow( static_cast< QQuickWindow* >( parent )->contentItem() )
{
    setPopupFlag( QskPopup::DeleteOnClose );
    setModal( true );
    setTitle( title );
    setDialogActions( actions );

    if ( actions != QskDialog::NoAction && defaultAction == QskDialog::NoAction )
        defaultAction = qskActionCandidate( buttonBox() );

    setDefaultDialogAction( defaultAction );
}

QskDialog::DialogCode QskWindowOrSubWindow< QskDialogSubWindow >::exec()
{
    return QskDialogSubWindow::exec();
}

void QskWindowOrSubWindow< QskDialogSubWindow >::setContentItem( QQuickItem* item )
{
    QskDialogSubWindow::setContentItem( item );
}
