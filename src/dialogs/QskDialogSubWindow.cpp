/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskDialogSubWindow.h"
#include "QskDialogButtonBox.h"
#include "QskTextLabel.h"
#include "QskPushButton.h"
#include "QskLinearBox.h"
#include "QskQuick.h"
#include "QskEvent.h"
#if 1
#include "QskSkin.h"
#include <QskPlatform.h>
#endif

#include <qquickwindow.h>
#include <qpointer.h>

QSK_SUBCONTROL( QskDialogSubWindow, DialogTitle )

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

namespace
{
    class TitleLabel final : public QskTextLabel
    {
      protected:
        QskAspect::Subcontrol substitutedSubcontrol(
            QskAspect::Subcontrol subControl ) const override
        {
            if ( subControl == QskTextLabel::Text )
                return QskDialogSubWindow::DialogTitle;

            return QskTextLabel::substitutedSubcontrol( subControl );
        }
    };
}

class QskDialogSubWindow::PrivateData
{
  public:
    QskDialog::Actions actions = QskDialog::NoAction;

    QskTextLabel* titleLabel = nullptr;
    QskDialogButtonBox* buttonBox = nullptr;

    QPointer< QQuickItem > contentItem;

    QskLinearBox* layout = nullptr;

    QskDialog::DialogCode result = QskDialog::Rejected;
};

QskDialogSubWindow::QskDialogSubWindow( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    qskSetRejectOnClose( this, true );

    m_data->layout = new QskLinearBox( Qt::Vertical, this );
    m_data->layout->setSizePolicy(
        QskSizePolicy::MinimumExpanding, QskSizePolicy::Constrained );

    setPolishOnResize( true );
    setPolishOnParentResize( true );
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
        delete m_data->buttonBox;
        m_data->buttonBox = nullptr;
    }
    else
    {
        if ( m_data->buttonBox == nullptr )
            initButtonBox();

        if ( m_data->buttonBox )
            m_data->buttonBox->setActions( actions );
    }
}

QskDialog::Actions QskDialogSubWindow::dialogActions() const
{
    if ( m_data->buttonBox )
        return m_data->buttonBox->actions();

    return QskDialog::NoAction;
}

void QskDialogSubWindow::setTitle( const QString& title )
{
    bool changed = false;
    auto& titleLabel = m_data->titleLabel;

    if ( title.isEmpty() )
    {
        changed = ( titleLabel != nullptr );

        delete titleLabel;
        titleLabel = nullptr;
    }
    else
    {
        if ( titleLabel == nullptr )
        {
            titleLabel = new TitleLabel();
            m_data->layout->insertItem( 0, titleLabel );
            changed = true;
        }
        else
        {
            changed = ( titleLabel->text() != title );
        }

        if ( changed )
            titleLabel->setText( title );
    }

    if ( changed )
    {
        resetImplicitSize();
        polish();

        Q_EMIT titleChanged( title );
    }
}

QString QskDialogSubWindow::title() const
{
    if ( auto label = m_data->titleLabel )
        return label->text();

    return QString();
}

QskTextLabel* QskDialogSubWindow::titleLabel()
{
    return m_data->titleLabel;
}

const QskTextLabel* QskDialogSubWindow::titleLabel() const
{
    return m_data->titleLabel;
}

void QskDialogSubWindow::setContentItem( QQuickItem* item )
{
    if ( item == m_data->contentItem )
        return;

    if ( m_data->contentItem )
    {
        if ( m_data->contentItem->parent() == m_data->layout )
            delete m_data->contentItem;
        else
            m_data->layout->removeItem( m_data->contentItem );
    }

    m_data->contentItem = item;

    if ( item )
    {
        const int index = m_data->titleLabel ? 1 : 0;
        m_data->layout->insertItem( index, m_data->contentItem );
    }
}

QQuickItem* QskDialogSubWindow::contentItem() const
{
    return m_data->contentItem;
}

void QskDialogSubWindow::setContentPadding( const QMarginsF& padding )
{
    // should be a skin hint ???
    m_data->layout->setMargins( padding );
}

QMarginsF QskDialogSubWindow::contentPadding() const
{
    return m_data->layout->margins();
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
    m_data->result = QskDialog::Rejected;
    ( void ) execPopup();
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
        {
            button->click();
            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskDialogSubWindow::initButtonBox()
{
    auto buttonBox = new QskDialogButtonBox();
    m_data->layout->addItem( buttonBox );

    connect( buttonBox, &QskDialogButtonBox::accepted,
        this, &QskDialogSubWindow::accept );

    connect( buttonBox, &QskDialogButtonBox::rejected,
        this, &QskDialogSubWindow::reject );

    m_data->buttonBox = buttonBox;
}

void QskDialogSubWindow::updateLayout()
{
    updateGeometry();
    m_data->layout->setGeometry( layoutRect() );
}

void QskDialogSubWindow::updateGeometry()
{
    /*
        This code is for Preferred/Constrained without checking
        the actual sizePolicy. TODO ...
     */
    const auto minSize = minimumSize();
    const auto maxSize = maximumSize();

    auto width = sizeHint().width();
    width = qMin( width, maxSize.width() );
    width = qMax( width, minSize.width() );

    auto height = heightForWidth( width );
    height = qMin( height, maxSize.height() );
    height = qMax( height, minSize.height() );

    QRectF rect( 0.0, 0.0, width, height );
    rect.moveCenter( qskItemRect( parentItem() ).center() );

    setGeometry( rect );
}

QSizeF QskDialogSubWindow::layoutSizeHint(
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which == Qt::MaximumSize && polishOnParentResize() )
        return 0.9 * parentItem()->size();

    auto size = m_data->layout->effectiveSizeHint( which, constraint );

    if ( which == Qt::MinimumSize )
    {
        const auto w = qMax( qskDpToPixels( 300.0 ), size.width() );
        size.setWidth( w );
    }

    return size;
}

#include "moc_QskDialogSubWindow.cpp"
