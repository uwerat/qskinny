/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskMenu.h"
#include "QskTextOptions.h"
#include "QskEvent.h"

#include <qquickwindow.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformtheme.h>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Graphic )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, PopupIndicator )
QSK_SUBCONTROL( QskComboBox, Splash )

QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 1 )

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )

static inline QList< Qt::Key > qskButtonPressKeys()
{
    const auto hint = QGuiApplicationPrivate::platformTheme()->themeHint(
        QPlatformTheme::ButtonPressKeys );

    return hint.value< QList< Qt::Key > >();
}

#else

static inline QList< Qt::Key > qskButtonPressKeys()
{
    static const QList< Qt::Key > keys =
        { Qt::Key_Space, Qt::Key_Enter, Qt::Key_Return, Qt::Key_Select };

    return keys;
}

#endif

class QskComboBox::PrivateData
{
  public:
    PrivateData( QskComboBox* const box )
        : menu( new QskMenu( box ) )
    {
        menu = new QskMenu();

        /*
            The popup menu is supposed to be modal for the window and
            therefore needs the root item of the window as parent item.
            So we set the box as QObject parent only, so that it gets
            destroyed properly.
         */
        menu->setParent( box );
        menu->setPopupFlag( QskPopup::DeleteOnClose, false );
    }

    QskMenu* menu;
    QString placeholderText;
};

QskComboBox::QskComboBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( this ) )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setPolishOnResize( true );

    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
    setFocusPolicy( Qt::StrongFocus );

    setAcceptHoverEvents( true );

    connect( m_data->menu, &QskMenu::triggered,
        this, &QskComboBox::activated );

    connect( m_data->menu, &QskMenu::currentIndexChanged,
        this, &QskComboBox::showOption );

    connect( m_data->menu, &QskMenu::countChanged,
        this, &QskComboBox::countChanged );

    connect( m_data->menu, &QskMenu::closed, this,
        [ this ]() { setPopupOpen( false ); setFocus( true ); } );
}

QskComboBox::~QskComboBox()
{
}

void QskComboBox::setPopupOpen( bool on )
{
    if ( on == isPopupOpen() )
        return;

    setSkinStateFlag( PopupOpen, on );

    if( on )
        openPopup();
    else
        closePopup();
}

bool QskComboBox::isPopupOpen() const
{
    return hasSkinState( PopupOpen );
}

QskGraphic QskComboBox::graphic() const
{
    const int index = currentIndex();
    if( index >= 0 )
    {
        const auto option = m_data->menu->optionAt( index );
        return option.at( 0 ).value< QskGraphic >();
    }

    return QskGraphic();
}

void QskComboBox::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskComboBox::textOptions() const
{
    return textOptionsHint( Text );
}

void QskComboBox::addOption( const QUrl& graphicSource, const QString& text )
{
    m_data->menu->addOption( graphicSource, text );
}

QVariantList QskComboBox::optionAt( int index ) const
{
    return m_data->menu->optionAt( index );
}

QString QskComboBox::placeholderText() const
{
    return m_data->placeholderText;
}

void QskComboBox::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText == text )
        return;

    m_data->placeholderText = text;

    resetImplicitSize();

    if ( currentIndex() < 0 )
        update();

    Q_EMIT placeholderTextChanged( text );
}

QString QskComboBox::currentText() const
{
    const int index = currentIndex();
    if( index >= 0 )
    {
        const auto option = optionAt( index );
        return option.at( 1 ).toString();
    }

    return placeholderText();
}

void QskComboBox::openPopup()
{
    const auto cr = contentsRect();

    auto menu = m_data->menu;

    menu->setParentItem( window()->contentItem() );
    menu->setOrigin( mapToScene( cr.bottomLeft() ) );
    menu->setFixedWidth( cr.width() );

    menu->open();
}

void QskComboBox::closePopup()
{
    m_data->menu->close();
}

void QskComboBox::mousePressEvent( QMouseEvent* )
{
    setPopupOpen( true );
}

void QskComboBox::mouseReleaseEvent( QMouseEvent* )
{
}

void QskComboBox::keyPressEvent( QKeyEvent* event )
{
    if ( qskButtonPressKeys().contains( event->key() ) )
    {
        if ( !event->isAutoRepeat() )
        {
            setPopupOpen( true );
            return;
        }
    }

    switch( event->key() )
    {
#if 0
        case Qt::Key_F4:
        {
            // QComboBox does this ???
            setPopupOpen( true );
            return;
        }
#endif
        case Qt::Key_Up:
        case Qt::Key_PageUp:
        {
            increment( -1 );
            return;
        }
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        {
            increment( 1 );
            return;
        }
        case Qt::Key_Home:
        {
            if ( count() > 0 )
                setCurrentIndex( 0 );
            return;
        }
        case Qt::Key_End:
        {
            if ( count() > 0 )
                setCurrentIndex( count() - 1 );
            return;
        }

        default:
            // searching option by key TODO ...
            break;
    }

    Inherited::keyPressEvent( event );
}

void QskComboBox::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskComboBox::wheelEvent( QWheelEvent* event )
{
    increment( -qRound( qskWheelSteps( event ) ) );
}

void QskComboBox::clear()
{
    m_data->menu->clear();
    update();
}

void QskComboBox::setCurrentIndex( int index )
{
    m_data->menu->setCurrentIndex( index );
}

int QskComboBox::currentIndex() const
{
    return m_data->menu->currentIndex();
}

int QskComboBox::count() const
{
    return m_data->menu->count();
}

void QskComboBox::showOption( int index )
{
    update();
    Q_EMIT currentIndexChanged( index );
}

void QskComboBox::increment( int steps )
{
    if ( count() == 0 )
        return;

    if ( currentIndex() == -1 && steps < 0 )
        steps++;

    int nextIndex = ( currentIndex() + steps ) % count();
    if ( nextIndex < 0 )
        nextIndex += count();

    if ( nextIndex != currentIndex() )
    {
        m_data->menu->setCurrentIndex( nextIndex );
        Q_EMIT activated( nextIndex );
    }
}

#include "moc_QskComboBox.cpp"
