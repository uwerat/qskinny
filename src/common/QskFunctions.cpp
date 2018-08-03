#include "QskFunctions.h"

#include <qguiapplication.h>
#include <qmath.h>
#include <qscreen.h>

template< class Rect, class Value >
static inline Rect qskAlignedRect( const Rect& outerRect,
    Value width, Value height, Qt::Alignment alignment )
{
    // we might need this code at other place too ???

    Rect r( 0, 0, width, height );

    switch ( alignment & Qt::AlignHorizontal_Mask )
    {
        case Qt::AlignLeft:
        {
            r.moveLeft( outerRect.left() );
            break;
        }
        case Qt::AlignRight:
        {
            r.moveRight( outerRect.right() );
            break;
        }
        default:
        {
            const auto dx = ( outerRect.width() - width ) / 2;
            r.moveLeft( outerRect.left() + dx );
        }
    }

    switch ( alignment & Qt::AlignVertical_Mask )
    {
        case Qt::AlignTop:
        {
            r.moveTop( outerRect.top() );
            break;
        }
        case Qt::AlignBottom:
        {
            r.moveBottom( outerRect.bottom() );
            break;
        }
        default:
        {
            const auto dy = ( outerRect.height() - height ) / 2;
            r.moveTop( outerRect.top() + dy );
        }
    }

    return r;
}

QRect qskAlignedRect( const QRect& outerRect,
    int width, int height, Qt::Alignment alignment )
{
    return qskAlignedRect< QRect, int >( outerRect, width, height, alignment );
}

QRectF qskAlignedRectF( const QRectF& outerRect,
    qreal width, qreal height, Qt::Alignment alignment )
{
    return qskAlignedRect< QRectF, qreal >( outerRect, width, height, alignment );
}

QRect qskInnerRect( const QRectF& rect )
{
    const int left = qCeil( rect.left() );
    const int top = qCeil( rect.top() );
    const int right = qFloor( rect.right() );
    const int bottom = qFloor( rect.bottom() );

    return QRect( left, top, right - left, bottom - top );
}

QRectF qskValidOrEmptyInnerRect( const QRectF& rect, const QMarginsF& margins )
{
    qreal x, y, h, w;

    const qreal marginsWidth = margins.left() + margins.right();
    const qreal marginsHeight = margins.top() + margins.bottom();

    if ( marginsWidth > rect.width() )
    {
        x = rect.x() + rect.width() * ( margins.left() / marginsWidth );
        w = 0;
    }
    else
    {
        x = rect.x() + margins.left();
        w = rect.width() - marginsWidth;
    }

    if ( marginsHeight > rect.height() )
    {
        y = rect.y() + rect.height() * ( margins.top() / marginsHeight );
        h = 0;
    }
    else
    {
        y = rect.y() + margins.top();
        h = rect.height() - marginsHeight;
    }

    return QRectF( x, y, w, h );
}

qreal qskDpiScaled( qreal value )
{
    static qreal factor = 0.0;

    if ( factor <= 0.0 )
    {
        if ( const QScreen* screen = QGuiApplication::primaryScreen() )
            factor = screen->logicalDotsPerInchX();
        else
            factor = 100.0;

        factor /= 96.0;
    }

    return value * factor;
}
