#include "QskPlatform.h"

#include <qguiapplication.h>
#include <qscreen.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

qreal qskDpiScaled( qreal value )
{
    static qreal factor = 0.0;

    if ( factor <= 0.0 )
    {
        if ( const auto screen = QGuiApplication::primaryScreen() )
            factor = screen->logicalDotsPerInchX();
        else
            factor = 100.0;

        factor /= 96.0;
    }

    return value * factor;
}

qreal qskGlobalScaleFactor()
{
    // The value of QT_SCALE_FACTOR
    const QScreen* noScreen = nullptr;
    return QHighDpiScaling::factor( noScreen );
}

const QPlatformIntegration* qskPlatformIntegration()
{
    return QGuiApplicationPrivate::platformIntegration();
}

bool qskMaybeDesktopPlatform()
{
#if QT_CONFIG(cursor)
    if ( const auto platform = QGuiApplicationPrivate::platformIntegration() )
        return platform->hasCapability( QPlatformIntegration::MultipleWindows );
#endif

    return false;
}

QRect qskPlatformScreenGeometry( const QScreen* screen )
{
    if ( screen == nullptr )
        return QRect();

    return screen->handle()->geometry();
}


