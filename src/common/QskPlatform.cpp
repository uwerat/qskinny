#include "QskPlatform.h"

#include <qguiapplication.h>
#include <qscreen.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qguiapplication_p.h>
#include <private/qhighdpiscaling_p.h>
QSK_QT_PRIVATE_END

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

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

static inline qreal qskRoundedDpi( qreal dpi )
{   
    // see https://developer.android.com/training/multiscreen/screendensities
    
    if( dpi <= 140.0 )
        return 120.0; // ldpi
    
    if( dpi <= 200.0 )
        return 160.0; // mdpi
    
    if( dpi <= 280.0 )
        return 240.0; // hdpi
    
    if( dpi <= 400.0 )
        return 320.0; // xhdpi
    
    if( dpi <= 560.0 )
        return 480.0; // xxhdpi
    
    return 640.0; // xxxhdpi
}   

qreal qskDpToPixelsFactor()
{
    if ( const auto screen = QGuiApplication::primaryScreen() )
    {
        // see: https://en.wikipedia.org/wiki/Device-independent_pixel
        return qskRoundedDpi( screen->physicalDotsPerInch() ) / 160.0;
    }

    return 1.0;
}
