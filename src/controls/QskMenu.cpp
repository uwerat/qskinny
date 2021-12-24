#include "QskMenu.h"

#include <QskGraphicProvider.h>
#include <QskTextOptions.h>
#include <QskGraphic.h>
#include <QskColorFilter.h>

#include <qvector.h>

QSK_SUBCONTROL( QskMenu, Panel )
QSK_SUBCONTROL( QskMenu, Cell )
QSK_SUBCONTROL( QskMenu, Cursor )
QSK_SUBCONTROL( QskMenu, Text )
QSK_SUBCONTROL( QskMenu, Graphic )

QSK_SYSTEM_STATE( QskMenu, Selected, QskAspect::FirstSystemState << 2 )

class QskMenu::PrivateData
{
  public:
    QVector< Entry > entries;
    QVector< QskGraphic > icons;

    QskTextOptions textOptions;
    QPointF origin;

    int currentIndex = 0;
    bool isPressed = false;
};

QskMenu::QskMenu( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setModal( true );
    setOverlay( true );

    setFaderAspect( QskMenu::Panel | QskAspect::Position | QskAspect::Metric );

    setPopupFlag( QskPopup::CloseOnPressOutside, true );
    setPopupFlag( QskPopup::DeleteOnClose, true );

    initSizePolicy( QskSizePolicy::Fixed, QskSizePolicy::Fixed );
}

QskMenu::~QskMenu()
{
}

void QskMenu::setOrigin( const QPointF& origin )
{
    if ( origin != m_data->origin )
    {
        m_data->origin = origin;
        Q_EMIT originChanged( origin );
    }
}

QPointF QskMenu::origin() const
{
    return m_data->origin;
}

void QskMenu::addItem( const QUrl& graphicSource, const QString& text )
{
    m_data->entries += { graphicSource, text };

#if 1
    {
        const auto& entries = m_data->entries;

        m_data->icons.clear();
        m_data->icons.reserve( entries.count() );

        for( const auto& entry : entries )
        {
            QskGraphic graphic;

            if( !entry.iconSource.isEmpty() )
                graphic = Qsk::loadGraphic( entry.iconSource );

            m_data->icons += graphic;
        }
    }
#endif

    resetImplicitSize();
    update();
}

void QskMenu::addItem( const QString& graphicSource, const QString& text )
{
    addItem( QUrl( graphicSource ), text );
}

void QskMenu::addSeparator()
{
    // TODO ...
}

void QskMenu::clear()
{
    m_data->icons.clear();
    m_data->entries.clear();
}

int QskMenu::count() const
{
    return m_data->entries.count();
}

QskMenu::Entry QskMenu::entryAt( int index ) const
{
    if( index >= 0 && index <= m_data->entries.count() )
    {
        return m_data->entries[ index ];
    }

    return Entry();
}

QskGraphic QskMenu::graphicAt( int index ) const
{
    if( index >= 0 && index <= m_data->icons.count() )
    {
        return m_data->icons[ index ];
    }

    return QskGraphic();
}

void QskMenu::setTextOptions( const QskTextOptions& textOptions )
{
    if( textOptions != m_data->textOptions )
    {
        m_data->textOptions = textOptions;
        update();
    }
}

QskTextOptions QskMenu::textOptions() const
{
    return m_data->textOptions;
}

void QskMenu::setCurrentIndex( int index )
{
    if( index >= 0 && index < count()
        && ( index != m_data->currentIndex ) )
    {
        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged( index );
    }
}

int QskMenu::currentIndex() const
{
    return m_data->currentIndex;
}

void QskMenu::keyPressEvent( QKeyEvent* event )
{
    if( m_data->currentIndex < 0 )
    {
        return;
    }

    int key = event->key();

    switch( key )
    {
        case Qt::Key_Up:
        {
            traverse( -1 );
            break;
        }

        case Qt::Key_Down:
        {
            traverse( 1 );
            break;
        }

        case Qt::Key_Select:
        case Qt::Key_Space:
        {
            m_data->isPressed = true;
            return;
        }

        case Qt::Key_Escape:
        case Qt::Key_Cancel:
        {
            setSelectedIndex( -1 );
            return;
        }

        default:
        {
            return;
        }
    }
}

void QskMenu::keyReleaseEvent( QKeyEvent* )
{
    if( m_data->isPressed )
    {
        m_data->isPressed = false;
        setSelectedIndex( m_data->currentIndex );
    }
}

void QskMenu::wheelEvent( QWheelEvent* event )
{
#if QT_VERSION < 0x050e00
    const int delta = event->delta();
#else
    const int delta = event->angleDelta().y();
#endif

    // we ignore the step count and use its direction only
    traverse( delta < 0 ? 1 : -1 );
}

void QskMenu::traverse( int steps )
{
    auto& index = m_data->currentIndex;

    index = ( index + steps ) % count();
    if ( index < 0 )
        index += count();

    update();
    Q_EMIT currentIndexChanged( index );
}

int QskMenu::indexAtPosition( const QPointF& pos ) const
{
    for ( int i = 0; i < count(); i++ )
    {
        // A menu never has many cells and we can simply iterate
        if ( cellRect( i ).contains( pos ) )
            return i;
    }

    return -1;
}

QskColorFilter QskMenu::graphicFilterAt( int index ) const
{
    Q_UNUSED( index );
    return QskColorFilter();
}

void QskMenu::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        const auto index = indexAtPosition( event->localPos() );
        if ( index >= 0 )
        {
            setCurrentIndex( index );
            m_data->isPressed = true;
        }
    }
}

void QskMenu::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        if( m_data->isPressed )
        {
            m_data->isPressed = false;

            const auto index = indexAtPosition( event->localPos() );
            if ( index == m_data->currentIndex )
                setSelectedIndex( index );
        }
    }
}

void QskMenu::aboutToShow()
{
    setGeometry( QRectF( m_data->origin, sizeConstraint() ) );
    Inherited::aboutToShow();
}

QRectF QskMenu::focusIndicatorRect() const
{
    // highlighting the item is good enough
    return QRectF();
}

void QskMenu::setSelectedIndex( int index )
{
    if ( !isOpen() )
        return;

    if ( index >= 0 )
        setCurrentIndex( index );

    Q_EMIT triggered( index );
    close();
}

QRectF QskMenu::cellRect( int index ) const
{
    /*
        Calculations like this one should be done in the skinlet.
        We need an API like subControlRect( int index, ... ) TODO ...
     */

    if ( index < 0 || index >= count() )
        return QRectF();

    const auto r = subControlContentsRect( QskMenu::Panel );
    const auto h = cellHeight();

    return QRectF( r.x(), r.y() + index * h, r.width(), h );
}

qreal QskMenu::cellHeight() const
{
    /*
        Calculations like this one should be done in the skinlet.
        We need an API like subControlRect( int index, ... ) TODO ...
     */
    const auto graphicHeight = strutSizeHint( Graphic ).height();
    const auto textHeight = effectiveFontHeight( Text );
    const auto padding = paddingHint( Cell );

    qreal h = qMax( graphicHeight, textHeight );
    h += padding.top() + padding.bottom();

    const auto minHeight = strutSizeHint( Cell ).height();
    h = qMax( h, minHeight );

    return h;
}

#include "moc_QskMenu.cpp"
