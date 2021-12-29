#include "QskMenu.h"

#include <QskGraphicProvider.h>
#include <QskTextOptions.h>
#include <QskGraphic.h>
#include <QskColorFilter.h>
#include <QskSkinlet.h>
#include <QskEvent.h>

#include <qvector.h>
#include <qvariant.h>

QSK_SUBCONTROL( QskMenu, Panel )
QSK_SUBCONTROL( QskMenu, Cell )
QSK_SUBCONTROL( QskMenu, Cursor )
QSK_SUBCONTROL( QskMenu, Text )
QSK_SUBCONTROL( QskMenu, Graphic )

QSK_SYSTEM_STATE( QskMenu, Selected, QskAspect::FirstSystemState << 2 )

namespace
{
    struct Entry
    {
        QUrl graphicSource;
        QskGraphic graphic;
        QString text;
    };
}

class QskMenu::PrivateData
{
  public:
    QVector< Entry > entries;

    QskTextOptions textOptions;
    QPointF origin;

    int currentIndex = -1;
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

bool QskMenu::isCascading() const
{
    return flagHint( QskMenu::Panel | QskAspect::Style );
}

void QskMenu::setCascading( bool on )
{
    if ( setFlagHint( QskMenu::Panel | QskAspect::Style, on ) )
        Q_EMIT cascadingChanged( on );
}

void QskMenu::resetCascading()
{
    if ( resetFlagHint( QskMenu::Panel | QskAspect::Style ) )
        Q_EMIT cascadingChanged( isCascading() );
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
    QskGraphic graphic;
    if( !graphicSource.isEmpty() )
        graphic = Qsk::loadGraphic( graphicSource );

    m_data->entries += { graphicSource, graphic, text };

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        countChanged( count() );
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
    m_data->entries.clear();
}

int QskMenu::count() const
{
    return m_data->entries.count();
}

QVariant QskMenu::itemAt( int index ) const
{
    const auto& entries = m_data->entries;

    if( index < 0 || index >= entries.count() )
        return QVariant();

    const auto& entry = m_data->entries[ index ];

    QVariantList list;
    list += QVariant::fromValue( entry.graphic );
    list += QVariant::fromValue( entry.text );

    return list;
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
    if( index < 0 || index >= count() )
        index = -1;

    if( index != m_data->currentIndex )
    {
        setPositionHint( Cursor, index );

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
    if ( count() == 0 || ( steps % count() == 0 ) )
        return;

    auto index = m_data->currentIndex + steps;

    auto newIndex = index % count();
    if ( newIndex < 0 )
        newIndex += count();

    if ( hasAnimationHint( Cursor | QskAspect::Metric ) )
    {
        // when cycling we want slide in 

        if ( index < 0 )
            setPositionHint( Cursor, count() );

        if ( index >= count() )
            setPositionHint( Cursor, -1 );
    
        movePositionHint( Cursor, newIndex );
    }

    setCurrentIndex( newIndex );
}

QskColorFilter QskMenu::graphicFilterAt( int index ) const
{
    Q_UNUSED( index )
    return QskColorFilter();
}

void QskMenu::mousePressEvent( QMouseEvent* event )
{
    // QGuiApplication::styleHints()->setFocusOnTouchRelease ??

    if ( event->button() == Qt::LeftButton )
    {
        const auto index = indexAtPosition( qskMousePosition( event ) );
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

            const auto index = indexAtPosition( qskMousePosition( event ) );
            if ( index == m_data->currentIndex )
                setSelectedIndex( index );
        }
    }
}

void QskMenu::aboutToShow()
{
    setGeometry( QRectF( m_data->origin, sizeConstraint() ) );

    if ( m_data->currentIndex < 0 )
        setCurrentIndex( 0 );

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
    return effectiveSkinlet()->sampleRect(
        this, contentsRect(), QskMenu::Cell, index );
}

int QskMenu::indexAtPosition( const QPointF& pos ) const
{
    return effectiveSkinlet()->sampleIndexAt(
        this, contentsRect(), QskMenu::Cell, pos );
}

#include "moc_QskMenu.cpp"
