/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskDialogSubWindow.h"
#include "QskDialogButtonBox.h"
#include "QskPushButton.h"
#include "QskQuick.h"

#include <qeventloop.h>
#include <qquickwindow.h>
#include <qpointer.h>

static inline void qskSetRejectOnClose( QskDialogSubWindow* subWindow, bool on )
{
    if ( on )
    {
        QObject::connect( subWindow, &QskPopup::closed,
            subWindow, &QskDialogSubWindow::reject );
    }
    else
    {
        QObject::disconnect( subWindow, &QskPopup::closed,
            subWindow, &QskDialogSubWindow::reject );
    }
}

class QskDialogSubWindow::PrivateData
{
  public:
    QskDialog::Actions actions = QskDialog::NoAction;

    QPointer< QQuickItem > contentItem;
    QskDialogButtonBox* buttonBox = nullptr;

    QMarginsF contentPadding;

    QskDialog::DialogCode result = QskDialog::Rejected;
};

QskDialogSubWindow::QskDialogSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    setPolishOnResize( true );
    qskSetRejectOnClose( this, true );
}

QskDialogSubWindow::~QskDialogSubWindow()
{
}

void QskDialogSubWindow::addDialogAction( QskDialog::Action action )
{
    if ( action != QskDialog::NoAction )
    {
        if ( m_data->buttonBox == nullptr )
            initButtonBox();

        if ( m_data->buttonBox )
            m_data->buttonBox->addAction( action );
    }
}

void QskDialogSubWindow::addDialogButton(
    QskPushButton* button, QskDialog::ActionRole actionRole )
{
    if ( button )
    {
        if ( m_data->buttonBox == nullptr )
            initButtonBox();

        if ( m_data->buttonBox )
            m_data->buttonBox->addButton( button, actionRole );
    }
}

void QskDialogSubWindow::setDialogActions( QskDialog::Actions actions )
{
    if ( m_data->actions == actions )
        return;

    m_data->actions = actions;

    if ( actions == QskDialog::NoAction )
    {
        if ( m_data->buttonBox->parent() == this )
        {
            delete m_data->buttonBox;
        }
        else
        {
            m_data->buttonBox->setParentItem( nullptr );

            disconnect( m_data->buttonBox, &QskDialogButtonBox::accepted,
                this, &QskDialogSubWindow::accept );

            disconnect( m_data->buttonBox, &QskDialogButtonBox::rejected,
                this, &QskDialogSubWindow::reject );

        }

        m_data->buttonBox = nullptr;
    }
    else
    {
        if ( m_data->buttonBox == nullptr )
        {
            initButtonBox();
        }

        if ( m_data->buttonBox )
            m_data->buttonBox->setActions( actions );
    }

    resetImplicitSize();
    polish();
}

QskDialog::Actions QskDialogSubWindow::dialogActions() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->actions();

    return QskDialog::NoAction;
}

void QskDialogSubWindow::setContentItem( QQuickItem* item )
{
    if ( item == m_data->contentItem )
        return;

    if ( m_data->contentItem )
    {
        if ( m_data->contentItem->parent() == this )
            delete m_data->contentItem;
        else
            m_data->contentItem->setParentItem( nullptr );
    }

    m_data->contentItem = item;

    if ( item )
    {
        item->setParentItem( this );

        if ( item->parent() == nullptr )
            item->setParent( this );
    }

    resetImplicitSize();
    polish();
}

QQuickItem* QskDialogSubWindow::contentItem() const
{
    return m_data->contentItem;
}

void QskDialogSubWindow::setContentPadding( const QMarginsF& padding )
{
    // should be a skin hint ???

    if ( m_data->contentPadding != padding )
    {
        m_data->contentPadding = padding;

        resetImplicitSize();
        polish();
    }
}

QMarginsF QskDialogSubWindow::contentPadding() const
{
    return m_data->contentPadding;
}

void QskDialogSubWindow::setDefaultDialogAction( QskDialog::Action action )
{
    QskPushButton* button = nullptr;

    if ( m_data->buttonBox )
        button = m_data->buttonBox->button( action );

    setDefaultButton( button );
}

void QskDialogSubWindow::setDefaultButton( QskPushButton* button )
{
    if ( !qskIsAncestorOf( m_data->buttonBox, button ) )
    {
#if defined( QT_DEBUG )
        qWarning( "Only buttons of the QskDialogButtonBox can be the default button." );
#endif
        return;
    }

    m_data->buttonBox->setDefaultButton( button );
}

QskPushButton* QskDialogSubWindow::defaultButton() const
{
    if ( m_data->buttonBox == nullptr )
        return nullptr;

    return m_data->buttonBox->defaultButton();
}

QskDialogButtonBox* QskDialogSubWindow::buttonBox()
{
    return m_data->buttonBox;
}

const QskDialogButtonBox* QskDialogSubWindow::buttonBox() const
{
    return m_data->buttonBox;
}

QskDialog::Action QskDialogSubWindow::clickedAction() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->clickedAction();

    return QskDialog::NoAction;
}

void QskDialogSubWindow::setResult( QskDialog::DialogCode result )
{
    m_data->result = result;
}

QskDialog::DialogCode QskDialogSubWindow::result() const
{
    return m_data->result;
}

QskDialog::DialogCode QskDialogSubWindow::exec()
{
    if ( priority() > 0 )
    {
        qWarning( "illegal call of QskDialogSubWindow::exec "
                  "for a subwindow with non default priority." );
    }

    open();

    //check for window after call to open(), because maybe a popupmanager assigns a window on open.
    if ( window() == nullptr )
    {
        qWarning( "trying to exec a subwindow without window." );
        return QskDialog::Rejected;
    }

    if ( auto mouseGrabber = window()->mouseGrabberItem() )
    {
        // when being called from QQuickWindow::mouseReleaseEvent
        // the mouse grabber has not yet been released.

        if( !qskIsAncestorOf( this, mouseGrabber ) )
            qskUngrabMouse( mouseGrabber );
    }

    QEventLoop eventLoop;

    connect( this, &QskDialogSubWindow::finished, &eventLoop, &QEventLoop::quit );
    ( void ) eventLoop.exec( QEventLoop::DialogExec );

    return m_data->result;
}

void QskDialogSubWindow::done( QskDialog::DialogCode result )
{
    m_data->result = result;

    if ( isOpen() )
    {
        qskSetRejectOnClose( this, false );
        close();
        qskSetRejectOnClose( this, true );
    }

    Q_EMIT finished( result );

    if ( result == QskDialog::Accepted )
        Q_EMIT accepted();
    else
        Q_EMIT rejected();
}

void QskDialogSubWindow::accept()
{
    done( QskDialog::Accepted );
}

void QskDialogSubWindow::reject()
{
    done( QskDialog::Rejected );
}

void QskDialogSubWindow::keyPressEvent( QKeyEvent* event )
{
    if ( m_data->buttonBox &&
        QskDialogButtonBox::isDefaultButtonKeyEvent( event ) )
    {
        auto button = m_data->buttonBox->defaultButton();
        if ( button && button->isEnabled() )
            button->click();
    }

    if ( event->matches( QKeySequence::Cancel ) )
    {
        // using shortcuts instead ???

        reject();
        return;
    }

    Inherited::keyPressEvent( event );
}

QskDialogButtonBox* QskDialogSubWindow::createButtonBox()
{
    return new QskDialogButtonBox();
}

void QskDialogSubWindow::initButtonBox()
{
    m_data->buttonBox = createButtonBox();

    if ( m_data->buttonBox )
    {
        m_data->buttonBox->setParentItem( this );

        if ( m_data->buttonBox->parent() == nullptr )
            m_data->buttonBox->setParent( this );

        connect( m_data->buttonBox, &QskDialogButtonBox::accepted,
            this, &QskDialogSubWindow::accept, Qt::UniqueConnection );

        connect( m_data->buttonBox, &QskDialogButtonBox::rejected,
            this, &QskDialogSubWindow::reject, Qt::UniqueConnection );
    }
}

void QskDialogSubWindow::aboutToShow()
{
    if ( size().isEmpty() )
    {
        // setting an initial size from the hint, centered inside the window

        const qreal cx = 0.5 * parentItem()->width();
        const qreal cy = 0.5 * parentItem()->height();

        QRectF rect;
        rect.setSize( sizeConstraint() );
        rect.moveCenter( QPointF( cx, cy ) );

        setGeometry( rect );
    }

    Inherited::aboutToShow();
}

void QskDialogSubWindow::updateLayout()
{
    Inherited::updateLayout();

    auto rect = layoutRect();

    if ( m_data->buttonBox && m_data->buttonBox->isVisibleToParent() )
    {
        const auto h = m_data->buttonBox->sizeConstraint().height();
        rect.setBottom( rect.bottom() - h );

        m_data->buttonBox->setGeometry( rect.x(), rect.bottom(), rect.width(), h );
    }

    if ( m_data->contentItem )
    {
        rect = rect.marginsRemoved( m_data->contentPadding );
        qskSetItemGeometry( m_data->contentItem, rect );
    }
}

QSizeF QskDialogSubWindow::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    QSizeF buttonBoxHint;

    qreal constraintHeight = constraint.height();

    if ( auto buttonBox = m_data->buttonBox )
    {
        if ( buttonBox->isVisibleToLayout() )
        {
            buttonBoxHint = buttonBox->sizeConstraint(
                which, QSizeF( constraint.width(), -1 ) );

            if ( constraint.width() >= 0.0 )
                buttonBoxHint.rwidth() = constraint.width();

            if ( constraintHeight >= 0.0 && buttonBoxHint.height() >= 0.0 )
            {
                constraintHeight -= buttonBoxHint.height();
                constraintHeight = qMax( constraintHeight, 0.0 );
            }
        }
    }

    QSizeF contentHint;

    if ( qskIsVisibleToLayout( m_data->contentItem ) )
    {
        const auto& m = m_data->contentPadding;
        const qreal dw = m.left() + m.right();
        const qreal dh = m.top() + m.bottom();

        qreal constraintWidth = constraint.width();

        if ( constraintWidth > 0.0 )
            constraintWidth = qMax( constraintWidth - dw, 0.0 );

        if ( constraintHeight > 0.0 )
            constraintHeight = qMax( constraintHeight - dh, 0.0 );

        contentHint = qskSizeConstraint( m_data->contentItem,
            which, QSizeF( constraintWidth, constraintHeight ) );

        if ( contentHint.width() >= 0 )
            contentHint.rwidth() += dw;

        if ( contentHint.height() >= 0 )
            contentHint.rheight() += dh;
    }

    qreal w = -1;
    w = qMax( w, buttonBoxHint.width() );
    w = qMax( w, contentHint.width() );

    qreal h = -1;

    if ( buttonBoxHint.height() > 0.0 && contentHint.height() > 0.0 )
        h = buttonBoxHint.height() + contentHint.height();

    return QSizeF( w, h );
}

#include "moc_QskDialogSubWindow.cpp"
