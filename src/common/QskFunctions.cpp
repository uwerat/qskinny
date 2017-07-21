#include "QskFunctions.h"

#include <QGuiApplication>
#include <QScreen>
#include <QtMath>

template< class Rect, class Value >
static inline Rect qskAlignedRect( const Rect& outerRect,
    Value width, Value height, Qt::Alignment alignment )
{
    // we might need this code at other place too ???

    Rect r( 0, 0, width, height );

    switch( alignment & Qt::AlignHorizontal_Mask )
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
            r.moveLeft( outerRect.center().x() - width / 2 );
        }
    }

    switch( alignment & Qt::AlignVertical_Mask )
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
            r.moveTop( outerRect.center().y() - height / 2 );
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
